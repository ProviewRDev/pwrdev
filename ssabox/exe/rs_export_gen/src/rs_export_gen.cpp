#include <cstdint>
#include <algorithm>
#include <unistd.h>
#include <sstream>
#include <vector>

#include <glib.h>

#include "pwr.h"
#include "pwr_baseclasses.h"
#include "rt_gdh.h"
#include "rt_gdh_msg.h"
#include "rt_errh.h"
#include "co_cdh.h"
#include "co_dcli.h"
#include "co_error.h"
#include "co_string.h"

static char json_filename[] = "$pwrp_load/select.json";
static int only_signals = 1;

std::vector<std::string> tmp_array;

std::string aref_to_str(pwr_tAttrRef &aref) {
  return "{\"Objid\":{\"oix\":" + std::to_string(aref.Objid.oix) + ",\"vid\":" + std::to_string(aref.Objid.vid) + "},\"Body\":" + std::to_string(aref.Body) + ",\"Offset\":" + std::to_string(aref.Offset) + ",\"Size\":" + std::to_string(aref.Size) + ",\"Flags\":" + std::to_string(aref.Flags.m) + "}";
}

void printObjectR(char* ap, char* aname, pwr_tAttrRef* arp, pwr_tCid cid) {
  gdh_sAttrDef* bd;
  int rows;
  pwr_tStatus sts = gdh_GetObjectBodyDef(cid, &bd, &rows, arp->Objid);
  if (EVEN(sts)) throw co_error(sts);

  for (int i = 0; i < rows; i++) {
    pwr_sParInfo pari = bd[i].attr->Param.Info;

    if (only_signals && !(pari.Flags & PWR_MASK_CLASS) && strcmp(bd[i].attrName, "ActualValue") != 0)
      continue;

    if (pari.Flags & PWR_MASK_RTVIRTUAL || (pari.Flags & PWR_MASK_PRIVATE && pari.Flags & PWR_MASK_POINTER) || pari.Type == pwr_eType_Void)
      continue;

    pwr_tOName name, attrName;
    strcpy(name, aname);
    strcat(name, ".");
    strcat(name, bd[i].attrName);
    strcpy(attrName, bd[i].attrName);

    int elements = 1;
    if (pari.Flags & PWR_MASK_ARRAY)
      elements = pari.Elements;

    for (int j = 0; j < elements; j++) {
      if (pari.Flags & PWR_MASK_ARRAY) {
        char idx[20];
        sprintf(idx, "[%d]", j);
        strcpy(name, aname);
        strcat(name, ".");
        strcat(name, bd[i].attrName);
        strcat(name, idx);
        strcpy(attrName, bd[i].attrName);
        strcat(attrName, idx);
      }
      pwr_tAttrRef aref;
      sts = gdh_ArefANameToAref(arp, attrName, &aref);
      if (EVEN(sts)) throw co_error(sts);

      if (bd[i].attr->Param.Info.Flags & PWR_MASK_CLASS) {
        printObjectR(ap + pari.Offset + j * pari.Size / elements, name, &aref, pari.Type);
      } else {
        std::string tmp = "{\"name\":\"" + std::string(name) + "\",\"aref\":" + aref_to_str(aref) + ",\"type\":" + std::to_string(pari.Type) + ",\"flags\":" + std::to_string(pari.Flags) + ",\"enable\":1}";
        tmp_array.push_back(tmp);
      }
    }
  }
}

void printObject(pwr_tAttrRef* arp, char* aname) {
  pwr_tTid tid;
  pwr_tStatus sts = gdh_GetAttrRefTid(arp, &tid);
  if (EVEN(sts)) throw co_error(sts);

  if (arp->Flags.b.Object && arp->Size == 0) {
    sts = gdh_GetObjectSize(arp->Objid, &arp->Size);
    if (EVEN(sts)) throw co_error(sts);
  } else if (arp->Size == 0) {
    throw co_error(GDH__BADARG);
  }

  char* ap = (char*)calloc(1, arp->Size);
  memset(ap, 0, arp->Size);

  sts = gdh_GetObjectInfoAttrref(arp, ap, arp->Size);
  if (EVEN(sts)) {
    fprintf(stderr, "Couldn't get object info attr ref for object %s\n", aname);
    throw co_error(sts);
  }

  printObjectR(ap, aname, arp, tid);

  free(ap);
}

void dfs_helper(pwr_tOid oid) {
  pwr_tOName name;
  pwr_tStatus sts = gdh_ObjidToName(oid, name, sizeof(name), cdh_mName_volumeStrict);
  if (EVEN(sts)) throw co_error(sts);

  pwr_tAttrRef aref = cdh_ObjidToAref(oid);

  printObject(&aref, name);

  pwr_tOid coid;
  pwr_tStatus sts2 = gdh_GetChild(oid, &coid);
  while (ODD(sts2)) {
    dfs_helper(coid);
    sts2 = gdh_GetNextSibling(coid, &coid);
  }
}

void usage() {
  printf("rs_export_gen\n\n"
	 "-a Generate all attributes, else only signals\n\n");
}

int main(int argc, char**argv) {

  if (argc > 1 && streq(argv[1], "-h")) {
    usage();
    exit(0);      
  }


  if (argc > 1 && streq(argv[1], "-a"))
    only_signals = 0;
      
  pwr_tFileName fname;
  dcli_translate_filename(fname, json_filename);
  errh_Interactive();

  pwr_tStatus sts = gdh_Init("java_native");
  if (EVEN(sts)) {
    fprintf(stderr, "gdh_Init failed\n");
    return sts;
  }

  std::ostringstream json_string;
  json_string << "{\n";
  json_string << "  \"frequency\": 1,\n";
  json_string << "  \"batches\": 1,\n";
  json_string << "  \"signals\": [\n";

  pwr_tOid oid;
  sts = gdh_GetRootList(&oid);
  while (ODD(sts)) {
    dfs_helper(oid);
    sts = gdh_GetNextSibling(oid, &oid);
  }
  std::sort(std::begin(tmp_array), std::end(tmp_array));
  for (int i = 0; i < tmp_array.size(); i++) {
    json_string << "    " << tmp_array[i];
    if (i < tmp_array.size() - 1) {
      json_string << ",";
    }
    json_string << "\n";
  }
  json_string << "  ]\n";
  json_string << "}";

  FILE* fp = fopen(fname, "w");
  if (!fp) return 1;
  fputs(json_string.str().c_str(), fp);
  fclose(fp);

  printf("%s generated with %d signals\n", fname, (int)tmp_array.size());

  return 0;
}
