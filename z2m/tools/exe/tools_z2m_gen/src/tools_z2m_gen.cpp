#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector>
#include <string>
#include <algorithm>

#include "co_cdh.h"
#include "co_string.h"
#include "co_dcli.h"

#define Z2M__SUCCESS 1
#define Z2M__ENUMTOOLONG 2
#define Z2M__NOFILE 4

#define EVEN(a) (((int)(a)&1) == 0)

typedef enum {
  z2m_eChanType_,
  z2m_eChanType_Do,
  z2m_eChanType_Eo,
  z2m_eChanType_Io,
  z2m_eChanType_Ao,
  z2m_eChanType_So,
  z2m_eChanType_Di,
  z2m_eChanType_Ei,
  z2m_eChanType_Ii,
  z2m_eChanType_Ai,
  z2m_eChanType_Si,
  z2m_eChanType_Composite
} z2m_eChanType;

typedef enum {
  z2m_mAccess_pubic = 1,
  z2m_mAccess_set = 2,
  z2m_mAccess_get = 4
} z2m_mAccess;

typedef enum {
  z2m_eVtype_binary,
  z2m_eVtype_numeric,
  z2m_eVtype_enum,
  z2m_eVtype_list,
  z2m_eVtype_composite,
  z2m_eVtype_string,
  z2m_eVtype_unknown
} z2m_eVtype;

typedef enum {
  z2m_eZtype_option,
  z2m_eZtype_expose
} z2m_eZtype;

typedef enum {
  z2m_eGroup_other,
  z2m_eGroup_light,
  z2m_eGroup_switch,
  z2m_eGroup_occupancy,
  z2m_eGroup_temperature,
  z2m_eGroup_climate,
  z2m_eGroup_water_leak,
  z2m_eGroup_contact,
  z2m_eGroup_smoke,
  z2m_eGroup_cover,
  z2m_eGroup_action
} z2m_eGroup;

typedef enum {  
  z2m_eSigFilter_,
  z2m_eSigFilter_Prio1,
  z2m_eSigFilter_Prio2,
  z2m_eSigFilter_Prio3,
  z2m_eSigFilter_Hide
} z2m_eSigFilter;

class z2m_enumtype {
public:
  z2m_enumtype() {
    strcpy(name, "");
    strcpy(vendor, "");
    strcpy(type, "");
    strcpy(enumvalues, "");
  }
  z2m_enumtype(const z2m_enumtype& x) {
    strcpy(name, x.name);
    strcpy(vendor, x.vendor);
    strcpy(type, x.type);
    strcpy(enumvalues, x.enumvalues);
  }
  char name[80];
  char vendor[80];
  char type[80];
  char enumvalues[1100];
};

class z2m_submodule {
public:
  z2m_submodule() : device_idx(0), signal_idx(0) {
    strcpy(name, "");
    strcpy(ioname, "");
    strcpy(vendor, "");
    strcpy(features, "");
  }
  z2m_submodule(const z2m_submodule& x) : device_idx(x.device_idx), 
     signal_idx(x.signal_idx) {
    strcpy(name, x.name);
    strcpy(ioname, x.ioname);
    strcpy(vendor, x.vendor);
    strcpy(features, x.features);
  }
  char name[80];
  char ioname[80];
  char vendor[80];
  char features[500];
  int device_idx;
  int signal_idx;
};

class z2m_signal {
public:
  z2m_signal() : status(Z2M__SUCCESS), ztype(0), vtype(0), chantype(0),
      fs_has_state(0), fs_has_brightness(0), access(0), minvalue(0), maxvalue(0), 
      stepvalue(0), filter(z2m_eSigFilter_), next_aix(1), no_of_decimals(0) {
    strcpy(signalname, "");
    strcpy(featuresname, "");
    strcpy(ioclassname, "");
    strcpy(name, "");
    strcpy(label, "");
    strcpy(attrname, "");
    strcpy(foattrname, "");
    strcpy(description, "");
    strcpy(features_str, "");
    strcpy(type, "");
    strcpy(property, "");
    strcpy(onvalue, "");
    strcpy(offvalue, "");
    strcpy(togglevalue, "");
    strcpy(enumvalues, "");
    strcpy(listvalues, "");
    strcpy(endpoint, "");
    strcpy(unit, "");
    strcpy(enumtype, "");
    strcpy(submodule, "");
    strcpy(iosubmodule, "");
  }
  z2m_signal(const z2m_signal& x) : status(x.status), ztype(x.ztype), 
      vtype(x.vtype), chantype(x.chantype), fs_has_state(x.fs_has_state), 
      fs_has_brightness(x.fs_has_brightness), access(x.access), 
      minvalue(x.minvalue), maxvalue(x.maxvalue), stepvalue(x.stepvalue),
      features(x.features), filter(x.filter), next_aix(x.next_aix), 
      no_of_decimals(x.no_of_decimals), doc(x.doc)
  {
    strcpy(signalname, x.signalname);
    strcpy(featuresname, x.featuresname);
    strcpy(ioclassname, x.ioclassname);
    strcpy(name, x.name);
    strcpy(label, x.label);
    strcpy(attrname, x.attrname);
    strcpy(foattrname, x.foattrname);
    strcpy(description, x.description);
    strcpy(features_str, x.features_str);
    strcpy(type, x.type);
    strcpy(property, x.property);
    strcpy(onvalue, x.onvalue);
    strcpy(offvalue, x.offvalue);
    strcpy(togglevalue, x.togglevalue);
    strcpy(enumvalues, x.enumvalues);
    strcpy(listvalues, x.listvalues);
    strcpy(endpoint, x.endpoint);
    strcpy(unit, x.unit);
    strcpy(enumtype, x.enumtype);
    strcpy(submodule, x.submodule);
    strcpy(iosubmodule, x.iosubmodule);
  }
  int status;
  int ztype;
  int vtype;
  int chantype;
  int fs_has_state;
  int fs_has_brightness;
  char signalname[80];
  char featuresname[80];
  char ioclassname[80];
  char name[80];
  char label[80];
  char attrname[200];
  char foattrname[32];
  char description[200];
  char features_str[500];
  char type[20];
  int access;
  char property[80];
  char onvalue[80];
  char offvalue[80];
  char togglevalue[80];
  char enumvalues[1100];
  char listvalues[200];
  char endpoint[80];
  int minvalue;
  int maxvalue;
  int stepvalue;
  char unit[20];
  char enumtype[80];
  char submodule[80];
  char iosubmodule[80];
  std::vector<z2m_signal> features;
  z2m_eSigFilter filter;
  int next_aix;
  int no_of_decimals;
  std::string doc;
};

class z2m_device {
public:
  z2m_device() : next_aix(1), has_scantime(0), has_state(0), has_brightness(0),
      has_transition(0), group(z2m_eGroup_other), max_enum_size(0) {
    strcpy(vendor, "");
    strcpy(model, "");
    strcpy(classname, "");
    strcpy(ioclassname, "");
    strcpy(foclassname, "");
    strcpy(description, "");
    strcpy(exposes, "");
  }
  z2m_device(const z2m_device& x) : next_aix(x.next_aix), has_scantime(x.has_scantime), 
      has_state(x.has_state), has_brightness(x.has_brightness), 
      has_transition(x.has_transition), group(x.group), 
      max_enum_size(x.max_enum_size), signals(x.signals),
      doc(x.doc) {
    strcpy(vendor, x.vendor);
    strcpy(model, x.model);
    strcpy(classname, x.classname);
    strcpy(ioclassname, x.ioclassname);
    strcpy(foclassname, x.foclassname);
    strcpy(description, x.description);
    strcpy(exposes, x.exposes);
  }
  int next_aix;
  int has_scantime;
  int has_state;
  int has_brightness;
  int has_transition;
  z2m_eGroup group;
  char vendor[80];
  char model[80];
  char classname[32];
  char ioclassname[32];
  char foclassname[32];
  char description[200];
  char exposes[200];
  int max_enum_size;
  FILE *fpog;
  std::vector<z2m_signal> signals;
  std::string doc;
};

class z2m {
public:
  std::vector<std::string> classnamevect;
  std::vector<z2m_enumtype> enumtypes;
  std::vector<z2m_submodule> submodules;
  int next_cix;
  int next_tix;
  char infile[200];
  char outfile[200];
  char outfile_cix[200];
  char outfile_c[200];
  char outfile_h[200];
  char outdir_c[200];
  char outdir_ge[200];
  char volume[80];
  char volid[20];
  char filter[20][100];
  int filter_cnt;
  int doc;
  int errlog;
  FILE *fpo;
  FILE *fpocix;
  FILE *fpoc;
  FILE *fpoh;
  std::vector<z2m_device> devices;

  z2m() : next_cix(1),  next_tix(1), filter_cnt(0), doc(1), errlog(0) {}
  int read_line(char* line, int maxsize, FILE* file);
  int device_check(z2m_device *dp);
  void trim_graphname(char *name);
  char *cut_name(char *name, int size);
  void get_signalname(z2m_signal *sp);
  void get_foattrname(z2m_signal *sp, z2m_signal *psp);
  char *get_foattrvaluename(char *name, char *vname);
  void get_featuresname(z2m_signal *sp);
  void get_ioclassname(z2m_signal *sp);
  void get_vendorname(z2m_device *dp, char *str);
  int get_classname(z2m_device *dp);
  char *pwr_name(char *name, int keep_par = 0);
  void print_volume();
  void print_volume_end();
  void print_device(z2m_device *dp);
  void print_signal(z2m_device *dp, z2m_signal *sp, int *next_aix, int has_brightness);
  void print_input(z2m_device *dp, z2m_signal *sp, int *input_cnt);
  void print_output(z2m_device *dp, z2m_signal *sp, int *output_cnt);
  void print_features(z2m_device *dp, z2m_signal *sp, int *next_aix);
  void print_features_end(z2m_device *dp, z2m_signal *sp, int *next_aix);
  void print_device_end(z2m_device *dp);
  void print_enumtypes();
  void print_submodules();
  void print_doc(std::string& doc, int is_signal);
  void error_msg(int sts, int line_cnt, z2m_device *dp, z2m_signal *sp);
  int get_enumtypes();
  int check_enum(z2m_device *dp, z2m_signal *sp); 
  int get_submodules();
  int read_file();
  char *uname(char *name); 
  void get_device_group(z2m_device *dp);
  int is_integer_chan(z2m_signal *sp);
  void check_signals(z2m_device *dp);
  void graph_device(z2m_device *dp);
  void graph_device_start(z2m_device *dp);
  void graph_device_end(z2m_device *dp);
  void graph_signal(z2m_device *dp, z2m_signal *sp, z2m_eSigFilter filter);
  void graph_features(z2m_device *dp, z2m_signal *sp);
  void graph_features_end(z2m_device *dp, z2m_signal *sp);
  void graph_signal_filter(z2m_signal *sp);
  void graph_signals_filter(z2m_device *dp);
  void sim_graph_device(z2m_device *dp);
  void sim_graph_device_start(z2m_device *dp);
  void sim_graph_device_end(z2m_device *dp);
  void sim_graph_signal(z2m_device *dp, z2m_signal *sp, z2m_eSigFilter filter);
  void sim_graph_features(z2m_device *dp, z2m_signal *sp);
  void sim_graph_features_end(z2m_device *dp, z2m_signal *sp);
  void print_stat();
  void print_stat_signal(z2m_signal *sp);
};

void z2m::print_stat_signal(z2m_signal *sp)
{
  int num;

  if (sp->vtype == z2m_eVtype_numeric) {
    if (sp->chantype == z2m_eChanType_Io ||
	sp->chantype == z2m_eChanType_Ao)
      printf("I ");
    else
      printf("O ");
    
    num = printf("%s ", sp->name);
    for (int i = num; i < 20; i++)
      printf(" ");

    printf("%6d %6d %1d\n", sp->minvalue, sp->maxvalue, sp->no_of_decimals);
  }
}

void z2m::print_stat()
{
  for (unsigned int i = 0; i < devices.size(); i++) {
    for (unsigned int j = 0; j < devices[i].signals.size(); j++) {
      z2m_signal *sp = &devices[i].signals[j];
      print_stat_signal(sp);

      if (sp->features.size() > 0) {
	for (unsigned int k = 0; k < sp->features.size(); k++) {
	  z2m_signal *ssp = &sp->features[k];
	  print_stat_signal(ssp);
	}
      }
    }
  }
}

int z2m::device_check(z2m_device *dp)
{
  if (strcmp(dp->vendor, "DIYRuZ") == 0 &&
      strcmp(dp->model, "DIYRuZ_Geiger") == 0)
    return 0;

  return 1;
}

int z2m::read_line(char* line, int maxsize, FILE* file)
{
  char* s;

  if (fgets(line, maxsize, file) == NULL)
    return 0;
  line[maxsize - 1] = 0;
  s = strchr(line, 10);
  if (s != 0)
    *s = 0;
  s = strchr(line, 13);
  if (s != 0)
    *s = 0;

  return 1;
}

void z2m::trim_graphname(char *name)
{
  char str[80];
  char str2[80];
  char *s;

  if (strlen(name) < 16)
    return;

  strcpy(str, name);
  strcpy(str2, name);
  s = strchr(str2, '_');
  if (s) {
    str[3] = 0;
    strcat(str, s);

    if (strlen(str) >= 16) {
      s = strchr(s+1, '_');
      if (s) {
	str[6] = 0;
	strcat(str, s);
      }

      if (strlen(str) >= 16) {
	s = strchr(s+1, '_');
	if (s) {
	  str[9] = 0;
	  strcat(str, s);
	}
      }
    }
  }

  if (strlen(str) >= 16)
    str[15] = 0;

  strcpy(name, str);
}

char *z2m::cut_name(char *name, int size)
{
  static char sname[200];
  char str[80];
  int len, slen;

  len = strlen(name);
  if ( len > size - 1) {
    char valuename[10][40];
    int valuelen[10];
    int tlen[10];
    int vlen;
    int cut = 0;

    int num = dcli_parse(name, "_", "", (char*)valuename,
	sizeof(valuename) / sizeof(valuename[0]), sizeof(valuename[0]),
        0);
    
    memset(&tlen, 0, sizeof(tlen));
    for (int i = num - 1; i >= 0; i--) {
      valuelen[i] = strlen(valuename[i]);
      if (i == num - 1)
	tlen[i] = valuelen[i] + 1;
      else if (i == 0)
	tlen[i] = valuelen[i] + tlen[i+1];
      else
	tlen[i] = valuelen[i] + tlen[i+1] + 1;
    }

    str[0] = 0;
    for (int i = 0; i < num - 1; i++) {
      vlen = (size - 1) - strlen(str);
      int diff = vlen - tlen[i+1];
      //printf("str: %d  left %d vlen %d tlen %d diff %d\n", strlen(str), 31 - strlen(str), vlen, tlen[i+1], diff);
      if (diff >= 3) {
	slen = strlen(str);
	strncat(str, valuename[i], diff);
	str[slen + diff] = 0;
	for (int j = i+1; j < num; j++) {
	  strcat(str, "_");
	  strcat(str, valuename[j]);
	}
	cut = 1;	
	break;
      }
      else {
	slen = strlen(str);
	strncat(str, valuename[i], 2);
	str[slen + 2] = 0;
	strcat(str, "_");
      }
    }
    if (cut) {
      strcpy(sname, str);
    } else {
      vlen = (size - 1) - strlen(str);
      if (tlen[num-1] - 1 <= vlen) {
	strcat(str, valuename[num-1]);
	strcpy(sname, str);
      } else {
	strcpy(sname, name);
	sname[size - 1] = 0;
      }
    }
  }
  else {
    strcpy(sname, name);
    sname[size - 1] = 0;
  }

  return sname;
}

void z2m::get_signalname(z2m_signal *sp)
{
  strcpy(sp->signalname, cut_name(pwr_name(sp->name), 32));
  sp->signalname[31] = 0;
}

void z2m::get_featuresname(z2m_signal *sp)
{
  strcpy(sp->featuresname, pwr_name(sp->name));
  sp->featuresname[29] = 0;
  strcat(sp->featuresname, "Fs");
}

void z2m::get_foattrname(z2m_signal *sp, z2m_signal *psp)
{
  if (!psp) {
    strcpy(sp->foattrname, sp->signalname);
    strcpy(sp->attrname, sp->signalname);
  }
  else {
    int len = 32 - strlen(psp->name) - 3;
    if (len < 5)
      len = 5;
      
    strcpy(sp->foattrname, cut_name(psp->signalname, len+1));
    sp->foattrname[len] = 0;
    strcat(sp->foattrname, "_");
    len = strlen(sp->foattrname);
    strncat(sp->foattrname, sp->signalname, sizeof(sp->foattrname) - len);
    sp->foattrname[sizeof(sp->foattrname)-1] = 0;

    strcpy(sp->attrname, psp->signalname);
    strcat(sp->attrname, ".");
    strcat(sp->attrname, sp->signalname);
  }
}

char *z2m::get_foattrvaluename(char *name, char *vname)
{
  static char aname[200];

  strcpy(aname, name);
  strcat(aname, "_");
  strcat(aname, cut_name(vname, 32 - strlen(aname)));

  return aname;
}

void z2m::get_ioclassname(z2m_signal *sp)
{
  strcpy(sp->ioclassname, sp->name);
  sp->ioclassname[29] = 0;
  strcat(sp->ioclassname, "Io");
}

char *z2m::pwr_name(char *name, int keep_par)
{
  static char pname[200];
  int in_par = 0;

  char *t = pname;
  if (isdigit(name[0])) {
    *t = 'N';
    t++;
  }
    
  for (char *s = name; *s; s++) {
    if (in_par) {
      if (*s == ')')
	in_par = 0;	  
      continue;
    }
    switch (*s) {
    case ' ':
      *t = '_';
      t++;
      break;
    case '-':
      *t = '_';
      t++;
      break;
    case '/':
      *t = '_';
      t++;
      break;
    case '+':
      *t = '_';
      t++;
      break;
    case '&':
      *t = 'e';
      t++;
      break;
    case '.':
      *t = '_';
      t++;
      break;
    case ':':
      *t = '_';
      t++;
      break;
    case '*':
      *t = 'o';
      t++;
      break;
    case '%':
      *t = '$';
      t++;
      break;
    case 'ü':
      *t = 'u';
      t++;
      break;
    case 'é':
      *t = 'e';
      t++;
      break;
    case -61:
      *t = 'u';
      t++;
      s++;
      break;
    case '\'':
      s++;
      break;
    case '(':
      if (!keep_par)
	in_par = 1;
      break;
    case ')':
      in_par = 0;
      break;
    default:
      *t = *s;
      t++;
    }
  }
  *t = 0;
  return pname;
}

void z2m::get_vendorname(z2m_device *dp, char *str)
{
  static char vname[200];

  strcpy(vname, str);
  if (strcmp(vname, "Universal Electronics Inc") == 0)
    vname[9] = 0;
  else if (strcmp(vname, "Schneider Electric") == 0)
    vname[9] = 0;
  else if (strcmp(vname, "Heimgard Technologies") == 0)
    vname[8] = 0;
  else if (strcmp(vname, "Dresden Elektronik") == 0)
    vname[7] = 0;
  else if (strcmp(vname, "Custom devices (DiY)") == 0)
    vname[6] = 0;
  else if (strcmp(vname, "Current Products Corp") == 0)
    vname[7] = 0;
  else if (strcmp(vname, "Aurora Lighting") == 0)
    vname[6] = 0;
  else if (strcmp(vname, "Atlantic Group") == 0)
    vname[8] = 0;
  else if (strcmp(vname, "Shenzhen Golden Security Technology") == 0)
    vname[8] = 0;
  else if (strcmp(vname, "Acuity Brands Lighting (ABL)") == 0)
    strcpy(vname, "ABL");
  strncpy(dp->vendor, vname, sizeof(dp->vendor));
}

int z2m::get_classname(z2m_device *dp)
{
  char cname[200];
  char name[200];

  strcpy(cname, dp->vendor);
  if (strcmp(cname, "Universal Electronics Inc") == 0)
    cname[9] = 0;
  else if (strcmp(cname, "Schneider Electric") == 0)
    cname[9] = 0;
  else if (strcmp(cname, "Heimgard Technologies") == 0)
    cname[8] = 0;
  else if (strcmp(cname, "Dresden Elektronik") == 0)
    cname[7] = 0;
  else if (strcmp(cname, "Custom devices (DiY)") == 0)
    cname[6] = 0;
  else if (strcmp(cname, "Current Products Corp") == 0)
    cname[7] = 0;
  else if (strcmp(cname, "Aurora Lighting") == 0)
    cname[6] = 0;
  else if (strcmp(cname, "Atlantic Group") == 0)
    cname[8] = 0;
  else if (strcmp(cname, "Shenzhen Golden Security Technology") == 0)
    cname[8] = 0;
  else if (strcmp(cname, "Acuity Brands Lighting (ABL)") == 0)
    strcpy(cname, "ABL");

  strcat(cname, "_");
  strcat(cname, dp->model);
  strcpy(name, pwr_name(cname));

  if (strcmp(name, "Schneider_MEG5126_0300_MEG5172_0000") == 0)
    strcpy(name, "Schneider_MEG5126_300_5172_000");
  else if (strcmp(name, "Tuya_TS0601_3_phase_clamp_meter_relay") == 0)
    strcpy(name, "Tuya_TS0601_3_phase_clamp_relay");
  else if (strcmp(name, "Tuya_TS0601_illuminance_temperature_humidity_sensor_1") == 0)
    strcpy(name, "Tuya_TS0601_illum_temp_hum_1");
  else if (strcmp(name, "Tuya_TS0601_illuminance_temperature_humidity_sensor_2") == 0)
    strcpy(name, "Tuya_TS0601_illum_temp_hum_2");
  else if (strcmp(name, "Tuya_TS0601_illuminance_temperature_humidity_sensor_3") == 0)
    strcpy(name, "Tuya_TS0601_illum_temp_hum_3");
  else if (strcmp(name, "Tuya_TS0601_temperature_humidity_sensor_1") == 0)
    strcpy(name, "Tuya_TS0601_temp_hum_1");
  else if (strcmp(name, "Tuya_TS0601_temperature_humidity_sensor_2") == 0)
    strcpy(name, "Tuya_TS0601_temp_hum_2");
  else if (strcmp(name, "Tuya_TS0601_temperature_humidity_sensor_3") == 0)
    strcpy(name, "Tuya_TS0601_temp_hum_3");
  else if (strncmp(name, "Lonsonho_QS_Zigbee_", 19) == 0) {
    char str[80];
    strcpy(str, &name[19]);
    strcpy(&name[12], str);
  }
  else if (strcmp(name, "Tuya_TS0207_water_leak_detector") == 0)
    strcpy(name, "Tuya_TS0207_leak_detector");
  else if (strcmp(name, "Tuya_TS0207_water_leak_detector_1") == 0)
    strcpy(name, "Tuya_TS0207_leak_detector_1");
  else if (strcmp(name, "Tuya_TS0207_water_leak_detector_3") == 0)
    strcpy(name, "Tuya_TS0207_leak_detector_3");

  if (strlen(name) > 31) {
    if (errlog)
      printf("Long name \'%s\'\n", name);
    name[31] = 0;
  }

  // Check if name exist
  std::string sname(name);
  for (unsigned int i = 0; i < classnamevect.size(); i++) {
    if (classnamevect[i] == sname) {
      if (errlog)
	printf("** Duplicate name, \'%s\'\n", name);
      strcat(name, "_2");
      break;
    }
  }
  classnamevect.push_back(sname);
  strcpy(dp->classname, name);
  strcpy(dp->ioclassname, name);
  dp->ioclassname[29] = 0;
  strcat(dp->ioclassname, "Io");
  strcpy(dp->foclassname, name);
  dp->foclassname[29] = 0;
  strcat(dp->foclassname, "Fo");
  return 1;
}

void z2m::print_volume()
{
  fprintf(fpo, "!     Generated by cnv\n");
  fprintf(fpo, "!     Volume %s\n", volume);
  fprintf(fpo, "!     Version 0.1\n");
  fprintf(fpo, "Volume %s $ClassVolume %s\n", volume, volid);

  fprintf(fpoc, "//     Generated by cnv\n");
  fprintf(fpoc, "//     Volume %s\n", volume);
  fprintf(fpoc, "//     Version 0.1\n");
  fprintf(fpoc, "#include \"pwr.h\"\n");
  fprintf(fpoc, "#include \"rt_gdh.h\"\n");
  fprintf(fpoc, "#include \"%s\"\n", outfile_h);
  fprintf(fpoc, "\n");

  fprintf(fpoh, "//     Generated by cnv\n");
  fprintf(fpoh, "//     Volume %s\n", volume);
  fprintf(fpoh, "//     Version 0.1\n");
  char defname[200];
  strcpy(defname, outfile_h);
  defname[strlen(defname)-2] = '_';
  fprintf(fpoh, "#ifndef %s\n", defname);
  fprintf(fpoh, "#define %s\n", defname);
  fprintf(fpoh, "#include \"rt_plc.h\"\n");
  fprintf(fpoh, "#include \"pwr_ssaboxclasses.h\"\n");
  fprintf(fpoh, "#include \"pwr_otherioclasses.h\"\n");
  fprintf(fpoh, "#include \"pwr_%sclasses.h\"\n", cdh_Low(volume));
  fprintf(fpoh, "\n");


}

void z2m::print_volume_end()
{
  fprintf(fpo, "  EndObject\n");
  fprintf(fpo, "EndVolume\n");

  fprintf(fpoh, "#endif\n");
}

void z2m::print_device(z2m_device *dp)
{
  //get_classname(dp);
  fprintf(fpocix, "Device %s %s %d\n", pwr_name(dp->vendor), dp->classname, next_cix);
  if (doc && !dp->doc.empty())
    print_doc(dp->doc, 0);
  fprintf(fpo, "    Object %s $ClassDef %d\n", dp->classname, next_cix++);
  fprintf(fpo, "      Body SysBody\n");
  fprintf(fpo, "        Attr PopEditor = 1\n");
  fprintf(fpo, "      EndBody\n");
  fprintf(fpo, "      Object RtBody $ObjBodyDef 1\n");
  fprintf(fpo, "        Body SysBody\n");
  fprintf(fpo, "          Attr StructName = \"%s\"\n", dp->classname);
  fprintf(fpo, "        EndBody\n");
  fprintf(fpo, "        !/**\n");
  fprintf(fpo, "        !  Optional description.\n");
  fprintf(fpo, "        !*/\n");
  fprintf(fpo, "        Object Description $Attribute %d\n", dp->next_aix++);
  fprintf(fpo, "          Body SysBody\n");
  fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$String80\"\n");
  fprintf(fpo, "          EndBody\n");
  fprintf(fpo, "        EndObject\n");
}

void z2m::print_features(z2m_device *dp, z2m_signal *sp, int *next_aix)
{
  get_featuresname(sp);
  get_ioclassname(sp);

  fprintf(fpocix, "SubModule %s %s %d\n", pwr_name(dp->vendor), sp->submodule, next_cix);

  fprintf(fpo, "    Object %s $ClassDef %d\n", sp->submodule, next_cix++);
  fprintf(fpo, "      Body SysBody\n");
  fprintf(fpo, "        Attr PopEditor = 1\n");
  fprintf(fpo, "        Attr Flags |= pwr_mClassDef_Internal\n");
  fprintf(fpo, "      EndBody\n");
  fprintf(fpo, "      Object RtBody $ObjBodyDef 1\n");
  fprintf(fpo, "        Body SysBody\n");
  fprintf(fpo, "          Attr StructName = \"%s\"\n", sp->submodule);
  fprintf(fpo, "        EndBody\n");
  fprintf(fpo, "        !/**\n");
  fprintf(fpo, "        !  Optional description.\n");
  fprintf(fpo, "        !*/\n");
  fprintf(fpo, "        Object Description $Attribute %d\n", (*next_aix)++);
  fprintf(fpo, "          Body SysBody\n");
  fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$String80\"\n");
  fprintf(fpo, "          EndBody\n");
  fprintf(fpo, "        EndObject\n");
}

void z2m::graph_device(z2m_device *dp) 
{
  graph_device_start(dp);
  for (unsigned int j = 0; j < dp->signals.size(); j++)
    graph_signal(dp, &dp->signals[j], z2m_eSigFilter_Prio1);
  for (unsigned int j = 0; j < dp->signals.size(); j++)
    graph_signal(dp, &dp->signals[j], z2m_eSigFilter_Prio2);
  for (unsigned int j = 0; j < dp->signals.size(); j++)
    graph_signal(dp, &dp->signals[j], z2m_eSigFilter_Prio3);
  for (unsigned int j = 0; j < dp->signals.size(); j++) {
    if (dp->signals[j].features.size() > 0) {
      graph_features(dp, &dp->signals[j]);
      for (unsigned int k = 0; k < dp->signals[j].features.size(); k++)
	graph_signal(dp, &dp->signals[j].features[k], z2m_eSigFilter_Prio1);
      for (unsigned int k = 0; k < dp->signals[j].features.size(); k++)
	graph_signal(dp, &dp->signals[j].features[k], z2m_eSigFilter_Prio2);
      for (unsigned int k = 0; k < dp->signals[j].features.size(); k++)
	graph_signal(dp, &dp->signals[j].features[k], z2m_eSigFilter_Prio3);
      graph_features_end(dp, &dp->signals[j]);
    }
  }
  graph_device_end(dp);
}

void z2m::graph_device_start(z2m_device *dp) 
{
  char symbol[80];
  char fname[200];
  strcpy(fname, outdir_ge);
  strcat(fname, "pwr_c_");
  strcat(fname, cdh_Low(dp->classname));
  strcat(fname, ".ge_com");

  dp->fpog = fopen(fname, "w");
  if (!dp->fpog) {
    printf("** Unable to open file %s\n", fname);
    exit(0);
  }

  fprintf(dp->fpog, "%s", "\
main()\n\
  float x0;\n\
  float y0;\n\
  float x1;\n\
  float y1;\n\
  float x2;\n\
  float y2;\n\
  float x;\n\
  float y;\n\
  float w;\n\
  float h;\n\
  float width;\n\
  int dyn;\n\
  int id;\n\
\n\
  verify(0);\n\
  width = 28;\n\
  x0 = 0;\n\
  y0 = 0;\n\
  SetDraw(0);\n\
  SetGraphAttribute(\"x0\",0.0);\n\
  SetGraphAttribute(\"y0\",0.0);\n\
  SetGraphAttribute(\"x1\",width);\n\
  SetGraphAttribute(\"AnimationScantime\",0.2);\n\
  SetGraphAttribute(\"Translate\",1);\n\
  SetGraphAttribute(\"AntiAliasing\",1);\n\
  SetGraphAttribute(\"TooltipTextsize\",3);\n\
  SetGraphAttribute(\"ColorTheme\",\"$default\");\n\
  SetColorTheme();\n\
  SetBackgroundColor(310);\n");
  fprintf(dp->fpog, "  SetBackgroundColor(%d);\n", 310);
  fprintf(dp->fpog, "  SetGraphName(\"pwr_c_%s\");\n", cdh_Low(dp->classname));
  fprintf(dp->fpog, "%s", "\
\n\
# Menu\n\
# Node Menu, pwrct_menubar2\n\
  x1 = x0;\n\
  y1 = y0;\n\
  x2 = x1 + width;\n\
  y2 = y1 + 1.5;\n\
  id = CreateObject(\"pwrct_menubar2\",x1,y1,x2);\n\
  SetObjectAttribute(id,\"Name\",\"Menu\");\n\
  SetObjectFillColor(id,550);\n\
  SetObjectBorderColor(id,10000);\n\
  SetObjectTextColor(id,554);\n\
  SetObjectAttribute(id,\"Access\",65532);\n\
# Node File, pwrct_pulldownmenu2\n\
  x1 = x0 + 1.0;\n\
  id = CreateObject(\"pwrct_pulldownmenu2\",x1,y1);\n\
  SetObjectAttribute(id,\"Name\",\"File\");\n\
  SetObjectFillColor(id,550);\n\
  SetObjectBorderColor(id,10000);\n\
  SetObjectTextColor(id,554);\n\
  SetObjectAttribute(id,\"DynType1\",0);\n\
  SetObjectAttribute(id,\"Access\",65535);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemMask\",3);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemText1\",\"Print\");\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn1.Action\",65);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn1.Access\",65535);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn1.Command.Command\",\"print graph/class/inst=$object\");\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemText2\",\"Close\");\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn2.Action\",262145);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn2.Access\",65535);\n\
  SetObjectAttribute(id,\"A1\",\"File\");\n\
  SetObjectAttribute(id,\"Text\",\"File\");\n\
# Node Help, pwrct_pulldownmenu2\n\
  x1 += 5.0;\n\
  id = CreateObject(\"pwrct_pulldownmenu2\",x1,y1);\n\
  SetObjectAttribute(id,\"Name\",\"Help\");\n\
  SetObjectFillColor(id,550);\n\
  SetObjectBorderColor(id,10000);\n\
  SetObjectTextColor(id,554);\n\
  SetObjectAttribute(id,\"Action\",8388608);\n\
  SetObjectAttribute(id,\"Access\",65535);\n\
  SetObjectAttribute(id,\"MethodPulldownMenu.Object\",\"$object\");\n\
  SetObjectAttribute(id,\"MethodPulldownMenu.MenuType\",1);\n\
  SetObjectAttribute(id,\"A1\",\"Help\");\n\
  SetObjectAttribute(id,\"Text\",\"Help\");\n\
\n\
# Node Simulate, pwr_mb2simulate\n\
  x1 = width - 2.0;\n\
  y1 += 0.05;\n\
  id = CreateObject(\"pwr_mb2simulate\",x1,y1);\n\
  SetObjectAttribute(id,\"Name\",\"SimButton\");\n\
  SetObjectBorderColor(id,10000);\n\
  SetObjectTextColor(id,554);\n\
  SetObjectAttribute(id,\"Access\",65535);\n\
  SetObjectAttribute(id,\"Invisible.Attribute\",\"$cmd(check method/filter=$Object-SimulateFilter/object=$object)\");\n\
  SetObjectAttribute(id,\"Command.command\",\"call method/function=$Object-Simulate/object=$object\");\n\
  y1 -= 0.05;\n\
\n\
# Rectangle shade\n\
  x1 = x0;\n\
  y1 += 1.25;\n\
  w = width;\n\
  h = 1.0;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"Shade\");\n\
  SetObjectFill(id,1);\n\
  SetObjectFillColor(id,310);\n\
  SetObjectBackgroundColor(id,314);\n\
  SetObjectBorder(id,0);\n\
  SetObjectAttribute(id,\"gradient_contrast\",2);\n\
  SetObjectAttribute(id,\"gradient\",2);\n\
  SetObjectAttribute(id,\"bgcolor_gradient\",1);\n\
  SelectAdd(id);\n\
  PushSelected();\n\
  SelectClear();\n\
\n");
  char txt[200];
  strcpy(txt, dp->vendor);
  strcat(txt, " ");
  strcat(txt, dp->model);
  fprintf(dp->fpog, "# Text Vendor and model\n");
  fprintf(dp->fpog, "  x1 = x0 + 2.3;\n");
  fprintf(dp->fpog, "  y1 += 1.6;\n");
  fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", txt);
  fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"VendorLabel\");\n");
  fprintf(dp->fpog, "\n");

  fprintf(dp->fpog, "%s", "\
# Node Description, pwr_valuelong\n\
  x1 = x0 + 2.0;\n\
  y1 += 0.3;\n\
  x2 = x0 + width - 2.0;\n\
  y2 = y1 + 1.0;\n\
  id = CreateObject(\"pwr_valuelong\",x1,y1,x2,y2);\n\
  SetObjectAttribute(id,\"Name\",\"Description\");\n\
  SetObjectFillColor(id,310);\n\
  SetObjectBorderColor(id,310);\n\
  SetObjectTextColor(id,326);\n\
  SetObjectTextFont(id,4);\n\
  SetObjectAttribute(id,\"Access\",65532);\n\
  SetObjectAttribute(id,\"Value.Attribute\",\"$object.Description##String80\");\n\
  SetObjectAttribute(id,\"Value.Format\",\"%s\");\n\
\n\
# Rectangle Delim1\n\
  x1 = x0;\n\
  y1 += 1.2;\n\
  w = width;\n\
  h = 0.5;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"Delim1\");\n\
  SetObjectFill(id,1);\n\
  SetObjectBorder(id,0);\n");
  fprintf(dp->fpog, "  SetObjectFillColor(id,%d);\n", 318);
  fprintf(dp->fpog, "  y1 += 2.0;\n");

  switch (dp->group) {
  case z2m_eGroup_light: {
    strcpy(symbol, "pwrct_bulb");
    fprintf(dp->fpog, "# Symbol, %s\n", symbol);
    fprintf(dp->fpog, "  x1 = width / 2 - 1.0;\n");
    fprintf(dp->fpog, "  y1 -= 1.0;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"%s\",x1,y1);\n", symbol);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"Symbol\");\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",33619964);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"DynType2\",mDynType2_ColorThemeLightness);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"DigLowColor.Attribute\",\"$object.state.ActualValue##Boolean\");\n");
    fprintf(dp->fpog, "  y1 += 5.0;\n");
    break;
  }
  case z2m_eGroup_switch:
  case z2m_eGroup_contact:
  case z2m_eGroup_water_leak:
  case z2m_eGroup_smoke:
  case z2m_eGroup_occupancy:
  case z2m_eGroup_cover: {
    char attr[80];
    char dyn[80];
    float symsize = 2.5;
    int gradient = 0;
    switch (dp->group) {
    case z2m_eGroup_switch:
      strcpy(attr, "state");
      strcpy(dyn, "DigLowColor.Attribute");
      strcpy(symbol, "pwrct_indroundgreen");
      gradient = 15;
      break;
    case z2m_eGroup_contact:
      strcpy(attr, "contact");
      strcpy(dyn, "DigLowColor.Attribute");
      strcpy(symbol, "pwrct_indroundgreen");
      gradient = 15;
      break;
    case z2m_eGroup_water_leak:
      strcpy(attr, "water_leak");
      strcpy(dyn, "Animation.Attribute");
      strcpy(symbol, "pwrct_waterleak");
      gradient = 15;
      symsize = 4.0;
      break;
    case z2m_eGroup_smoke:
      strcpy(attr, "smoke");
      strcpy(dyn, "Animation.Attribute");
      strcpy(symbol, "pwrct_smoke");
      gradient = 15;
      symsize = 4.0;
      break;
    case z2m_eGroup_occupancy:
      strcpy(attr, "occupancy");
      strcpy(dyn, "DigLowColor.Attribute");
      strcpy(symbol, "pwrct_indroundgreen");
      gradient = 15;
      break;
    case z2m_eGroup_cover:
      strcpy(attr, "cover");
      strcpy(dyn, "DigLowColor.Attribute");
      strcpy(symbol, "pwrct_indroundgreen");
      gradient = 15;
      break;
    default: ;
    }
    fprintf(dp->fpog, "# Symbol, %s\n", symbol);
    fprintf(dp->fpog, "  x1 = width / 2 - 1.0;\n");
    fprintf(dp->fpog, "  y1 -= 1.0;\n");
    fprintf(dp->fpog, "  x2 = x1 + %3.1f;\n", symsize);
    fprintf(dp->fpog, "  y2 = y1 + %3.1f;\n", symsize);
    fprintf(dp->fpog, "  id = CreateObject(\"%s\",x1,y1,x2,y2);\n", symbol);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"Symbol\");\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",33619964);\n");
    if (gradient)
      fprintf(dp->fpog, "  SetObjectGradient(id,%d);\n", gradient);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"DynType2\",mDynType2_ColorThemeLightness);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"%s\",\"$object.%s.ActualValue##Boolean\");\n", dyn,attr);
    fprintf(dp->fpog, "  y1 += %3.1f;\n", 2.5 + symsize);
    break;
  }
  default: ;
  }
}
void z2m::graph_device_end(z2m_device *dp) 
{

  fprintf(dp->fpog, "### DeviceID\n");
  fprintf(dp->fpog, "# Text DeviceIDLabel\n");
  fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
  fprintf(dp->fpog, "  id = CreateText(\"DeviceID\",x1,y1,3,4,0,326);\n");
  fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"DeviceIDLabel\");\n");
  fprintf(dp->fpog, "\n");
  fprintf(dp->fpog, "# Node DeviceID, pwrct_valuemedium\n");
  fprintf(dp->fpog, "  x1 += 7.0;\n");
  fprintf(dp->fpog, "  y1 -= 0.8;\n");
  fprintf(dp->fpog, "  x2 = x1 + 9.0;\n");
  fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valuemedium\",x1,y1,x2);\n");
  fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"DeviceIDValue\");\n");
  fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
  fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"&($object.IoConnect).DeviceID##String40\");\n");
  fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%s\");\n");
  fprintf(dp->fpog, "  y1 += 2.5;\n");

  fprintf(dp->fpog, "%s", "\
# Rectangle BorderLow\n\
  x1 = x0;\n\
  w = width;\n\
  h = 2.0;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderLow\");\n\
  SetObjectFill(id,1);\n");
  fprintf(dp->fpog, "  SetObjectFillColor(id,%d);\n", 318);
  fprintf(dp->fpog, "%s", "\
  SetObjectBorder(id,0);\n\
\n\
# Rectangle BorderLeft\n\
  x2 = x0 - 2.0;\n\
  y2 = y0;\n\
  w = 2.5;\n\
  h = y1 + 2.0;\n\
  id = CreateRectangle(x2,y2,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderLeft\");\n\
  SetObjectFill(id,1);\n\
  SetObjectFillColor(id,318);\n\
  SetObjectBorder(id,0);\n\
\n\
# Rectangle BorderRight\n\
  x2 += width + 1.5;\n\
  id = CreateRectangle(x2,y2,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderRight\");\n\
  SetObjectFill(id,1);\n");
  fprintf(dp->fpog, "  SetObjectFillColor(id,%d);\n", 318);
  fprintf(dp->fpog, "%s", "\
  SetObjectBorder(id,0);\n\
\n\
  y1 += 0.5;\n\
  SetGraphAttribute(\"y1\",y1);\n\
  LayerResetActiveAll();\n\
  SetDraw(1);\n\
endmain\n");

  fclose(dp->fpog);
}

char *z2m::uname(char *name) 
{
  static char str[200];
  char *t = str;

  for (char *s = name; *s; s++) {
    if (s == name)
      *t = toupper(*s);
    else if (*s == '_') {
      s++;
      if (*s)
	*t = toupper(*s);
    }
    else
      *t = *s;
    t++;
  }
  *t = 0;
  return str;
}

void z2m::graph_signals_filter(z2m_device *dp)
{
  for (unsigned int j = 0; j < dp->signals.size(); j++) {
    if (dp->signals[j].features.size() > 0) {
      for (unsigned int k = 0; k < dp->signals[j].features.size(); k++)
	graph_signal_filter(&dp->signals[j].features[k]);
    }
    else
      graph_signal_filter(&dp->signals[j]);
  }
}

void z2m::graph_signal_filter(z2m_signal *sp)
{
  // Prio 1
  if (strcmp(sp->name, "state") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "action") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "brightness") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "color_temp") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "color_xy") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "effect") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "occupancy") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "smoke") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "climate") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "temperature") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "water_leak") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "contact") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  else if (strcmp(sp->name, "cover") == 0)
    sp->filter = z2m_eSigFilter_Prio1;
  
  // Prio 2
  else if (strcmp(sp->name, "linkquality") == 0)
    sp->filter = z2m_eSigFilter_Prio2;
  else if (strcmp(sp->name, "battery") == 0)
    sp->filter = z2m_eSigFilter_Prio2;
  else if (strcmp(sp->name, "transition") == 0)
    sp->filter = z2m_eSigFilter_Prio2;
  
  // Hide
  else if (strcmp(sp->name, "identify") == 0)
    sp->filter = z2m_eSigFilter_Hide;
  else if (strcmp(sp->name, "identify_timeout") == 0)
    sp->filter = z2m_eSigFilter_Hide;
  
  else
    sp->filter = z2m_eSigFilter_Prio3;
}

void z2m::check_signals(z2m_device *dp)
{
  // Check that signals name is unique within class
  for (unsigned int i = 0; i < dp->signals.size(); i++) {
    for (unsigned int j = i + 1; j < dp->signals.size(); j++) {
      if (strcmp(dp->signals[i].name, dp->signals[j].name) == 0) {
	if (strcmp(dp->signals[i].property, "") != 0 &&
	    strcmp(dp->signals[i].property, "undefined") != 0 &&
	    strcmp(dp->signals[j].property, "") != 0 &&
	    strcmp(dp->signals[j].property, "undefined") != 0 &&
	    strcmp(dp->signals[i].property, dp->signals[j].property) != 0) {
	  strcpy(dp->signals[i].name, dp->signals[i].property);
	  strcpy(dp->signals[j].name, dp->signals[j].property);
	  get_signalname(&dp->signals[i]);
	  get_signalname(&dp->signals[j]);
	} else {
	  if (errlog)
	    printf("Duplicate signal %s %s\n", dp->classname, dp->signals[i].name);
	  dp->signals[i].access |= dp->signals[j].access;
	  dp->signals.erase(dp->signals.begin()+j);
	  j--;
	}
      }
    }
    for (unsigned int k = 0; k < dp->signals[i].features.size(); k++) {
      for (unsigned int m = k + 1; m < dp->signals[i].features.size(); m++) {
	if (strcmp(dp->signals[i].features[k].name, dp->signals[i].features[m].name) == 0) {
	  if (strcmp(dp->signals[i].features[k].property, "") != 0 &&
	      strcmp(dp->signals[i].features[k].property, "undefined") != 0 &&
	      strcmp(dp->signals[i].features[m].property, "") != 0 &&
	      strcmp(dp->signals[i].features[m].property, "undefined") != 0 &&
	      strcmp(dp->signals[i].features[k].property, dp->signals[i].features[m].property) != 0) {
	    strcpy(dp->signals[i].features[k].name, dp->signals[i].features[k].property);
	    strcpy(dp->signals[i].features[m].name, dp->signals[i].features[m].property);
	    get_signalname(&dp->signals[i].features[k]);
	    get_signalname(&dp->signals[i].features[m]);
	  } else {
	    printf("Duplicate signal %s %s.%s\n", dp->classname, dp->signals[i].name, dp->signals[i].features[k].name);
	    dp->signals[i].features[k].access |= dp->signals[i].features[m].access;
	    dp->signals[i].features.erase(dp->signals[i].features.begin()+m);
	    m--;
	  }
	}
      }
    }
  }

  // Check property
  for (unsigned int i = 0; i < dp->signals.size(); i++) {
    if (strcmp(dp->signals[i].property, "") == 0 ||
	strcmp(dp->signals[i].property, "undefined") == 0)
      strcpy(dp->signals[i].property, dp->signals[i].name);
    for (unsigned int j = 0; j < dp->signals[i].features.size(); j++) {
      if (strcmp(dp->signals[i].features[j].property, "") == 0 ||
	  strcmp(dp->signals[i].features[j].property, "undefined") == 0)
	strcpy(dp->signals[i].features[j].property, dp->signals[i].features[j].name);
    }
  }
}

int z2m::is_integer_chan(z2m_signal *sp)
{
  if (strcmp(sp->name, "battery") == 0 ||
      strcmp(sp->name, "linkquality") == 0 ||
      strcmp(sp->name, "brightness") == 0)
    return 1;

  if (strstr(sp->name, "precision") != 0)
    return 1;
  if (strstr(sp->name, "interval") != 0)
    return 1;
    
  if (strstr(sp->name, "temperature") != 0 ||
      strstr(sp->name, "voltage") != 0 ||
      strstr(sp->name, "current") != 0 ||
      strstr(sp->name, "humidity") != 0 ||
      strstr(sp->name, "heat") != 0 ||
      strstr(sp->name, "cool") != 0 ||
      strstr(sp->name, "setpoint") != 0)
    return 0;
      
  return 1;
}

void z2m::get_device_group(z2m_device *dp)
{
  for (unsigned int j = 0; j < dp->signals.size(); j++) {
    z2m_signal *sp = &dp->signals[j];

    if (strcmp(sp->name, "state") == 0)
      dp->has_state = 1;
    else if (strcmp(sp->name, "brightness") == 0 && sp->access & z2m_mAccess_set) {
      dp->has_brightness = 1;
      dp->has_scantime = 1;
    }
    if (sp->features.size() > 0) {
      sp->chantype = z2m_eChanType_Composite;
      for (unsigned int k = 0; k < sp->features.size(); k++) {
	if (strcmp(sp->features[k].name, "state") == 0)
	  sp->fs_has_state = 1;
	else if (strcmp(sp->features[k].name, "brightness") == 0 && 
		 sp->features[k].access & z2m_mAccess_set) {
	  sp->fs_has_brightness = 1;
	  dp->has_scantime = 1;
	}
	if (sp->features[k].access & z2m_mAccess_set) {
	  if (sp->features[k].vtype == z2m_eVtype_binary)
	    sp->features[k].chantype = z2m_eChanType_Do;
	  else if (sp->features[k].vtype == z2m_eVtype_enum)
	    sp->features[k].chantype = z2m_eChanType_Eo;
	  else if (sp->features[k].vtype == z2m_eVtype_string)
	    sp->features[k].chantype = z2m_eChanType_So;
	  else if (sp->features[k].vtype == z2m_eVtype_list)
	    sp->features[k].chantype = z2m_eChanType_So;
	  else {
	    if (is_integer_chan(&sp->features[k]))
	      sp->features[k].chantype = z2m_eChanType_Io;
	    else
	      sp->features[k].chantype = z2m_eChanType_Ao;
	  }
	} else {
	  if (sp->features[k].vtype == z2m_eVtype_binary)
	    sp->features[k].chantype = z2m_eChanType_Di;
	  else if (sp->features[k].vtype == z2m_eVtype_enum)
	    sp->features[k].chantype = z2m_eChanType_Ei;
	  else if (sp->features[k].vtype == z2m_eVtype_string)
	    sp->features[k].chantype = z2m_eChanType_Si;
	  else if (sp->features[k].vtype == z2m_eVtype_list)
	    sp->features[k].chantype = z2m_eChanType_Si;
	  else {
	    if (is_integer_chan(&sp->features[k]))
	      sp->features[k].chantype = z2m_eChanType_Ii;
	    else
	      sp->features[k].chantype = z2m_eChanType_Ai;
	  }
	}
	if (sp->features[k].chantype == z2m_eChanType_Ao ||
	    sp->features[k].chantype == z2m_eChanType_Ai) {
	  if (sp->features[k].maxvalue != 0 || sp->features[k].minvalue != 0) {
	    if (sp->features[k].maxvalue - sp->features[k].minvalue < 3)
	      sp->features[k].no_of_decimals = 3;
	    else if (sp->features[k].maxvalue - sp->features[k].minvalue < 30)
	      sp->features[k].no_of_decimals = 2;
	    else if (sp->features[k].maxvalue - sp->features[k].minvalue < 300)
	      sp->features[k].no_of_decimals = 1;
	  }
	}
      }
    }
    else if (sp->access & z2m_mAccess_set) {
      if (sp->vtype == z2m_eVtype_binary)
	sp->chantype = z2m_eChanType_Do;
      else if (sp->vtype == z2m_eVtype_enum)
	sp->chantype = z2m_eChanType_Eo;
      else if (sp->vtype == z2m_eVtype_string)
	sp->chantype = z2m_eChanType_So;
      else if (sp->vtype == z2m_eVtype_list)
	sp->chantype = z2m_eChanType_So;
      else {
	if (is_integer_chan(sp))
	  sp->chantype = z2m_eChanType_Io;
	else
	  sp->chantype = z2m_eChanType_Ao;
      }
    } else {
      if (sp->vtype == z2m_eVtype_binary)
	sp->chantype = z2m_eChanType_Di;
      else if (sp->vtype == z2m_eVtype_enum)
	sp->chantype = z2m_eChanType_Ei;
      else if (sp->vtype == z2m_eVtype_string)
	sp->chantype = z2m_eChanType_Si;
      else if (sp->vtype == z2m_eVtype_list)
	sp->chantype = z2m_eChanType_Si;
      else {
	if (is_integer_chan(sp))
	  sp->chantype = z2m_eChanType_Ii;
	else
	  sp->chantype = z2m_eChanType_Ai;
      }
    }
    if (sp->chantype == z2m_eChanType_Ao ||
	sp->chantype == z2m_eChanType_Ai) {
      if (sp->maxvalue != 0 || sp->minvalue != 0) {
	if (sp->maxvalue - sp->minvalue < 3)
	  sp->no_of_decimals = 3;
	else if (sp->maxvalue - sp->minvalue < 30)
	  sp->no_of_decimals = 2;
	else if (sp->maxvalue - sp->minvalue < 300)
	  sp->no_of_decimals = 1;
      }
    }
  }  

  if (strstr(dp->exposes, "light") != 0 && dp->has_state)
    dp->group = z2m_eGroup_light;
  else if (strstr(dp->exposes, "switch") != 0 && dp->has_state)
    dp->group = z2m_eGroup_switch;
  else if (strstr(dp->exposes, "occupancy") != 0)
    dp->group = z2m_eGroup_occupancy;
  else if (strstr(dp->exposes, "water_leak") != 0)
    dp->group = z2m_eGroup_water_leak;
  else if (strstr(dp->exposes, "smoke") != 0)
    dp->group = z2m_eGroup_smoke;
  else if (strstr(dp->exposes, "climate") != 0)
    dp->group = z2m_eGroup_climate;
  else if (strstr(dp->exposes, "temperature") != 0)
    dp->group = z2m_eGroup_temperature;
  else if (strstr(dp->exposes, "contact") != 0)
    dp->group = z2m_eGroup_contact;
  else if (strstr(dp->exposes, "cover") != 0)
    dp->group = z2m_eGroup_cover;
  else if (strstr(dp->exposes, "action") != 0)
    dp->group = z2m_eGroup_action;
}

void z2m::graph_signal(z2m_device *dp, z2m_signal *sp, z2m_eSigFilter filter)
{
  if (sp->filter != filter)
    return;

  char sname[80];
  strcpy(sname, uname(sp->name));
  if (strcmp(sp->attrname, "") == 0)
    get_foattrname(sp, 0);

  switch (sp->chantype) {
  case z2m_eChanType_Ii: {
    fprintf(dp->fpog, "## %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valuemedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valuemedium\",x1,y1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##Int32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%d\");\n");
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    fprintf(dp->fpog, "\n");
    break;
  }
  case z2m_eChanType_Ai: {
    fprintf(dp->fpog, "## %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valuemedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valuemedium\",x1,y1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%6.1f\");\n");
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    fprintf(dp->fpog, "\n");
    break;
  }
  case z2m_eChanType_Eo: {
    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valueinputmedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  x2 = x1 + 9.0;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valueinputmedium\",x1,y1,x2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##Enum\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%s\");\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MinValueAttr\",\"$object.%s.PresMinLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MaxValueAttr\",\"$object.%s.PresMaxLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_Ei: {
    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valuemedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  x2 = x1 + 9.0;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valuemedium\",x1,y1,x2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##Enum\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%s\");\n");
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_So: {
    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valueinputmedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  x2 = x1 + 9.0;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valueinputmedium\",x1,y1,x2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##String80\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%s\");\n");
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_Si: {
    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valuemedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  x2 = x1 + 9.0;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valuemedium\",x1,y1,x2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##String80\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%s\");\n");
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_Io: {
    int button = 0;
    int spectrum = 0;
    int slider = 0;
    char format[40];
    char stype[40];
    strcpy(format, "%d");
    strcpy(stype, "Int32");

    if (strcmp(sp->name, "color_xy") == 0) {
      button = 1;
      spectrum = 1;
      slider = 1;
      strcpy(format, "%06x");
      strcpy(stype, "Color");
    }
    else if (strcmp(sp->name, "brightness") == 0) {
      slider = 1;
    }
    else if (strcmp(sp->name, "color_temp") == 0) {
      slider = 1;
    }

    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valueinputmedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valueinputmedium\",x1,y1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##Int32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%s\");\n", format);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MinValueAttr\",\"$object.%s.PresMinLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MaxValueAttr\",\"$object.%s.PresMaxLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "\n");
    if (slider) {
      fprintf(dp->fpog, "# Node %sSliderBack, pwrct_sliderbackground1_4\n", sname);
      fprintf(dp->fpog, "  x = x1 + 9.0;\n");
      fprintf(dp->fpog, "  y = y1 - 5.0;\n");
      fprintf(dp->fpog, "  x2 = x + 1.2;\n");
      fprintf(dp->fpog, "  y2 = y + 11.0;\n");
      fprintf(dp->fpog, "  id = CreateObject(\"pwrct_sliderbackground1_4\",x,y,x2,y2);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sSliderBack\");\n", sname);
      fprintf(dp->fpog, "  SelectAdd(id);\n");
      fprintf(dp->fpog, "\n");
      fprintf(dp->fpog, "# Node %sSlider, pwrct_slider4\n", sname);
      fprintf(dp->fpog, "  x += 0.02;\n");
      fprintf(dp->fpog, "  y = y1;\n");
      fprintf(dp->fpog, "  id = CreateObject(\"pwrct_slider4\",x,y);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sSlider\");\n", sname);
      fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Slider.Attribute\",\"$object.%s.ActualValue##%s\");\n", sp->attrname, stype);
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Slider.MinValueAttr\",\"$object.%s.PresMinLimit##Float32\");\n", sp->attrname);
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Slider.MaxValueAttr\",\"$object.%s.PresMaxLimit##Float32\");\n", sp->attrname);
      fprintf(dp->fpog, "  SelectAdd(id);\n");
      fprintf(dp->fpog, "  RotateSelected(90.0, 4);\n");
      fprintf(dp->fpog, "  SelectClear();\n");
      fprintf(dp->fpog, "\n");
    }
    if (button) {
      fprintf(dp->fpog, "# Node %sWhite, pwrct_buttonrounded\n", sname);
      fprintf(dp->fpog, "  x = x + 6.5;\n");
      fprintf(dp->fpog, "  y -= 0.1;\n");
      fprintf(dp->fpog, "  x2 = x + 2.8;\n");
      fprintf(dp->fpog, "  y2 = y + 1.2;\n");
      fprintf(dp->fpog, "  id = CreateObject(\"pwrct_buttonrounded\",x,y,x2,y2);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sWhite\");\n", sname);
      fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Action\", mActionType1_SetValue);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"SetValue.Attribute\",\"$object.%s.ActualValue##Int32\");\n", sp->attrname);
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"SetValue.Value\",\"16777215\");\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Text\",\"White\");\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"A1\",\"White\");\n");
      fprintf(dp->fpog, "\n");
    }
    if (spectrum) {
      fprintf(dp->fpog, "# Node %sSpectrum, pwrct_spectrum\n", sname);
      fprintf(dp->fpog, "  x = x1 + 4.6;\n");
      fprintf(dp->fpog, "  y = y1 + 1.2;\n");
      fprintf(dp->fpog, "  x2 = x + 10.0;\n");
      fprintf(dp->fpog, "  y2 = y + 0.4;\n");
      fprintf(dp->fpog, "  id = CreateObject(\"pwrct_spectrum\",x,y,x2,y2);\n");
      fprintf(dp->fpog, "  SelectAdd(id);\n");
      fprintf(dp->fpog, "  RotateSelected(180.0,4);\n");
      fprintf(dp->fpog, "  SelectClear();\n");
      fprintf(dp->fpog, "  y1 += 0.5;\n");
    }
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_Ao: {
    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valueinputmedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valueinputmedium\",x1,y1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%6.2f\");\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MinValueAttr\",\"$object.%s.PresMinLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MaxValueAttr\",\"$object.%s.PresMaxLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_Di: {
    char ind[80] = "pwrct_indsquaregreen";
    if (strcmp(sp->attrname, "water_leak") == 0 ||
	strcmp(sp->attrname, "smoke") == 0)
      strcpy(ind, "pwrct_indsquarered");
    fprintf(dp->fpog, "###   %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sInd, pwrct_indsquare\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  x2 = x1 + 1.0;\n");
    fprintf(dp->fpog, "  y2 = y1 + 1.0;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"%s\",x1,y1,x2,y2);\n", ind);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sInd\");\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"DigLowColor.Attribute\",\"$object.%s.ActualValue##Boolean\");\n", sp->attrname);
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    fprintf(dp->fpog, "\n");
    break;
  }
  case z2m_eChanType_Do: {
    fprintf(dp->fpog, "###   %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sInd, pwrct_indsquare\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  x2 = x1 + 1.0;\n");
    fprintf(dp->fpog, "  y2 = y1 + 1.0;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_indsquaregreen\",x1,y1,x2,y2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sInd\");\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"DigLowColor.Attribute\",\"$object.%s.ActualValue##Boolean\");\n", sp->attrname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sOnButton, pwrct_buttonrounded\n", sname);
    fprintf(dp->fpog, "  x1 += 4.5;\n");
    fprintf(dp->fpog, "  y1 -= 0.1;\n");
    fprintf(dp->fpog, "  x2 = x1 + 2.8;\n");
    fprintf(dp->fpog, "  y2 = y1 + 1.2;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_buttonrounded\",x1,y1,x2,y2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sOnButton\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Action\", mActionType1_SetDig);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"SetDig.Attribute\",\"$object.%s.ActualValue##Boolean\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Text\",\"On\");\n");
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sOffButton, pwrct_buttonrounded\n", sname);
    fprintf(dp->fpog, "  x1 += 4.0;\n");
    fprintf(dp->fpog, "  x2 = x1 + 2.8;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_buttonrounded\",x1,y1,x2,y2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sOffButton\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Action\", mActionType1_ResetDig);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ResetDig.Attribute\",\"$object.%s.ActualValue##Boolean\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Text\",\"Off\");\n");
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_: {
    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,326);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valueinputmedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valueinputmedium\",x1,y1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%4.1f\");\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MinValue\",%d.0);\n", sp->minvalue);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MaxValue\",%d.0);\n", sp->maxvalue);
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    fprintf(dp->fpog, "\n");
    break;
  }
  default: ;
  }
}

void z2m::graph_features(z2m_device *dp, z2m_signal *sp)
{
  char sname[80];
  strcpy(sname, uname(sp->name));

  fprintf(dp->fpog, "%s", "\
# Rectangle BorderLow\n\
  x1 = x0;\n\
  w = width;\n\
  h = 1.5;\n\
  y1 -= 1.0;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderLow\");\n\
  SetObjectFill(id,1);\n");
  fprintf(dp->fpog, "  SetObjectFillColor(id,%d);\n", 318);
  fprintf(dp->fpog, "%s", "\
  SetObjectBorder(id,0);\n\
  y1 += 1.0;\n");

  fprintf(dp->fpog, "## %s\n", sname);
  fprintf(dp->fpog, "# Text %sLabel\n", sname);
  fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
  fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,eDrawType_CustomColor34);\n", sname);
  fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
  fprintf(dp->fpog, "\n");
  fprintf(dp->fpog, "  y1 += 1.8;\n");
  fprintf(dp->fpog, "\n");
}

void z2m::graph_features_end(z2m_device *dp, z2m_signal *sp)
{
  fprintf(dp->fpog, "%s", "\
# Rectangle BorderLow\n\
  x1 = x0;\n\
  w = width;\n\
  h = 0.5;\n\
  y1 -= 1.0;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderLow\");\n\
  SetObjectFill(id,1);\n");
  fprintf(dp->fpog, "  SetObjectFillColor(id,%d);\n", 318);
  fprintf(dp->fpog, "%s", "\
  SetObjectBorder(id,0);\n\
  y1 += 2.0;\n\
  \n");
}

void z2m::sim_graph_device(z2m_device *dp)
{
  // Check if device contains any input channels
  int found = 0;
  for (unsigned int j = 0; j < dp->signals.size(); j++) {
    if (strcmp(dp->signals[j].name, "linkquality") == 0 ||
        strcmp(dp->signals[j].name, "battery") == 0)
      continue;
    if (dp->signals[j].features.size() > 0) {
      for (unsigned int k = 0; k < dp->signals[j].features.size(); k++) {
	switch (dp->signals[j].features[k].chantype) {
	case z2m_eChanType_Di:
	case z2m_eChanType_Ii:
	case z2m_eChanType_Ei:
	  found = 1;
	  break;
	default: ;
	}
	if (found)
	  break;
      }
    }
    else {
      switch (dp->signals[j].chantype) {
      case z2m_eChanType_Di:
      case z2m_eChanType_Ii:
      case z2m_eChanType_Ei:
	found = 1;
	break;
      default: ;
      }
    }
    if (found)
      break;
  }
  if (!found)
    return;

  // Simulate graph
  sim_graph_device_start(dp);
  for (unsigned int j = 0; j < dp->signals.size(); j++) {
    if (dp->signals[j].features.size() > 0) {
      sim_graph_features(dp, &dp->signals[j]);
      for (unsigned int k = 0; k < dp->signals[j].features.size(); k++)
	sim_graph_signal(dp, &dp->signals[j].features[k], z2m_eSigFilter_Prio1);
      for (unsigned int k = 0; k < dp->signals[j].features.size(); k++)
	sim_graph_signal(dp, &dp->signals[j].features[k], z2m_eSigFilter_Prio2);
      for (unsigned int k = 0; k < dp->signals[j].features.size(); k++)
	sim_graph_signal(dp, &dp->signals[j].features[k], z2m_eSigFilter_Prio3);
      sim_graph_features_end(dp, &dp->signals[j]);
    }
  }
  for (unsigned int j = 0; j < dp->signals.size(); j++)
    sim_graph_signal(dp, &dp->signals[j], z2m_eSigFilter_Prio1);
  for (unsigned int j = 0; j < dp->signals.size(); j++)
    sim_graph_signal(dp, &dp->signals[j], z2m_eSigFilter_Prio2);
  for (unsigned int j = 0; j < dp->signals.size(); j++)
    sim_graph_signal(dp, &dp->signals[j], z2m_eSigFilter_Prio3);
  sim_graph_device_end(dp);
}

void z2m::sim_graph_device_start(z2m_device *dp) 
{
  char fname[200];
  strcpy(fname, outdir_ge);
  strcat(fname, "pwr_c_");
  strcat(fname, cdh_Low(dp->classname));
  strcat(fname, "sim");
  strcat(fname, ".ge_com");

  dp->fpog = fopen(fname, "w");
  if (!dp->fpog) {
    printf("** Unable to open file %s\n", fname);
    exit(0);
  }

  dp->max_enum_size = 0;

  fprintf(dp->fpog, "%s", "\
main()\n\
  float x0;\n\
  float y0;\n\
  float x1;\n\
  float y1;\n\
  float x2;\n\
  float y2;\n\
  float x;\n\
  float y;\n\
  float w;\n\
  float h;\n\
  float width;\n\
  int dyn;\n\
  int id;\n\
\n\
  verify(0);\n\
  width = 28;\n\
  x0 = 0;\n\
  y0 = 0;\n\
  SetDraw(0);\n\
  SetGraphAttribute(\"x0\",0.0);\n\
  SetGraphAttribute(\"y0\",0.0);\n\
  SetGraphAttribute(\"x1\",width);\n\
  SetGraphAttribute(\"AnimationScantime\",0.2);\n\
  SetGraphAttribute(\"Translate\",1);\n\
  SetGraphAttribute(\"AntiAliasing\",1);\n\
  SetGraphAttribute(\"TooltipTextsize\",3);\n\
  SetGraphAttribute(\"ColorTheme\",\"$pwr_exe/pwr_colortheme1\");\n\
  SetColorTheme();\n\
  SetBackgroundColor(3);\n");
  fprintf(dp->fpog, "  SetGraphName(\"pwr_c_%ssim\");\n", cdh_Low(dp->classname));
  fprintf(dp->fpog, "%s", "\
\n\
# Menu\n\
# Node Menu, pwrct_menubar2\n\
  x1 = x0;\n\
  y1 = y0;\n\
  x2 = x1 + width;\n\
  y2 = y1 + 1.5;\n\
  id = CreateObject(\"pwrct_menubar2\",x1,y1,x2);\n\
  SetObjectAttribute(id,\"Name\",\"Menu\");\n\
  SetObjectFillColor(id,141);\n\
  SetObjectBorderColor(id,148);\n\
  SetObjectTextColor(id,148);\n\
  SetObjectAttribute(id,\"Access\",65532);\n\
# Node File, pwrct_pulldownmenu2\n\
  x1 = x0 + 1.0;\n\
  id = CreateObject(\"pwrct_pulldownmenu2\",x1,y1);\n\
  SetObjectAttribute(id,\"Name\",\"File\");\n\
  SetObjectFillColor(id,141);\n\
  SetObjectBorderColor(id,148);\n\
  SetObjectTextColor(id,148);\n\
  SetObjectAttribute(id,\"DynType1\",0);\n\
  SetObjectAttribute(id,\"Access\",65535);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemMask\",3);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemText1\",\"Print\");\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn1.Action\",65);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn1.Access\",65535);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn1.Command.Command\",\"print graph/class/inst=$object\");\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemText2\",\"Close\");\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn2.Action\",262145);\n\
  SetObjectAttribute(id,\"PulldownMenu.ItemDyn2.Access\",65535);\n\
  SetObjectAttribute(id,\"A1\",\"File\");\n\
  SetObjectAttribute(id,\"Text\",\"File\");\n\
# Node Help, pwrct_pulldownmenu2\n\
  x1 += 5.0;\n\
  id = CreateObject(\"pwrct_pulldownmenu2\",x1,y1);\n\
  SetObjectAttribute(id,\"Name\",\"Help\");\n\
  SetObjectFillColor(id,141);\n\
  SetObjectBorderColor(id,148);\n\
  SetObjectTextColor(id,148);\n\
  SetObjectAttribute(id,\"Action\",8388608);\n\
  SetObjectAttribute(id,\"Access\",65535);\n\
  SetObjectAttribute(id,\"MethodPulldownMenu.Object\",\"$object\");\n\
  SetObjectAttribute(id,\"MethodPulldownMenu.MenuType\",1);\n\
  SetObjectAttribute(id,\"A1\",\"Help\");\n\
  SetObjectAttribute(id,\"Text\",\"Help\");\n\
\n\
# Rectangle shade\n\
  x1 = x0;\n\
  y1 += 1.25;\n\
  w = width;\n\
  h = 1.0;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"Shade\");\n\
  SetObjectFill(id,1);\n\
  SetObjectFillColor(id,133);\n\
  SetObjectBorder(id,0);\n\
  SetObjectAttribute(id,\"gradient_contrast\",2);\n\
  SetObjectAttribute(id,\"gradient\",2);\n\
  SelectAdd(id);\n\
  PushSelected();\n\
  SelectClear();\n\
  y1 += 1.0;\n\
\n");

  fprintf(dp->fpog, "%s", "\
\n\
# Rectangle Delim1\n\
  x1 = x0;\n\
  w = width;\n\
  h = 1.2;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"Delim1\");\n\
  SetObjectFill(id,1);\n\
  SetObjectBorder(id,0);\n\
  SetObjectFillColor(id,141);\n\
  y1 += 1.2;\n\
# Text Simulate\n\
  x1 = x0 + 2.0;\n\
  y1 -= 0.4;\n");
  char txt[200];
  sprintf(txt, "Simulate %s", dp->classname);
  fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,4,4,1,148);\n", txt);
  fprintf(dp->fpog, "%s", "\
  SetObjectAttribute(id,\"Name\",\"SimText\");\n\
  y1 += 2.2;\n");

}
void z2m::sim_graph_device_end(z2m_device *dp) 
{
  fprintf(dp->fpog, "  if (y1 < %4.1f)\n", 1.1 * dp->max_enum_size);
  fprintf(dp->fpog, "    y1 = %4.1f;\n", 1.1 * dp->max_enum_size);
  fprintf(dp->fpog, "  endif\n");

  fprintf(dp->fpog, "%s", "\
# Rectangle BorderLow\n\
  x1 = x0;\n\
  w = width;\n\
  h = 2.0;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderLow\");\n\
  SetObjectFill(id,1);\n\
  SetObjectFillColor(id,141);\n\
  SetObjectBorder(id,0);\n\
\n\
# Rectangle BorderLeft\n\
  x2 = x0 - 2.0;\n\
  y2 = y0;\n\
  w = 2.5;\n\
  h = y1 + 2.0;\n\
  id = CreateRectangle(x2,y2,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderLeft\");\n\
  SetObjectFill(id,1);\n\
  SetObjectFillColor(id,141);\n\
  SetObjectBorder(id,0);\n\
\n\
# Rectangle BorderRight\n\
  x2 += width + 1.5;\n\
  id = CreateRectangle(x2,y2,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderRight\");\n\
  SetObjectFill(id,1);\n\
  SetObjectFillColor(id,141);\n\
  SetObjectBorder(id,0);\n\
\n\
  y1 += 0.5;\n\
  SetGraphAttribute(\"y1\",y1);\n\
  LayerResetActiveAll();\n\
  SetDraw(1);\n\
endmain\n");

  fclose(dp->fpog);
}

void z2m::sim_graph_signal(z2m_device *dp, z2m_signal *sp, z2m_eSigFilter filter)
{
  if (sp->filter != filter)
    return;

  char sname[80];
  strcpy(sname, uname(sp->name));
  if (strcmp(sp->name, "linkquality") == 0 ||
      strcmp(sp->name, "battery") == 0)
    return;

  switch (sp->chantype) {
  case z2m_eChanType_Ei: {
    char valuename[80][100];
    int num = dcli_parse(sp->enumvalues, ",", "", (char*)valuename,
	sizeof(valuename) / sizeof(valuename[0]), sizeof(valuename[0]), 0);
    if (dp->max_enum_size < num)
      dp->max_enum_size = num;

    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,148);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valueinputmedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  x2 = x1 + 5.0;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valueinputmedium\",x1,y1,x2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##Enum\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%s\");\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MinValueAttr\",\"$object.%s.PresMinLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MaxValueAttr\",\"$object.%s.PresMaxLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_Ii: {
    int button = 0;
    int spectrum = 0;
    int slider = 0;
    char format[40];
    char stype[40];
    strcpy(format, "%d");
    strcpy(stype, "Int32");

    if (strcmp(sp->name, "color_xy") == 0) {
      button = 1;
      spectrum = 1;
      slider = 1;
      strcpy(format, "%06x");
      strcpy(stype, "Color");
    }
    else if (strcmp(sp->name, "brightness") == 0) {
      slider = 1;
    }
    else if (strcmp(sp->name, "color_temp") == 0) {
      slider = 1;
    }

    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,148);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valueinputmedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valueinputmedium\",x1,y1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##Int32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%s\");\n", format);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MinValueAttr\",\"$object.%s.PresMinLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MaxValueAttr\",\"$object.%s.PresMaxLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "\n");
    if (slider) {
      fprintf(dp->fpog, "# Node %sSliderBack, pwrct_sliderbackground1_4\n", sname);
      fprintf(dp->fpog, "  x = x1 + 9.0;\n");
      fprintf(dp->fpog, "  y = y1 - 5.0;\n");
      fprintf(dp->fpog, "  x2 = x + 1.2;\n");
      fprintf(dp->fpog, "  y2 = y + 11.0;\n");
      fprintf(dp->fpog, "  id = CreateObject(\"pwrct_sliderbackground1_4\",x,y,x2,y2);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sSliderBack\");\n", sname);
      fprintf(dp->fpog, "  SelectAdd(id);\n");
      fprintf(dp->fpog, "\n");
      fprintf(dp->fpog, "# Node %sSlider, pwrct_slider4\n", sname);
      fprintf(dp->fpog, "  x += 0.02;\n");
      fprintf(dp->fpog, "  y = y1;\n");
      fprintf(dp->fpog, "  id = CreateObject(\"pwrct_slider4\",x,y);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sSlider\");\n", sname);
      fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Slider.Attribute\",\"$object.%s.ActualValue##%s\");\n", sp->attrname, stype);
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Slider.MinValueAttr\",\"$object.%s.PresMinLimit##Float32\");\n", sp->attrname);;
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Slider.MaxValueAttr\",\"$object.%s.PresMaxLimit##Float32\");\n", sp->attrname);;
      fprintf(dp->fpog, "  SelectAdd(id);\n");
      fprintf(dp->fpog, "  RotateSelected(90.0, 4);\n");
      fprintf(dp->fpog, "  SelectClear();\n");
      fprintf(dp->fpog, "\n");
    }
    if (button) {
      fprintf(dp->fpog, "# Node %sWhite, pwrct_buttonrounded\n", sname);
      fprintf(dp->fpog, "  x = x + 6.5;\n");
      fprintf(dp->fpog, "  y -= 0.1;\n");
      fprintf(dp->fpog, "  x2 = x + 2.8;\n");
      fprintf(dp->fpog, "  y2 = y + 1.2;\n");
      fprintf(dp->fpog, "  id = CreateObject(\"pwrct_buttonrounded\",x,y,x2,y2);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sWhite\");\n", sname);
      fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Action\", mActionType1_SetValue);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"SetValue.Attribute\",\"$object.%s.ActualValue##Int32\");\n", sp->attrname);
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"SetValue.Value\",\"16777215\");\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"Text\",\"White\");\n");
      fprintf(dp->fpog, "  SetObjectAttribute(id,\"A1\",\"White\");\n");
      fprintf(dp->fpog, "\n");
    }
    if (spectrum) {
      fprintf(dp->fpog, "# Node %sSpectrum, pwrct_spectrum\n", sname);
      fprintf(dp->fpog, "  x = x1 + 4.6;\n");
      fprintf(dp->fpog, "  y = y1 + 1.2;\n");
      fprintf(dp->fpog, "  x2 = x + 10.0;\n");
      fprintf(dp->fpog, "  y2 = y + 0.4;\n");
      fprintf(dp->fpog, "  id = CreateObject(\"pwrct_spectrum\",x,y,x2,y2);\n");
      fprintf(dp->fpog, "  SelectAdd(id);\n");
      fprintf(dp->fpog, "  RotateSelected(180.0,4);\n");
      fprintf(dp->fpog, "  SelectClear();\n");
      fprintf(dp->fpog, "  y1 += 0.5;\n");
    }
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_Ai: {

    fprintf(dp->fpog, "### %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,148);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sValue, pwrct_valueinputmedium\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_valueinputmedium\",x1,y1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sValue\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Attribute\",\"$object.%s.ActualValue##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Value.Format\",\"%%6.2f\");\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MinValueAttr\",\"$object.%s.PresMinLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ValueInput.MaxValueAttr\",\"$object.%s.PresMaxLimit##Float32\");\n", sp->attrname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  case z2m_eChanType_Di: {
    fprintf(dp->fpog, "###   %s\n", sname);
    fprintf(dp->fpog, "# Text %sLabel\n", sname);
    fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
    fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,148);\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sInd, pwrct_indsquare\n", sname);
    fprintf(dp->fpog, "  x1 += 7.0;\n");
    fprintf(dp->fpog, "  y1 -= 0.8;\n");
    fprintf(dp->fpog, "  x2 = x1 + 1.0;\n");
    fprintf(dp->fpog, "  y2 = y1 + 1.0;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_indsquaregreen\",x1,y1,x2,y2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sInd\");\n", sname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"DigLowColor.Attribute\",\"$object.%s.ActualValue##Boolean\");\n", sp->attrname);
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sOnButton, pwrct_buttonrounded\n", sname);
    fprintf(dp->fpog, "  x1 += 4.5;\n");
    fprintf(dp->fpog, "  y1 -= 0.1;\n");
    fprintf(dp->fpog, "  x2 = x1 + 2.8;\n");
    fprintf(dp->fpog, "  y2 = y1 + 1.2;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_buttonrounded\",x1,y1,x2,y2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sOnButton\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Action\", mActionType1_SetDig);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"SetDig.Attribute\",\"$object.%s.ActualValue##Boolean\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Text\",\"On\");\n");
    fprintf(dp->fpog, "\n");
    fprintf(dp->fpog, "# Node %sOffButton, pwrct_buttonrounded\n", sname);
    fprintf(dp->fpog, "  x1 += 4.0;\n");
    fprintf(dp->fpog, "  x2 = x1 + 2.8;\n");
    fprintf(dp->fpog, "  id = CreateObject(\"pwrct_buttonrounded\",x1,y1,x2,y2);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sOffButton\");\n", sname);
    fprintf(dp->fpog, "  SetObjectShadow(id,1);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Action\", mActionType1_ResetDig);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Access\",65532);\n");
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"ResetDig.Attribute\",\"$object.%s.ActualValue##Boolean\");\n", sp->attrname);
    fprintf(dp->fpog, "  SetObjectAttribute(id,\"Text\",\"Off\");\n");
    fprintf(dp->fpog, "  y1 += 2.5;\n");
    break;
  }
  default: ;
  }
}

void z2m::sim_graph_features(z2m_device *dp, z2m_signal *sp)
{
  char sname[80];
  strcpy(sname, uname(sp->name));

  fprintf(dp->fpog, "%s", "\
# Rectangle BorderLow\n\
  x1 = x0;\n\
  w = width;\n\
  h = 1.5;\n\
  y1 -= 2.0;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderLow\");\n\
  SetObjectFill(id,1);\n");
  fprintf(dp->fpog, "  SetObjectFillColor(id,%d);\n", 141);
  fprintf(dp->fpog, "%s", "\
  SetObjectBorder(id,0);\n\
  y1 += 1.0;\n");

  fprintf(dp->fpog, "## %s\n", sname);
  fprintf(dp->fpog, "# Text %sLabel\n", sname);
  fprintf(dp->fpog, "  x1 = x0 + 2.0;\n");
  fprintf(dp->fpog, "  id = CreateText(\"%s\",x1,y1,3,4,0,148);\n", sname);
  fprintf(dp->fpog, "  SetObjectAttribute(id,\"Name\",\"%sLabel\");\n", sname);
  fprintf(dp->fpog, "\n");
  fprintf(dp->fpog, "  y1 += 1.8;\n");
  fprintf(dp->fpog, "\n");
}

void z2m::sim_graph_features_end(z2m_device *dp, z2m_signal *sp)
{
  fprintf(dp->fpog, "%s", "\
# Rectangle BorderLow\n\
  x1 = x0;\n\
  w = width;\n\
  h = 0.5;\n\
  y1 -= 1.0;\n\
  id = CreateRectangle(x1,y1,w,h);\n\
  SetObjectAttribute(id,\"Name\",\"BorderLow\");\n\
  SetObjectFill(id,1);\n");
  fprintf(dp->fpog, "  SetObjectFillColor(id,%d);\n", 141);
  fprintf(dp->fpog, "%s", "\
  SetObjectBorder(id,0);\n\
  y1 += 2.0;\n\
  \n");
}

void z2m::print_signal(z2m_device *dp, z2m_signal *sp, int *next_aix, int has_brightness)
{
  if (doc && !sp->doc.empty())
    print_doc(sp->doc, 1);
  fprintf(fpo, "        Object %s $Attribute %d\n", sp->signalname, (*next_aix)++);
  fprintf(fpo, "          Body SysBody\n");
  fprintf(fpo, "            Attr PgmName = \"%s\"\n", sp->signalname);
  if (strcmp(sp->name, "transition") == 0 && has_brightness) {
    dp->has_transition = 1;
    sp->chantype = z2m_eChanType_;
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Float32\"\n");
  }
  else {
    switch(sp->chantype) {
    case z2m_eChanType_Composite:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"%s:Class-%s\"\n", volume, sp->submodule);
      break;
    case z2m_eChanType_Do:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-Do\"\n");
      break;
    case z2m_eChanType_Eo:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-Eo\"\n");
      break;
    case z2m_eChanType_So:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-So\"\n");
      break;
    case z2m_eChanType_Ao:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-Ao\"\n");
      break;
    case z2m_eChanType_Io:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-Io\"\n");
      break;
    case z2m_eChanType_Di:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-Di\"\n");
      break;
    case z2m_eChanType_Ei:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-Ei\"\n");
      break;
    case z2m_eChanType_Si:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-Si\"\n");
      break;
    case z2m_eChanType_Ai:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-Ai\"\n");
      break;
    case z2m_eChanType_Ii:
      fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-Ii\"\n");
      break;
    default:
      ;
    }
  }
  fprintf(fpo, "          EndBody\n");
  fprintf(fpo, "        EndObject\n");

}

void z2m::print_device_end(z2m_device *dp)
{
  if (dp->has_brightness) {
    fprintf(fpo, "        !/**\n");
    fprintf(fpo, "        !  Size of increment or decrement at brightness up or down event.\n");
    fprintf(fpo, "        !*/\n");
    fprintf(fpo, "        Object BrightnessIncr $Attribute %d\n", dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"BrightnessIncr\"\n");
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$UInt32\"\n");
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
  }
  fprintf(fpo, "        !/**\n");
  fprintf(fpo, "        !  Reference to the connected I/O object.\n");
  fprintf(fpo, "        !  This attribute is filled in when the IoConnect method is activated.\n");
  fprintf(fpo, "        !*/\n");
  fprintf(fpo, "        Object IoConnect $Attribute %d\n", dp->next_aix++);
  fprintf(fpo, "          Body SysBody\n");
  fprintf(fpo, "            Attr PgmName = \"IoConnect\"\n");
  fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$AttrRef\"\n");
  fprintf(fpo, "          EndBody\n");
  fprintf(fpo, "        EndObject\n");
  fprintf(fpo, "      EndObject\n");
  fprintf(fpo, "      Object ConfiguratorPoson $Menu\n");
  fprintf(fpo, "        Object Pointed $Menu\n");
  fprintf(fpo, "          Object IoConnect $MenuButton\n");
  fprintf(fpo, "            Body SysBody\n");
  fprintf(fpo, "              Attr ButtonName = \"Connect IO\"\n");
  fprintf(fpo, "              Attr MethodName = \"$AttrRef-IoConnect\"\n");
  fprintf(fpo, "              Attr MethodArguments[0] = \"%s\"\n", dp->ioclassname);
  fprintf(fpo, "              Attr FilterName = \"$AttrRef-IoConnectFilter\"\n");
  fprintf(fpo, "              Attr FilterArguments[0] = \"%s\"\n", dp->ioclassname);
  fprintf(fpo, "            EndBody\n");
  fprintf(fpo, "          EndObject\n");
  fprintf(fpo, "        EndObject\n");
  fprintf(fpo, "      EndObject\n");
  fprintf(fpo, "      Object Template %s\n", dp->classname);
  fprintf(fpo, "        Body RtBody\n");
  dp->description[79] = 0;
  fprintf(fpo, "          Attr Description = \"%s\"\n", dp->description);
  if (dp->has_brightness)
    fprintf(fpo, "          Attr BrightnessIncr = 20\n");
  for (unsigned int i = 0; i < dp->signals.size(); i++) {
    if (dp->signals[i].chantype == z2m_eChanType_Composite) {
      if (dp->signals[i].fs_has_brightness)
	fprintf(fpo, "          Attr %s.BrightnessIncr = 20\n", dp->signals[i].signalname);
      for (unsigned int j = 0; j < dp->signals[i].features.size(); j++) {
	dp->signals[i].features[j].description[79] = 0;
	fprintf(fpo, "          Attr %s.%s.Description = \"%s\"\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname, 
	    dp->signals[i].features[j].description);
	fprintf(fpo, "          Attr %s.%s.SigChanCon = \"$IoConnect:%s-Class-%s.%s.%s\"\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname, volume, dp->ioclassname, dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	if ((dp->signals[i].features[j].vtype == z2m_eVtype_numeric ||
	     dp->signals[i].features[j].vtype == z2m_eVtype_enum) && 
	    (dp->signals[i].features[j].minvalue != 0 || dp->signals[i].features[j].maxvalue != 0)) {
	  fprintf(fpo, "          Attr %s.%s.PresMinLimit = %d\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname, dp->signals[i].features[j].minvalue);
	  fprintf(fpo, "          Attr %s.%s.PresMaxLimit = %d\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname, dp->signals[i].features[j].maxvalue);
	}    
	if (dp->signals[i].features[j].vtype == z2m_eVtype_enum) {
	  fprintf(fpo, "          Attr %s.%s.CastActualValue = \"Type-%s\"\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname, dp->signals[i].features[j].enumtype);
	  fprintf(fpo, "          Attr %s.%s.CastInitialValue = \"Type-%s\"\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname, dp->signals[i].features[j].enumtype);
	}
	if (dp->signals[i].features[j].chantype == z2m_eChanType_Ai ||
	    dp->signals[i].features[j].chantype == z2m_eChanType_Ao)
	  fprintf(fpo, "          Attr %s.%s.NoOfDecimals = %d\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname, dp->signals[i].features[j].no_of_decimals);
      }
    }
    else if (dp->signals[i].chantype != z2m_eChanType_) {
      dp->signals[i].description[79] = 0;
      fprintf(fpo, "          Attr %s.Description = \"%s\"\n", dp->signals[i].signalname, 
	    dp->signals[i].description);
      fprintf(fpo, "          Attr %s.SigChanCon = \"$IoConnect:%s-Class-%s.%s\"\n", dp->signals[i].signalname, volume, dp->ioclassname, dp->signals[i].signalname);
      if ((dp->signals[i].vtype == z2m_eVtype_numeric ||
	   dp->signals[i].vtype == z2m_eVtype_enum) && 
	  (dp->signals[i].minvalue != 0 || dp->signals[i].maxvalue != 0)) {
	fprintf(fpo, "          Attr %s.PresMinLimit = %d\n", dp->signals[i].signalname, dp->signals[i].minvalue);
	fprintf(fpo, "          Attr %s.PresMaxLimit = %d\n", dp->signals[i].signalname, dp->signals[i].maxvalue);
      }    
      if (dp->signals[i].vtype == z2m_eVtype_enum) {
	fprintf(fpo, "          Attr %s.CastActualValue = \"Type-%s\"\n", dp->signals[i].signalname, dp->signals[i].enumtype);
	fprintf(fpo, "          Attr %s.CastInitialValue = \"Type-%s\"\n", dp->signals[i].signalname, dp->signals[i].enumtype);
      }
      if (dp->signals[i].chantype == z2m_eChanType_Ai ||
	  dp->signals[i].chantype == z2m_eChanType_Ao)
	fprintf(fpo, "          Attr %s.NoOfDecimals = %d\n", dp->signals[i].signalname, dp->signals[i].no_of_decimals);
    }
  }
  fprintf(fpo, "        EndBody\n");
  fprintf(fpo, "      EndObject\n");
  fprintf(fpo, "    EndObject\n");

  // Io class
  dp->next_aix = 1;
  fprintf(fpocix, "DeviceIo %s %s %d\n", pwr_name(dp->vendor), dp->ioclassname, next_cix);

  fprintf(fpo, "    Object %s $ClassDef %d\n", dp->ioclassname, next_cix++);
  fprintf(fpo, "      Body SysBody\n");
  fprintf(fpo, "        Attr PopEditor = 1\n");
  fprintf(fpo, "        Attr Flags |= pwr_mClassDef_IO\n");
  fprintf(fpo, "        Attr Flags |= pwr_mClassDef_IOCard\n");
  fprintf(fpo, "      EndBody\n");
  fprintf(fpo, "      Object RtBody $ObjBodyDef 1\n");
  fprintf(fpo, "        Body SysBody\n");
  fprintf(fpo, "          Attr StructName = \"%s\"\n", dp->ioclassname);
  fprintf(fpo, "        EndBody\n");
  fprintf(fpo, "        !/**\n");
  fprintf(fpo, "        !  Base class.\n");
  fprintf(fpo, "        !*/\n");
  fprintf(fpo, "        Object Super $Attribute %d\n", dp->next_aix++);
  fprintf(fpo, "          Body SysBody\n");
  fprintf(fpo, "            Attr Flags |= PWR_MASK_CLASS\n");
  fprintf(fpo, "            Attr Flags |= PWR_MASK_SUPERCLASS\n");
  fprintf(fpo, "            Attr TypeRef = \"OtherIo:Class-Zigbee2MQTT_Device\"\n");
  fprintf(fpo, "          EndBody\n");
  fprintf(fpo, "        EndObject\n");

  for (unsigned int i = 0; i < dp->signals.size(); i++) {
    z2m_signal *sp = &dp->signals[i];
    if (sp->chantype == z2m_eChanType_)
      continue;
    fprintf(fpo, "        Object %s $Attribute %d\n", sp->signalname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", sp->signalname);
    fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");

    switch (sp->chantype) {
    case z2m_eChanType_Composite:
      fprintf(fpo, "            Attr TypeRef = \"%s:Class-%s\"\n", volume, sp->iosubmodule);
      break;
    case z2m_eChanType_Do:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanDo\"\n");
      break;
    case z2m_eChanType_Eo:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanEo\"\n");
      break;
    case z2m_eChanType_So:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanSo\"\n");
      break;
    case z2m_eChanType_Io:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanIo\"\n");
      break;
    case z2m_eChanType_Ao:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanAo\"\n");
      break;
    case z2m_eChanType_Di:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanDi\"\n");
      break;
    case z2m_eChanType_Ei:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanEi\"\n");
      break;
    case z2m_eChanType_Si:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanSi\"\n");
      break;
    case z2m_eChanType_Ii:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanIi\"\n");
      break;
    case z2m_eChanType_Ai:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanAi\"\n");
      break;
    default: 
      ;
    }
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
  }

  fprintf(fpo, "      EndObject\n");
  fprintf(fpo, "      Object Template %s\n", dp->ioclassname);
  fprintf(fpo, "        Body RtBody\n");
  dp->description[79] = 0;
  fprintf(fpo, "          Attr Super.Description = \"%s\"\n", dp->description);
  for (unsigned int i = 0; i < dp->signals.size(); i++) {
    if (dp->signals[i].chantype == z2m_eChanType_Composite) {
      for (unsigned int j = 0; j < dp->signals[i].features.size(); j++) {
	dp->signals[i].features[j].description[79] = 0;
	fprintf(fpo, "          Attr %s.%s.Description = \"%s\"\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname, 
	    dp->signals[i].features[j].description);
	if (dp->signals[i].features[j].vtype == z2m_eVtype_binary) {
	  if ((strcmp(dp->signals[i].features[j].offvalue, "true") == 0 && strcmp(dp->signals[i].features[j].onvalue, "false") == 0) ||
	      (strcmp(dp->signals[i].features[j].offvalue, "false") == 0 && strcmp(dp->signals[i].features[j].onvalue, "true") == 0))
	    fprintf(fpo, "          Attr %s.%s.Identity = \"\\\"%s\\\":true,false\"\n", dp->signals[i].signalname, 
		  dp->signals[i].features[j].signalname, dp->signals[i].features[j].property);
	  else
	    fprintf(fpo, "          Attr %s.%s.Identity = \"\\\"%s\\\":\\\"%s\\\",\\\"%s\\\"\"\n", dp->signals[i].signalname, 
		    dp->signals[i].features[j].signalname, dp->signals[i].features[j].property, dp->signals[i].features[j].onvalue, 
		    dp->signals[i].features[j].offvalue);
	}
	else if (dp->signals[i].features[j].vtype == z2m_eVtype_numeric)
	  fprintf(fpo, "          Attr %s.%s.Identity = \"\\\"%s\\\":\"\n", dp->signals[i].signalname, 
		  dp->signals[i].features[j].signalname, dp->signals[i].features[j].property);
	else if (dp->signals[i].features[j].vtype == z2m_eVtype_string)
	  fprintf(fpo, "          Attr %s.%s.Identity = \"\\\"%s\\\":\"\n", dp->signals[i].signalname, 
		  dp->signals[i].features[j].signalname, dp->signals[i].features[j].property);
	else if (dp->signals[i].features[j].vtype == z2m_eVtype_list)
	  fprintf(fpo, "          Attr %s.%s.Identity = \"\\\"%s\\\":\"\n", dp->signals[i].signalname, 
		  dp->signals[i].features[j].signalname, dp->signals[i].features[j].property);
	else if (dp->signals[i].features[j].vtype == z2m_eVtype_composite)
	  fprintf(fpo, "          Attr %s.%s.Identity = \"\\\"%s\\\":\"\n", dp->signals[i].signalname, 
		  dp->signals[i].features[j].signalname, dp->signals[i].features[j].property);
	else if (dp->signals[i].features[j].vtype == z2m_eVtype_enum)
	  fprintf(fpo, "          Attr %s.%s.Identity = \"\\\"%s\\\":\"\n", dp->signals[i].signalname, 
		  dp->signals[i].features[j].signalname, dp->signals[i].features[j].property);
	if (dp->signals[i].features[j].chantype == z2m_eChanType_Ei ||
	    dp->signals[i].features[j].chantype == z2m_eChanType_Si ||
	    dp->signals[i].features[j].chantype == z2m_eChanType_Ii ||
	    dp->signals[i].features[j].chantype == z2m_eChanType_Ai ||
	    dp->signals[i].features[j].chantype == z2m_eChanType_Di)
	  fprintf(fpo, "          Attr %s.%s.ConversionOn = 1\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	if (dp->signals[i].features[j].chantype == z2m_eChanType_Ai) {
	  fprintf(fpo, "          Attr %s.%s.RawValRangeLow = -10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.RawValRangeHigh = 10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.ChannelSigValRangeLow = -10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.ChannelSigValRangeHigh = 10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.SensorSigValRangeLow = -10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.SensorSigValRangeHigh = 10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.ActValRangeLow = -10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.ActValRangeHigh = 10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.SensorPolyType = 1\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.Representation = 12\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	}
	if (dp->signals[i].features[j].chantype == z2m_eChanType_Ao) {
	  fprintf(fpo, "          Attr %s.%s.RawValRangeLow = -10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.RawValRangeHigh = 10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.ChannelSigValRangeLow = -10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.ChannelSigValRangeHigh = 10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.SensorSigValRangeLow = -10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.SensorSigValRangeHigh = 10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.ActValRangeLow = -10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.ActValRangeHigh = 10\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.OutPolyType = 1\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	  fprintf(fpo, "          Attr %s.%s.Representation = 12\n", dp->signals[i].signalname, dp->signals[i].features[j].signalname);
	}
      }
    }
    else if (dp->signals[i].chantype != z2m_eChanType_) {
      dp->signals[i].description[79] = 0;
      fprintf(fpo, "          Attr %s.Description = \"%s\"\n", dp->signals[i].signalname, 
	    dp->signals[i].description);
      if (dp->signals[i].vtype == z2m_eVtype_binary) {
	if ((strcmp(dp->signals[i].offvalue, "true") == 0 && strcmp(dp->signals[i].onvalue, "false") == 0) ||
	    (strcmp(dp->signals[i].offvalue, "false") == 0 && strcmp(dp->signals[i].onvalue, "true") == 0))
	  fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":true,false\"\n", 
		  dp->signals[i].signalname, dp->signals[i].property);
	else
	  fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\\\"%s\\\",\\\"%s\\\"\"\n", 
	      dp->signals[i].signalname, dp->signals[i].property, dp->signals[i].onvalue, 
	      dp->signals[i].offvalue);
      }
      else if (dp->signals[i].vtype == z2m_eVtype_numeric)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		dp->signals[i].signalname, dp->signals[i].property);
      else if (dp->signals[i].vtype == z2m_eVtype_string)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		dp->signals[i].signalname, dp->signals[i].property);
      else if (dp->signals[i].vtype == z2m_eVtype_list)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		dp->signals[i].signalname, dp->signals[i].property);
      else if (dp->signals[i].vtype == z2m_eVtype_composite)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		dp->signals[i].signalname, dp->signals[i].property);
      else if (dp->signals[i].vtype == z2m_eVtype_enum)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		dp->signals[i].signalname, dp->signals[i].property);
      if (dp->signals[i].chantype == z2m_eChanType_Ei ||
	  dp->signals[i].chantype == z2m_eChanType_Si ||
	  dp->signals[i].chantype == z2m_eChanType_Ii ||
	  dp->signals[i].chantype == z2m_eChanType_Ai ||
	  dp->signals[i].chantype == z2m_eChanType_Di)
	fprintf(fpo, "          Attr %s.ConversionOn = 1\n", dp->signals[i].signalname);
      if (dp->signals[i].chantype == z2m_eChanType_Ai) {
	fprintf(fpo, "          Attr %s.RawValRangeLow = -10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.RawValRangeHigh = 10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.ChannelSigValRangeLow = -10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.ChannelSigValRangeHigh = 10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.SensorSigValRangeLow = -10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.SensorSigValRangeHigh = 10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.ActValRangeLow = -10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.ActValRangeHigh = 10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.SensorPolyType = 1\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.Representation = 12\n", dp->signals[i].signalname);
      }
      if (dp->signals[i].chantype == z2m_eChanType_Ao) {
	fprintf(fpo, "          Attr %s.RawValRangeLow = -10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.RawValRangeHigh = 10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.ChannelSigValRangeLow = -10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.ChannelSigValRangeHigh = 10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.SensorSigValRangeLow = -10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.SensorSigValRangeHigh = 10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.ActValRangeLow = -10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.ActValRangeHigh = 10\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.OutPolyType = 1\n", dp->signals[i].signalname);
	fprintf(fpo, "          Attr %s.Representation = 12\n", dp->signals[i].signalname);
      }
    }
  }
  fprintf(fpo, "        EndBody\n");
  fprintf(fpo, "      EndObject\n");
  fprintf(fpo, "    EndObject\n");

  // Fo class

  // Check if any fo class should be written
  int fo = 0;
  for (unsigned int i = 0; i < dp->signals.size(); i++) {
    if (dp->signals[i].features.size() > 0) {
      for (unsigned int j = 0; j < dp->signals[i].features.size(); j++) {
	if (strncmp(dp->signals[i].features[j].name, "state", 6) == 0 &&
	    dp->signals[i].features[j].chantype == z2m_eChanType_Do) {
	  fo = 1;
	  break;
	}
	else if (strcmp(dp->signals[i].features[j].name, "brightness") == 0 &&
		 dp->signals[i].features[j].chantype == z2m_eChanType_Io) {
	  fo = 1;
	  break;
	}
	else if (strcmp(dp->signals[i].features[j].name, "effect") == 0 &&
		 dp->signals[i].features[j].chantype == z2m_eChanType_Eo) {
	  fo = 1;
	  break;
	}
	if (strncmp(dp->signals[i].features[j].name, "state", 6) == 0 &&
	    dp->signals[i].features[j].chantype == z2m_eChanType_Di) {
	  fo = 1;
	  break;
	}
	else if (strcmp(dp->signals[i].features[j].name, "action") == 0 &&
		 dp->signals[i].features[j].chantype == z2m_eChanType_Ei) {
	  fo = 1;
	  break;
	}
      }
      if (fo)
	break;
    }
    else if (strncmp(dp->signals[i].name, "state", 6) == 0 &&
	dp->signals[i].chantype == z2m_eChanType_Do) {
      fo = 1;
      break;
    }
    else if (strcmp(dp->signals[i].name, "brightness") == 0 &&
	dp->signals[i].chantype == z2m_eChanType_Io) {
      fo = 1;
      break;
    }
    else if (strcmp(dp->signals[i].name, "effect") == 0 &&
	dp->signals[i].chantype == z2m_eChanType_Eo) {
      fo = 1;
      break;
    }
    if (strncmp(dp->signals[i].name, "state", 6) == 0 &&
	dp->signals[i].chantype == z2m_eChanType_Di) {
      fo = 1;
      break;
    }
    else if (strcmp(dp->signals[i].name, "action") == 0 &&
	dp->signals[i].chantype == z2m_eChanType_Ei) {
      fo = 1;
      break;
    }
  }
  if (fo) {

    // Code
    fprintf(fpoc, "void %s_init(pwr_sClass_%s* o)\n", dp->foclassname, dp->foclassname);
    fprintf(fpoc, "{\n");
    fprintf(fpoc, "  pwr_tStatus sts;\n");
    fprintf(fpoc, "  pwr_tDlid dlid;\n");

    fprintf(fpoc, "  sts = gdh_DLRefObjectInfoAttrref(\n");
    fprintf(fpoc, "      &o->PlcConnect, (void**)&o->PlcConnectP, &dlid);\n");
    fprintf(fpoc, "  if (EVEN(sts)) {\n");
    fprintf(fpoc, "    o->PlcConnectP = 0;\n");
    fprintf(fpoc, "    return;\n");
    fprintf(fpoc, "  }\n");
    fprintf(fpoc, "}\n");
    fprintf(fpoc, "void %s_exec(plc_sThread* tp, pwr_sClass_%s* o)\n", dp->foclassname, dp->foclassname);
    fprintf(fpoc, "{\n");
    fprintf(fpoc, "  pwr_sClass_%s* co = (pwr_sClass_%s*)o->PlcConnectP;\n", dp->classname, dp->classname);

    // Print prototype declarations
    fprintf(fpoh, "void %s_init(pwr_sClass_%s* o);\n", dp->foclassname, dp->foclassname);
    fprintf(fpoh, "void %s_exec(plc_sThread* tp, pwr_sClass_%s* o);\n", dp->foclassname, dp->foclassname);


    dp->next_aix = 1;
    fprintf(fpocix, "DeviceFo %s %s %d\n", pwr_name(dp->vendor), dp->foclassname, next_cix);

    fprintf(fpo, "    Object %s $ClassDef %d\n", dp->foclassname, next_cix++);
    fprintf(fpo, "      Body SysBody\n");
    fprintf(fpo, "        Attr PopEditor = 1\n");
    fprintf(fpo, "        Attr Flags |= pwr_mClassDef_Plc\n");
    fprintf(fpo, "      EndBody\n");
    fprintf(fpo, "      Object RtBody $ObjBodyDef 1\n");
    fprintf(fpo, "        Body SysBody\n");
    fprintf(fpo, "          Attr StructName = \"%s\"\n", dp->foclassname);
    fprintf(fpo, "        EndBody\n");

    int input_cnt = 0;
    int intern_cnt = 0;
    int output_cnt = 0;

    // Print inputs
    for (unsigned int i = 0; i < dp->signals.size(); i++) {
      z2m_signal *sp = &dp->signals[i];

      if (sp->features.size() > 0) {
	for (unsigned int j = 0; j < sp->features.size(); j++) {
	  get_foattrname(&sp->features[j], sp);
	  print_input(dp, &sp->features[j], &input_cnt);
	}
      }
      else {
	get_foattrname(sp, 0);
	print_input(dp, sp, &input_cnt);
      }
    }

    // Print interns
    if (dp->has_scantime) {
      fprintf(fpo, "        Object ScanTime $Intern %d\n", dp->next_aix++);
      fprintf(fpo, "          Body SysBody\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Float32\"\n");
      fprintf(fpo, "            Attr Flags |= PWR_MASK_POINTER\n");
      fprintf(fpo, "            Attr Flags |= PWR_MASK_PRIVATE\n");
      fprintf(fpo, "            Attr Flags |= PWR_MASK_INVISIBLE\n");
      fprintf(fpo, "          EndBody\n");
      fprintf(fpo, "        EndObject\n");
      intern_cnt++;
    }
    if (dp->has_brightness) {
      fprintf(fpo, "        Object brightness_mup $Intern %d\n", dp->next_aix++);
      fprintf(fpo, "          Body SysBody\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
      fprintf(fpo, "          EndBody\n");
      fprintf(fpo, "        EndObject\n");
      fprintf(fpo, "        Object brightness_mdown $Intern %d\n", dp->next_aix++);
      fprintf(fpo, "          Body SysBody\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
      fprintf(fpo, "          EndBody\n");
      fprintf(fpo, "        EndObject\n");
      fprintf(fpo, "        Object brightness_mtime $Intern %d\n", dp->next_aix++);
      fprintf(fpo, "          Body SysBody\n");
      fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Float32\"\n");
      fprintf(fpo, "          EndBody\n");
      fprintf(fpo, "        EndObject\n");
      intern_cnt++;
    }
    for (unsigned int i = 0; i < dp->signals.size(); i++) {
      if (dp->signals[i].fs_has_brightness) {
	for (unsigned int j = 0; j < dp->signals[i].features.size(); j++) {
	  if (strcmp(dp->signals[i].features[j].name, "brightness") == 0) {
	    z2m_signal *fsp = &dp->signals[i].features[j];
	    fprintf(fpo, "        Object %s_mup $Intern %d\n", fsp->foattrname, dp->next_aix++);
	    fprintf(fpo, "          Body SysBody\n");
	    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
	    fprintf(fpo, "          EndBody\n");
	    fprintf(fpo, "        EndObject\n");
	    fprintf(fpo, "        Object %s_mdown $Intern %d\n", fsp->foattrname, dp->next_aix++);
	    fprintf(fpo, "          Body SysBody\n");
	    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
	    fprintf(fpo, "          EndBody\n");
	    fprintf(fpo, "        EndObject\n");
	    fprintf(fpo, "        Object %s_mtime $Intern %d\n", fsp->foattrname, dp->next_aix++);
	    fprintf(fpo, "          Body SysBody\n");
	    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Float32\"\n");
	    fprintf(fpo, "          EndBody\n");
	    fprintf(fpo, "        EndObject\n");
	    intern_cnt++;
	    break;
	  }	  
	}
      }
    }

    fprintf(fpo, "        Object PlcConnect $Intern %d\n", dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$AttrRef\"\n");
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    intern_cnt++;

    fprintf(fpo, "        Object PlcConnectP $Intern %d\n", dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$AttrRef\"\n");
    fprintf(fpo, "            Attr Flags |= PWR_MASK_POINTER\n");
    fprintf(fpo, "            Attr Flags |= PWR_MASK_PRIVATE\n");
    fprintf(fpo, "            Attr Flags |= PWR_MASK_INVISIBLE\n");
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    intern_cnt++;

    // Print outputs
    for (unsigned int i = 0; i < dp->signals.size(); i++) {
      z2m_signal *sp = &dp->signals[i];

      if (sp->features.size() > 0) {
	for (unsigned int j = 0; j < sp->features.size(); j++) {
	  get_foattrname(&sp->features[j], sp);
	  print_output(dp, &sp->features[j], &output_cnt);
	}
      }
      else {
	get_foattrname(sp, 0);
	print_output(dp, sp, &output_cnt);
      }
    }
    fprintf(fpo, "      EndObject\n");

    fprintf(fpo, "      Object DevBody $ObjBodyDef 2\n");
    fprintf(fpo, "        Body SysBody\n");
    fprintf(fpo, "          Attr StructName = \"%s\"\n", dp->foclassname);
    fprintf(fpo, "        EndBody\n");
    fprintf(fpo, "        Object PlcNode $Buffer 1\n");
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr Class = pwr_eClass_PlcNode\n");
    fprintf(fpo, "            Attr Flags |= PWR_MASK_INVISIBLE\n");
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    fprintf(fpo, "      EndObject\n");

    char attrname[80];
    strcpy(attrname, dp->foclassname);
    attrname[15] = 0;
    fprintf(fpo, "      Object GraphPlcNode $GraphPlcNode\n");
    fprintf(fpo, "        Body SysBody\n");
    fprintf(fpo, "          Attr object_type = 11\n");
    fprintf(fpo, "          Attr parameters[0] = %d\n", input_cnt);
    fprintf(fpo, "          Attr parameters[1] = %d\n", intern_cnt);
    fprintf(fpo, "          Attr parameters[2] = %d\n", output_cnt);
    if (input_cnt)
      fprintf(fpo, "          Attr default_mask[0] = 1\n");
    if (output_cnt)
      fprintf(fpo, "          Attr default_mask[1] = 1\n");
    fprintf(fpo, "          Attr segname_annotation = 1\n");
    fprintf(fpo, "          Attr compmethod = 35\n");
    fprintf(fpo, "          Attr connectmethod = 10\n");
    fprintf(fpo, "          Attr executeordermethod = 2\n");
    fprintf(fpo, "          Attr objname = \"%s\"\n", attrname);
    fprintf(fpo, "          Attr graphname = \"%s\"\n", attrname);
    fprintf(fpo, "        EndBody\n");
    fprintf(fpo, "      EndObject\n");

    fprintf(fpo, "      Object RtXtt $RtMenu\n");
    fprintf(fpo, "        Object PlcConnect $MenuRef\n");
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr ButtonName = \"PlcConnect\"\n");
    fprintf(fpo, "            Attr ButtonName = \"PlcConnect\"\n");
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    fprintf(fpo, "      EndObject\n");

    fprintf(fpo, "      Object Template %s\n", dp->foclassname);
    fprintf(fpo, "        Body RtBody\n");
    fprintf(fpo, "        EndBody\n");
    fprintf(fpo, "      EndObject\n");
    fprintf(fpo, "    EndObject\n");

    fprintf(fpoc, "}\n");

  }
}

void z2m::print_output(z2m_device *dp, z2m_signal *sp, int *output_cnt) 
{
  if (strncmp(sp->name, "state", 6) == 0 &&
      sp->chantype == z2m_eChanType_Di) {
    char attrname[80];
    char graphname[80];
    strcpy(attrname, sp->foattrname);
    attrname[31] = 0;
    strcpy(graphname, sp->name);
    trim_graphname(graphname);

    fprintf(fpo, "        Object %s $Output %d\n", attrname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
    fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    (*output_cnt)++;	
  }
  else if (strcmp(sp->name, "action") == 0 &&
	   sp->chantype == z2m_eChanType_Ei) {
    char attrname[80];
    char graphname[80];
    char valuename[80][100];
    char vname[100];

    fprintf(fpoc, "  pwr_tInt32 *%s_p = gdh_TranslateRtdbPointer((unsigned long)co->%s.ActualValue);\n", sp->foattrname, sp->attrname);
    fprintf(fpoc, "  if (*%s_p) {\n", sp->foattrname);
    fprintf(fpoc, "    switch (*%s_p) {\n", sp->foattrname);

    int num = dcli_parse(sp->enumvalues, ",", "", (char*)valuename,
	sizeof(valuename) / sizeof(valuename[0]), sizeof(valuename[0]), 0);
    for (int k = 0; k < num; k++) {

      strcpy(attrname, get_foattrvaluename(sp->foattrname, pwr_name(valuename[k])));
      strcpy(graphname, sp->name);
      strcat(graphname, "_");
      strcat(graphname, valuename[k]);
      strcpy(vname, pwr_name(cut_name(valuename[k],32)));
      vname[31] = 0;
      trim_graphname(graphname);
      fprintf(fpo, "        Object %s $Output %d\n", attrname, dp->next_aix++);
      fprintf(fpo, "          Body SysBody\n");
      fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
      fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
      fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
      fprintf(fpo, "          EndBody\n");
      fprintf(fpo, "        EndObject\n");
      (*output_cnt)++;

      fprintf(fpoc, "    case pwr_e%s_%s:\n", sp->enumtype, vname);
      fprintf(fpoc, "      o->%s = 1;\n", attrname);
      fprintf(fpoc, "      break;\n");
    }
    fprintf(fpoc, "    default: ;\n");
    fprintf(fpoc, "    }\n");
    fprintf(fpoc, "    *%s_p = 0;\n", sp->foattrname);
    fprintf(fpoc, "  } else {\n");

    for (int k = 0; k < num; k++) {
      strcpy(attrname, get_foattrvaluename(sp->foattrname, pwr_name(valuename[k])));
      fprintf(fpoc, "    o->%s = 0;\n", attrname);
    }
    fprintf(fpoc, "  }\n");
  }
}

void z2m::print_input(z2m_device *dp, z2m_signal *sp, int *input_cnt) 
{
  if (strncmp(sp->name, "state", 6) == 0 &&
      sp->chantype == z2m_eChanType_Do) {
    char attrname[80];
    char graphname[80];
    strcpy(attrname, sp->foattrname);
    strcat(attrname, "_toggle");
    attrname[31] = 0;
    strcpy(graphname, sp->name);
    strcat(graphname, "_toggle");
    trim_graphname(graphname);

    fprintf(fpo, "        Object %s $Input %d\n", attrname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
    fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    (*input_cnt)++;

    strcpy(attrname, sp->foattrname);
    strcat(attrname, "_on");
    attrname[31] = 0;
    strcpy(graphname, sp->name);
    strcat(graphname, "_on");
    trim_graphname(graphname);
    fprintf(fpo, "        Object %s $Input %d\n", attrname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
    fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    (*input_cnt)++;

    strcpy(attrname, sp->foattrname);
    strcat(attrname, "_off");
    attrname[31] = 0;
    strcpy(graphname, sp->name);
    strcat(graphname, "_off");
    trim_graphname(graphname);
    fprintf(fpo, "        Object %s $Input %d\n", attrname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
    fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    (*input_cnt)++;

    // c code
    fprintf(fpoc, "  if (*o->%s_toggleP) {\n", sp->foattrname);
    fprintf(fpoc, "    pwr_tBoolean* p = gdh_TranslateRtdbPointer((unsigned long)co->%s.ActualValue);\n", sp->attrname);
    fprintf(fpoc, "    *p = *p ? 0 : 1;\n");
    fprintf(fpoc, "  }\n");
    fprintf(fpoc, "  if (*o->%s_onP) {\n", sp->foattrname);
    fprintf(fpoc, "    pwr_tBoolean* p = gdh_TranslateRtdbPointer((unsigned long)co->%s.ActualValue);\n", sp->attrname);
    fprintf(fpoc, "    *p = 1;\n");
    fprintf(fpoc, "  }\n");
    fprintf(fpoc, "  if (*o->%s_offP) {\n", sp->foattrname);
    fprintf(fpoc, "    pwr_tBoolean* p = gdh_TranslateRtdbPointer((unsigned long)co->%s.ActualValue);\n", sp->attrname);
    fprintf(fpoc, "    *p = 0;\n");
    fprintf(fpoc, "  }\n");
    
  }
  else if (strcmp(sp->name, "brightness") == 0 &&
	   sp->chantype == z2m_eChanType_Io) {

    char attrname[80];
    char graphname[80];
    strcpy(attrname, sp->foattrname);
    strcat(attrname, "_up");
    attrname[31] = 0;
    strcpy(graphname, sp->name);
    strcat(graphname, "_up");
    trim_graphname(graphname);
    fprintf(fpo, "        Object %s $Input %d\n", attrname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
    fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    (*input_cnt)++;

    strcpy(attrname, sp->foattrname);
    strcat(attrname, "_down");
    attrname[31] = 0;
    strcpy(graphname, sp->name);
    strcat(graphname, "_down");
    trim_graphname(graphname);
    fprintf(fpo, "        Object %s $Input %d\n", attrname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
    fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    (*input_cnt)++;

    strcpy(attrname, sp->foattrname);
    strcat(attrname, "_moveup");
    attrname[31] = 0;
    strcpy(graphname, sp->name);
    strcat(graphname, "_moveup");
    trim_graphname(graphname);
    fprintf(fpo, "        Object %s $Input %d\n", attrname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
    fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    (*input_cnt)++;

    strcpy(attrname, sp->foattrname);
    strcat(attrname, "_movedown");
    attrname[31] = 0;
    strcpy(graphname, sp->name);
    strcat(graphname, "_movedown");
    trim_graphname(graphname);
    fprintf(fpo, "        Object %s $Input %d\n", attrname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
    fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    (*input_cnt)++;

    strcpy(attrname, sp->foattrname);
    strcat(attrname, "_movestop");
    attrname[31] = 0;
    strcpy(graphname, sp->name);
    strcat(graphname, "_movestop");
    trim_graphname(graphname);
    fprintf(fpo, "        Object %s $Input %d\n", attrname, dp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
    fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
    (*input_cnt)++;

    // c code
    char fsname[200];
    char *s;
    strcpy(fsname, sp->attrname);
    if ((s = strchr(fsname, '.')))
      *(s+1) = 0;
    else
      strcpy(fsname, "");

    fprintf(fpoc, "  if (*o->%s_upP) {\n", sp->foattrname);
    fprintf(fpoc, "    pwr_tInt32* p = gdh_TranslateRtdbPointer((unsigned long)co->%s.ActualValue);\n", sp->attrname);
    fprintf(fpoc, "    *p += co->%sBrightnessIncr;\n", fsname);
    fprintf(fpoc, "    if (*p > co->%s.PresMaxLimit)\n", sp->attrname);
    fprintf(fpoc, "      *p = co->%s.PresMaxLimit;\n", sp->attrname);
    fprintf(fpoc, "  }\n");
    fprintf(fpoc, "  if (*o->%s_downP) {\n", sp->foattrname);
    fprintf(fpoc, "    pwr_tInt32* p = gdh_TranslateRtdbPointer((unsigned long)co->%s.ActualValue);\n", sp->attrname);
    fprintf(fpoc, "    *p -= co->%sBrightnessIncr;\n", fsname);
    fprintf(fpoc, "    if (*p < co->%s.PresMinLimit)\n", sp->attrname);
    fprintf(fpoc, "      *p = co->%s.PresMinLimit;\n", sp->attrname);
    fprintf(fpoc, "  }\n");
    fprintf(fpoc, "  if (*o->%s_moveupP) {\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mup = 1;\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mdown = 0;\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mtime = 1;\n", sp->foattrname);
    fprintf(fpoc, "  }\n");
    fprintf(fpoc, "  if (*o->%s_movedownP) {\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mdown = 1;\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mup = 0;\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mtime = 1;\n", sp->foattrname);
    fprintf(fpoc, "  }\n");
    fprintf(fpoc, "  if (o->%s_mup) {\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mtime += *o->ScanTime;\n", sp->foattrname);
    fprintf(fpoc, "    if (o->%s_mtime >= 0.25) {\n", sp->foattrname);
    fprintf(fpoc, "      o->%s_mtime = 0;\n", sp->foattrname);
    fprintf(fpoc, "      pwr_tInt32* p = gdh_TranslateRtdbPointer((unsigned long)co->%s.ActualValue);\n", sp->attrname);
    fprintf(fpoc, "      *p += co->%sBrightnessIncr;\n", fsname);
    fprintf(fpoc, "      if (*p >= co->%s.PresMaxLimit) {\n", sp->attrname);
    fprintf(fpoc, "        *p = co->%s.PresMaxLimit;\n", sp->attrname);
    fprintf(fpoc, "        o->%s_mup = 0;\n", sp->foattrname);
    fprintf(fpoc, "      }\n");
    fprintf(fpoc, "    }\n");
    fprintf(fpoc, "  }\n");
    fprintf(fpoc, "  if (o->%s_mdown) {\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mtime += *o->ScanTime;\n", sp->foattrname);
    fprintf(fpoc, "    if (o->%s_mtime >= 0.25) {\n", sp->foattrname);
    fprintf(fpoc, "      o->%s_mtime = 0;\n", sp->foattrname);
    fprintf(fpoc, "      pwr_tInt32* p = gdh_TranslateRtdbPointer((unsigned long)co->%s.ActualValue);\n", sp->attrname);
    fprintf(fpoc, "      *p -= co->%sBrightnessIncr;\n", fsname);
    fprintf(fpoc, "      if (*p <= co->%s.PresMinLimit) {\n", sp->attrname);
    fprintf(fpoc, "        *p = co->%s.PresMinLimit;\n", sp->attrname);
    fprintf(fpoc, "        o->%s_mdown = 0;\n", sp->foattrname);
    fprintf(fpoc, "      }\n");
    fprintf(fpoc, "    }\n");
    fprintf(fpoc, "  }\n");
    fprintf(fpoc, "  if (*o->%s_movestopP) {\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mtime = 0;\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mup = 0;\n", sp->foattrname);
    fprintf(fpoc, "    o->%s_mdown = 0;\n", sp->foattrname);
    fprintf(fpoc, "  }\n");
  }
  else if (strcmp(sp->name, "effect") == 0 &&
	   sp->chantype == z2m_eChanType_Eo) {
    char attrname[80];
    char graphname[80];
    char valuename[80][100];
    char vname[80];
    
    int num = dcli_parse(sp->enumvalues, ",", "", (char*)valuename,
			 sizeof(valuename) / sizeof(valuename[0]), sizeof(valuename[0]),
			 0);
    for (int k = 0; k < num; k++) {
      strcpy(attrname, get_foattrvaluename(sp->foattrname, pwr_name(valuename[k])));
      strcpy(graphname, sp->name);
      strcat(graphname, "_");
      strcat(graphname, valuename[k]);
      trim_graphname(graphname);
      strcpy(vname, pwr_name(valuename[k]));
      vname[31] = 0;

      fprintf(fpo, "        Object %s $Input %d\n", attrname, dp->next_aix++);
      fprintf(fpo, "          Body SysBody\n");
      fprintf(fpo, "            Attr PgmName = \"%s\"\n", attrname);
      fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$Boolean\"\n");
      fprintf(fpo, "            Attr GraphName = \"%s\"\n", graphname);
      fprintf(fpo, "          EndBody\n");
      fprintf(fpo, "        EndObject\n");
      (*input_cnt)++;
      fprintf(fpoc, "  if (*o->%sP) {\n", attrname);
      fprintf(fpoc, "    pwr_tInt32* p = gdh_TranslateRtdbPointer((unsigned long)co->%s.ActualValue);\n", sp->attrname);
      fprintf(fpoc, "    *p = pwr_e%s_%s;\n", sp->enumtype, vname);
      fprintf(fpoc, "  }\n");
    }
  }
}

void z2m::print_features_end(z2m_device *dp, z2m_signal *sp, int *next_aix)
{
  if (sp->fs_has_brightness) {
    fprintf(fpo, "        !/**\n");
    fprintf(fpo, "        !  Size of increment or decrement at brightness up or down event.\n");
    fprintf(fpo, "        !*/\n");
    fprintf(fpo, "        Object BrightnessIncr $Attribute %d\n", (*next_aix)++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"BrightnessIncr\"\n");
    fprintf(fpo, "            Attr TypeRef = \"pwrs:Type-$UInt32\"\n");
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
  }
  fprintf(fpo, "      EndObject\n");
  fprintf(fpo, "      Object Template %s\n", sp->submodule);
  fprintf(fpo, "        Body RtBody\n");
  sp->description[79] = 0;
  fprintf(fpo, "          Attr Description = \"%s\"\n", sp->description);
  if (sp->fs_has_brightness)
    fprintf(fpo, "          Attr BrightnessIncr = 20\n");
  fprintf(fpo, "        EndBody\n");
  fprintf(fpo, "      EndObject\n");
  fprintf(fpo, "    EndObject\n");

  // Io class
  sp->next_aix = 1;
  fprintf(fpocix, "SubModuleIo %s %s %d\n", pwr_name(dp->vendor), sp->iosubmodule, next_cix);

  fprintf(fpo, "    Object %s $ClassDef %d\n", sp->iosubmodule, next_cix++);
  fprintf(fpo, "      Body SysBody\n");
  fprintf(fpo, "        Attr PopEditor = 1\n");
  fprintf(fpo, "        Attr Flags |= pwr_mClassDef_IO\n");
  fprintf(fpo, "        Attr Flags |= pwr_mClassDef_IOCard\n");
  fprintf(fpo, "        Attr Flags |= pwr_mClassDef_Internal\n");
  fprintf(fpo, "      EndBody\n");
  fprintf(fpo, "      Object RtBody $ObjBodyDef 1\n");
  fprintf(fpo, "        Body SysBody\n");
  fprintf(fpo, "          Attr StructName = \"%s\"\n", sp->iosubmodule);
  fprintf(fpo, "        EndBody\n");
  fprintf(fpo, "        !/**\n");
  fprintf(fpo, "        !  Base class.\n");
  fprintf(fpo, "        !*/\n");
  fprintf(fpo, "        Object Super $Attribute %d\n", sp->next_aix++);
  fprintf(fpo, "          Body SysBody\n");
  fprintf(fpo, "            Attr Flags |= PWR_MASK_CLASS\n");
  fprintf(fpo, "            Attr Flags |= PWR_MASK_SUPERCLASS\n");
  fprintf(fpo, "            Attr TypeRef = \"OtherIo:Class-Zigbee2MQTT_Device\"\n");
  fprintf(fpo, "          EndBody\n");
  fprintf(fpo, "        EndObject\n");

  for (unsigned int i = 0; i < sp->features.size(); i++) {
    z2m_signal *sp2 = &sp->features[i];
    if (sp2->chantype == z2m_eChanType_)
      continue;
    fprintf(fpo, "        Object %s $Attribute %d\n", sp2->signalname, sp->next_aix++);
    fprintf(fpo, "          Body SysBody\n");
    fprintf(fpo, "            Attr PgmName = \"%s\"\n", sp2->signalname);
    fprintf(fpo, "            Attr Flags = PWR_MASK_CLASS\n");
    switch (sp2->chantype) {
    case z2m_eChanType_Do:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanDo\"\n");
      break;
    case z2m_eChanType_Eo:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanEo\"\n");
      break;
    case z2m_eChanType_So:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanSo\"\n");
      break;
    case z2m_eChanType_Io:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanIo\"\n");
      break;
    case z2m_eChanType_Ao:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanAo\"\n");
      break;
    case z2m_eChanType_Di:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanDi\"\n");
      break;
    case z2m_eChanType_Ei:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanEi\"\n");
      break;
    case z2m_eChanType_Si:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanSi\"\n");
      break;
    case z2m_eChanType_Ii:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanIi\"\n");
      break;
    case z2m_eChanType_Ai:
      fprintf(fpo, "            Attr TypeRef = \"pwrb:Class-ChanAi\"\n");
      break;
    default: 
      ;
    }
    fprintf(fpo, "          EndBody\n");
    fprintf(fpo, "        EndObject\n");
  }

  fprintf(fpo, "      EndObject\n");
  fprintf(fpo, "      Object Template %s\n", sp->iosubmodule);
  fprintf(fpo, "        Body RtBody\n");
  sp->description[79] = 0;
  fprintf(fpo, "          Attr Super.Description = \"%s\"\n", sp->description);
  for (unsigned int i = 0; i < sp->features.size(); i++) {
    if (sp->features[i].chantype != z2m_eChanType_) {
      sp->features[i].description[79] = 0;
      fprintf(fpo, "          Attr %s.Description = \"%s\"\n", sp->features[i].signalname, 
	    sp->features[i].description);
      if (sp->features[i].vtype == z2m_eVtype_binary) {
	if ((strcmp(sp->features[i].offvalue, "true") == 0 && strcmp(sp->features[i].onvalue, "false") == 0) ||
	    (strcmp(sp->features[i].offvalue, "false") == 0 && strcmp(sp->features[i].onvalue, "true") == 0))
	  fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":true,false\"\n", 
		  sp->features[i].signalname, sp->features[i].property);
	else
	  fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\\\"%s\\\",\\\"%s\\\"\"\n", 
	      sp->features[i].signalname, sp->features[i].property, sp->features[i].onvalue, 
	      sp->features[i].offvalue);
      }
      else if (sp->features[i].vtype == z2m_eVtype_numeric)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		sp->features[i].signalname, sp->features[i].property);
      else if (sp->features[i].vtype == z2m_eVtype_string)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		sp->features[i].signalname, sp->features[i].property);
      else if (sp->features[i].vtype == z2m_eVtype_list)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		sp->features[i].signalname, sp->features[i].property);
      else if (sp->features[i].vtype == z2m_eVtype_composite)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		sp->features[i].signalname, sp->features[i].property);
      else if (sp->features[i].vtype == z2m_eVtype_enum)
	fprintf(fpo, "          Attr %s.Identity = \"\\\"%s\\\":\"\n", 
		sp->features[i].signalname, sp->features[i].property);
      if (sp->features[i].chantype == z2m_eChanType_Ei ||
	  sp->features[i].chantype == z2m_eChanType_Si ||
	  sp->features[i].chantype == z2m_eChanType_Ii ||
	  sp->features[i].chantype == z2m_eChanType_Ai ||
	  sp->features[i].chantype == z2m_eChanType_Di)
	fprintf(fpo, "          Attr %s.ConversionOn = 1\n", sp->features[i].signalname);
    }
  }
  fprintf(fpo, "        EndBody\n");
  fprintf(fpo, "      EndObject\n");
  fprintf(fpo, "    EndObject\n");
}

void z2m::error_msg(int sts, int line_cnt, z2m_device *dp, z2m_signal *sp)
{
  char msg[80];
  switch (sts) {
  case Z2M__ENUMTOOLONG:
    strcpy(msg, "Enumeration string too long");
    break;
  case Z2M__NOFILE:
    strcpy(msg, "File not found");
    break;
  default:
    strcpy(msg, "Undefined error");
  }

  if (!dp)
    printf("** Error %s\n", msg);
  else
    printf("** Error %s, at line %d, Device \'%s %s\', Signal \'%s\'\n", msg, line_cnt, 
	   dp->vendor, dp->model, sp->name);
}

int z2m::get_enumtypes()
{

  for (unsigned int i = 0; i < devices.size(); i++) {
    for (unsigned int j = 0; j < devices[i].signals.size(); j++) {
      z2m_signal *sp = &devices[i].signals[j];
      check_enum(&devices[i], sp);

      if (sp->features.size() > 0) {
	for (unsigned int k = 0; k < sp->features.size(); k++) {
	  z2m_signal *ssp = &sp->features[k];
	  check_enum(&devices[i], ssp);
	}
      }
    }
  }
  return 1;
}

int z2m::check_enum(z2m_device *dp, z2m_signal *sp) 
{
  char valuename[80][100];

  // Remove any duplicate elements
  int num = dcli_parse(sp->enumvalues, ",", "", (char*)valuename,
      sizeof(valuename) / sizeof(valuename[0]), sizeof(valuename[0]), 0);

  int found = 0;
  for (int j = 0; j < num; j++) {
    for (int k = j + 1; k < num; k++) {
      if (strcmp(valuename[j], valuename[k]) == 0) {
	for (int m = k + 1; m < num; m++)
	  strcpy(valuename[m-1], valuename[m]);
	num--;
	found = 1;
      }
    }
  }
  if (found) {
    for (int j = 0; j < num; j++) {
      if (j == 0)
	strcpy(sp->enumvalues, valuename[j]);
      else {
	strcat(sp->enumvalues, ",");
	strcat(sp->enumvalues, valuename[j]);
      }
    }
  }

  // check if type exist
  int exist = 0;
  for (unsigned int k = 0; k < enumtypes.size(); k++) {
    if (sp->vtype != z2m_eVtype_enum)
      return 0;
    if (strcmp(dp->vendor, enumtypes[k].vendor) != 0)
      continue;
    
    if (strncmp(sp->name, &enumtypes[k].type[strlen(dp->vendor) +1], strlen(sp->name)) != 0)
      continue;
	
    if (strcmp(sp->enumvalues, enumtypes[k].enumvalues) == 0) {
      strcpy(sp->enumtype, enumtypes[k].type);
      exist = 1;
      break;
    }
  }
  if (!exist) {
    // Insert
    z2m_enumtype e;
    char name[80];
    char tname[80];
    char numstr[10];
    int num;

    // Get unique name
    for (int k = 0; ; k++) {
      strcpy(name, dp->vendor);
      strcat(name, "_");
      strcat(name, sp->name);
      strcpy(name, pwr_name(name));
      name[31] = 0;      
      if (k != 0) {
	num = sprintf(numstr, "%d", k);
	name[31-num] = 0;
	strcat(name, numstr);
      }
      int name_exist = 0;
      for (unsigned int l = 0; l < enumtypes.size(); l++) {
	if (str_NoCaseStrcmp(name, enumtypes[l].type) == 0) {
	  name_exist = 1;
	  break;
	}
      }
      if (!name_exist)
	break;
    }

    strcpy(e.name, sp->name);
    strcpy(tname, name);
    strcpy(e.type, tname);
    strcpy(e.enumvalues, sp->enumvalues);
    strcpy(e.vendor, dp->vendor);
    enumtypes.push_back(e);

    strcpy(sp->enumtype, e.type);
  }
  sp->maxvalue = num;
  return 1;
}

void z2m::print_enumtypes()
{
  fprintf(fpo, "  Object Type $TypeHier\n");
  for (unsigned int i = 0; i < enumtypes.size(); i++) {
    fprintf(fpo, "    Object %s $TypeDef %d\n", enumtypes[i].type, next_tix++);
    fprintf(fpo, "      Body SysBody\n");
    fprintf(fpo, "        Attr TypeRef = \"pwrs:Type-$Enum\"\n");
    fprintf(fpo, "        Attr Elements = 1\n");
    fprintf(fpo, "      EndBody\n");

    char valuename[80][100];
    char nullname[80];
    char vname[80];
    int num = dcli_parse(enumtypes[i].enumvalues, ",", "", (char*)valuename,
        sizeof(valuename) / sizeof(valuename[0]), sizeof(valuename[0]),
        0);
    int eval = 0;

    if (strcmp(enumtypes[i].name, "effect") == 0)
      strcpy(nullname, "select");
    else
      strcpy(nullname, "null");
    fprintf(fpo, "      Object %s $Value\n", nullname);
    fprintf(fpo, "        Body SysBody\n");
    fprintf(fpo, "          Attr Text = \"%s\"\n", nullname);
    fprintf(fpo, "          Attr PgmName = \"%s\"\n", nullname);
    fprintf(fpo, "          Attr Value = %d\n", eval++);
    fprintf(fpo, "        EndBody\n");
    fprintf(fpo, "      EndObject\n");
    for (int j = 0; j < num; j++) {
      if (strcmp(valuename[j], "") == 0)
	strcpy(valuename[j], "undef");
      strcpy(vname, pwr_name(cut_name(valuename[j], 32), 1));
      vname[31] = 0;
      fprintf(fpo, "      Object %s $Value\n", vname);
      fprintf(fpo, "        Body SysBody\n");
      fprintf(fpo, "          Attr Text = \"%s\"\n", valuename[j]);
      fprintf(fpo, "          Attr PgmName = \"%s\"\n", vname);
      fprintf(fpo, "          Attr Value = %d\n", eval++);
      fprintf(fpo, "        EndBody\n");
      fprintf(fpo, "      EndObject\n");
    }
    fprintf(fpo, "    EndObject\n");
  }
  fprintf(fpo, "  EndObject\n");
  fprintf(fpo, "  Object Class $ClassHier\n");
}

void z2m::print_submodules()
{
  for (unsigned int i = 0; i < submodules.size(); i++) {
    z2m_device *dp = &devices[submodules[i].device_idx];
    z2m_signal *sp = &dp->signals[submodules[i].signal_idx];
    int next_aix = 1;

    print_features(dp, sp, &next_aix);
    for (unsigned int k = 0; k < sp->features.size(); k++)
      print_signal(dp, &sp->features[k], &next_aix, sp->fs_has_brightness);
    print_features_end(dp, sp, &next_aix);
  }
}

void z2m::print_doc(std::string& str, int is_signal)
{  
  std::size_t pos, pos2;
  std::size_t prev = 0;
  std::string line;
  std::string indent;

  if (is_signal)
    indent = "        ";
  else
    indent = "    ";
  
  for (;;) {
    pos = str.find('\n', prev);
    if (pos == std::string::npos) {
      line = indent + "! " + str.substr(prev);
      if (line != indent + "! ")
	fprintf(fpo, "%s", line.c_str());
      break;
    } else {
      if (pos - prev > 80) {
	pos2 = str.find(' ', prev + 80);
	if (pos2 != std::string::npos && pos2 <  pos)
	  pos = pos2;
	line = indent + "! " + str.substr(prev, pos - prev) + '\n';
	prev = pos + 1;
	fprintf(fpo, "%s", line.c_str());
      } else {
	line = indent + "! " + str.substr(prev, pos - prev + 1);
	prev = pos + 1;      
	if (line == indent + "! */\n")
	  line = indent + "!*/\n";
	else if (line == indent + "! /**\n")
	  line = indent + "!/**\n";

	fprintf(fpo, "%s", line.c_str());
      }
    }
  }    
}

int z2m::get_submodules()
{
  int next_module_idx = 1;

  for (unsigned int i = 0; i < devices.size(); i++) {
    for (unsigned int j = 0; j < devices[i].signals.size(); j++) {
      z2m_signal *sp = &devices[i].signals[j];

      if (sp->features.size() == 0)
	continue;

      // check if type exist
      int exist = 0;
      for (unsigned int k = 0; k < submodules.size(); k++) {
	if (strcmp(devices[i].signals[j].features_str, submodules[k].features) != 0)
	  continue;

	z2m_signal *mp = &devices[submodules[k].device_idx].signals[submodules[k].signal_idx];
	int match = 1;
	if (sp->features.size() != mp->features.size())
	  match = 0;
	else {
	  for (unsigned int m = 0; m < mp->features.size(); m++) {
	    if (strcmp(sp->features[m].name, mp->features[m].name) != 0 || 
		sp->features[m].vtype != mp->features[m].vtype) {
	      match = 0;
	      break;
	    }
	  }
	}
	if (match) {
	  strcpy(sp->submodule, submodules[k].name);
	  strcpy(sp->iosubmodule, submodules[k].ioname);
	  exist = 1;
	  break;
	}
      }
      if (!exist) {
	// Insert
	z2m_submodule m;
	char name[80];
	char ioname[80];
	char subname[80];

	// Get unique name
	strcpy(subname, "_sub");
	sprintf(&subname[strlen(subname)], "%d", next_module_idx++);

	strcpy(name, devices[i].vendor);
	strcat(name, "_");
	strncat(name, sp->signalname, sizeof(name) - strlen(name));
	name[31 - strlen(subname)] = 0;
	strcpy(name, pwr_name(name));
	strcat(name, subname);

	strcpy(ioname, devices[i].vendor);
	strcat(ioname, "_");
	strncat(ioname, sp->signalname, sizeof(ioname) - strlen(ioname));
	ioname[31 - strlen(subname) - 2] = 0;
	strcpy(ioname, pwr_name(ioname));
	strcat(ioname, subname);
	strcat(ioname, "Io");

	strcpy(m.name, name);
	strcpy(m.ioname, ioname);
	strcpy(m.features, sp->features_str);
	m.device_idx = i;
	m.signal_idx = j;
	submodules.push_back(m);

	strcpy(sp->submodule, name);
	strcpy(sp->iosubmodule, ioname);
      }
    }
  }
  return 1;
}


bool sort_cmp(z2m_device &a, z2m_device &b) 
{
  char aname[80], bname[80];
  
  strcpy(aname, cdh_Low(a.classname));
  strcpy(bname, cdh_Low(b.classname));
 
  int c = strcmp(aname, bname);
  if (c == 0)
    return false;
  if ( c < 0)
    return true;
  if ( c > 0)
    return false;
  return false;
}

int z2m::read_file()
{
  FILE *fp;
  int sts;
  char line[1000];
  int in_device = 0;
  int in_signal = 0;
  int in_features = 0;
  int skipped_features = 0;
  z2m_device *dp = 0;
  z2m_signal *sp = 0;
  z2m_signal *main_sp = 0;
  int line_cnt = 0;
  int num;
  pwr_tFileName fname;

  dcli_translate_filename(infile, infile);
  fp = fopen(infile, "r");
  if (!fp) {
    printf("** Unable to open file %s\n", infile);
    exit(0);
  }

  dcli_translate_filename(outfile, outfile);
  fpo = fopen(outfile, "w");
  if (!fpo) {
    printf("** Unable to open file %s\n", outfile);
    exit(0);
  }

  dcli_translate_filename(outfile_cix, outfile_cix);
  fpocix = fopen(outfile_cix, "w");
  if (!fpocix) {
    printf("** Unable to open file %s\n", outfile_cix);
    exit(0);
  }

  strcpy(fname, outdir_c);
  strcat(fname, outfile_c);
  dcli_translate_filename(fname, fname);
  fpoc = fopen(fname, "w");
  if (!fpoc) {
    printf("** Unable to open file %s\n", outfile_c);
    exit(0);
  }

  strcpy(fname, outdir_c);
  strcat(fname, outfile_h);
  dcli_translate_filename(fname, fname);
  fpoh = fopen(fname, "w");
  if (!fpoh) {
    printf("** Unable to open file %s\n", outfile_h);
    exit(0);
  }

  if (!fp || !fpo)
    return Z2M__NOFILE;

  while (read_line(line, sizeof(line), fp)) {
    line_cnt++;
    if (strncmp(line, "**Device", 8) == 0) {
      char vendor[80];
      strcpy(vendor, &line[9]);
      if (filter_cnt != 0) {
	int match = 0;
	for (int i = 0; i < filter_cnt; i++) {
	  if (dcli_wildcard(filter[i], vendor) == 0) {
	    match = 1;
	    break;
	  }
	}
	if (!match)
	  continue;
      }
      dp = new z2m_device();
      in_device = 1;
    }
    else if (!in_device)
      continue;

    if (strcmp(line, "/**") == 0) {
      if (in_signal)
	sp->doc = std::string(line) + '\n';
      else if (in_device)
        dp->doc = std::string(line) + '\n';
      //fprintf(fpo, "!%s\n", line);
      sts = read_line(line, sizeof(line), fp);
      line_cnt++;
      while (sts) {
	if (strcmp(line, "*/") == 0) {
	  if (in_signal)
	    sp->doc += std::string(line) + '\n';
	  else if (in_device)
	    dp->doc += std::string(line) + '\n';
	  break;
	}
	else if (in_signal) {
	  sp->doc += std::string(line) + '\n';
	  //fprintf(fpo, "!%s\n", line);
	}
	else if (in_device) {
	  if (strncmp(line, "Exposes ", 8) == 0) {
	    strncpy(dp->exposes, &line[8], sizeof(dp->exposes));
	    dp->exposes[sizeof(dp->exposes)-1] = 0;
	  }
	  dp->doc += std::string(line) + '\n';
	}
	sts = read_line(line, sizeof(line), fp);
	line_cnt++;
      }
    }

    else if (strncmp(line, "Vendor ", 7) == 0)
      get_vendorname(dp, &line[7]);
    else if (strncmp(line, "Model ", 6) == 0) {
      strcpy(dp->model, &line[6]);
      if (!device_check(dp)) {
	delete dp;
	in_device = 0;
	continue;
      }
    }
    else if (strncmp(line, "Name ", 5) == 0)
      strcpy(sp->name, &line[5]);
    else if (strncmp(line, "Label ", 6) == 0)
      strcpy(sp->label, &line[6]);
    else if (strncmp(line, "Type ", 5) == 0) {
      strcpy(sp->type, &line[5]);
      if (strcmp(sp->type, "binary") == 0)
	sp->vtype = z2m_eVtype_binary;
      else if (strcmp(sp->type, "numeric") == 0)
	sp->vtype = z2m_eVtype_numeric;
      else if (strcmp(sp->type, "enum") == 0)
	sp->vtype = z2m_eVtype_enum;
      else if (strcmp(sp->type, "list") == 0)
	sp->vtype = z2m_eVtype_list;
      else if (strcmp(sp->type, "composite") == 0)
	sp->vtype = z2m_eVtype_composite;
      else if (strcmp(sp->type, "string") == 0)
	sp->vtype = z2m_eVtype_string;
      else
	sp->vtype = z2m_eVtype_unknown;
    }
    else if (strncmp(line, "Property ", 9) == 0)
      strcpy(sp->property, &line[9]);
    else if (strncmp(line, "OnValue ", 8) == 0)
      strcpy(sp->onvalue, &line[8]);
    else if (strncmp(line, "OffValue ", 9) == 0)
      strcpy(sp->offvalue, &line[9]);
    else if (strncmp(line, "ToggleValue ", 12) == 0)
      strcpy(sp->togglevalue, &line[12]);
    else if (strncmp(line, "EnumValues ", 11) == 0) {
      if (strlen(line) - 11 > sizeof(sp->enumvalues) - 1) {
	sp->status = Z2M__ENUMTOOLONG;
	error_msg(sp->status, line_cnt, dp, sp);
      }
      else
	strcpy(sp->enumvalues, &line[11]);
    }
    else if (strncmp(line, "ListValues ", 11) == 0)
      strcpy(sp->listvalues, &line[11]);
    else if (strncmp(line, "Endpoint ", 9) == 0)
      strcpy(sp->endpoint, &line[9]);
    else if (strncmp(line, "Unit ", 5) == 0)
      strcpy(sp->unit, &line[5]);
    else if (strncmp(line, "Access ", 7) == 0) {
      num = sscanf(&line[7], "%d", &sp->access);
      if (num != 1) {
	if (strcmp(&line[7], "undefined") == 0)
	  sp->access = 0;
	else
	  printf("**Error access value, line %d\n", line_cnt);
      }
    }
    else if (strncmp(line, "MinValue ", 9) == 0) {
      num = sscanf(&line[9], "%d", &sp->minvalue);
      if (num != 1)
	printf("**Error minvalue value, line %d\n", line_cnt);
    }
    else if (strncmp(line, "MaxValue ", 9) == 0) {
      num = sscanf(&line[9], "%d", &sp->maxvalue);
      if (num != 1)
	printf("**Error maxvalue value, line %d\n", line_cnt);
    }
    else if (strncmp(line, "StepValue ", 9) == 0) {
      num = sscanf(&line[9], "%d", &sp->stepvalue);
      if (num != 1)
	printf("**Error stepvalue value, line %d\n", line_cnt);
    }
    else if (strncmp(line, "Description ", 12) == 0) {
      if (!in_signal) {
	strncpy(dp->description, &line[12], sizeof(dp->description)-12);
	dp->description[sizeof(sp->description)-1] = 0;
      }
      else {
	strncpy(sp->description, &line[12], sizeof(sp->description)-12);
	sp->description[sizeof(sp->description)-1] = 0;
      }
    }
    else if (strncmp(line, "Features ", 9) == 0) {
      strncpy(sp->features_str, &line[9], sizeof(sp->features_str)-9);
      sp->features_str[sizeof(sp->features_str)-1] = 0;
    }
    else if (strcmp(line, "**Option") == 0 || strcmp(line, "**Expose") == 0 ) {
      if (skipped_features && sp)
	delete sp;
      sp = new z2m_signal();
      in_signal = 1;
    }
    else if (strcmp(line, "**EndOption") == 0 || strcmp(line, "**EndExpose") == 0) {
      if (sp) {
	get_signalname(sp);
	if (in_features)
	  main_sp->features.push_back(*sp);
	else
	  dp->signals.push_back(*sp);
	in_signal = 0;
	delete sp;
	sp = 0;
      }
    }
    else if (strcmp(line, "**Features") == 0) {
      if ((strcmp(sp->name, "undefined") == 0 || strcmp(sp->name, "") == 0) && 
	  (strcmp(sp->endpoint, "undefined") == 0 || strcmp(sp->endpoint, "") == 0))
	skipped_features = 1;
      else if ((strcmp(sp->name, "undefined") == 0 || strcmp(sp->name, "") == 0) && 
	       !(strcmp(sp->endpoint, "undefined") == 0 || strcmp(sp->endpoint, "") == 0))
        strcpy(sp->name, sp->endpoint);
      if (!skipped_features) {
	in_features = 1;
	main_sp = sp;
	sp = 0;
      }
    }
    else if (strcmp(line, "**EndFeatures") == 0) {
      if (skipped_features)
	skipped_features = 0;
      else {
	in_features = 0;
	sp = main_sp;
	main_sp = 0;
      }
    }
    else if (strcmp(line, "**EndDevice") == 0) {
      devices.push_back(*dp);
      in_device = 0;
      delete dp;
      dp = 0;
      
    }
  }

  for (unsigned int i = 0; i < devices.size(); i++) {
    get_classname(&devices[i]);
    check_signals(&devices[i]);
  }

  // Sort
  std::sort(devices.begin(), devices.end(), sort_cmp);

  get_enumtypes();
  get_submodules();


  // Print file
  for (unsigned int i = 0; i < devices.size(); i++) {
    get_device_group(&devices[i]);
    graph_signals_filter(&devices[i]);
  }
  print_volume();
  print_enumtypes();
  print_submodules();

  for (unsigned int i = 0; i < devices.size(); i++) {
    // Print wbl file and c-code
    print_device(&devices[i]);
    for (unsigned int j = 0; j < devices[i].signals.size(); j++) {
      print_signal(&devices[i], &devices[i].signals[j], &devices[i].next_aix, devices[i].has_brightness);
    }
    print_device_end(&devices[i]);

    // Print ge graph
    graph_device(&devices[i]);

    // Print ge simulate graph
    sim_graph_device(&devices[i]);
  }

  print_volume_end();
  fclose(fp);
  fclose(fpo);
  fclose(fpocix);
  fclose(fpoc);
  fclose(fpoh);

  return 1;
}

static void usage()
{
  printf("\n\
  Generate wb_load file from zigbee2mqtt data file\n\
\n\
  -i name of zigbee2mqtt data file.\n\
  -d directory for generated wb_load file.\n\
  -c directory for generated code files.\n\
  -g directory for generated ge files.\n\
  -f vendor filter. Comma separated with wildcard, eg 'a*,b*,c*'.\n\
  -n volume name.\n\
  -v volume identity.\n\
\n");
}

int main(int argc, char* argv[])
{
  int sts;
  char dir[200] = "";
  char cdir[200] = "";
  char gdir[200] = "";

  z2m zig = z2m();
  strcpy(zig.infile, "$pwre_sroot/exp/z2m/src/z2m_devices.dat");
  strcpy(zig.volume, "CVolZigbee2Mqtt");
  strcpy(zig.volid, "0.0.100.4");
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      int i_incr = 0;
      for (int j = 1; argv[i][j] != 0 && argv[i][j] != ' '
           && argv[i][j] != '	';
           j++) {
        switch (argv[i][j]) {
        case 'i':
          if (i + 1 >= argc
              || !(argv[i][j + 1] == ' ' || argv[i][j + 1] != '	')) {
            usage();
            exit(0);
          }
          strncpy(zig.infile, argv[i + 1], sizeof(zig.infile));
          i++;
          i_incr = 1;
          break;
        case 'n':
          if (i + 1 >= argc
              || !(argv[i][j + 1] == ' ' || argv[i][j + 1] != '	')) {
            usage();
            exit(0);
          }
          strncpy(zig.volume, argv[i + 1], sizeof(zig.volume));
          i++;
          i_incr = 1;
          break;
        case 'd':
          if (i + 1 >= argc
              || !(argv[i][j + 1] == ' ' || argv[i][j + 1] != '	')) {
            usage();
            exit(0);
          }
          strncpy(dir, argv[i + 1], sizeof(dir));
          i++;
          i_incr = 1;
          break;
        case 'c':
          if (i + 1 >= argc
              || !(argv[i][j + 1] == ' ' || argv[i][j + 1] != '	')) {
            usage();
            exit(0);
          }
          strncpy(cdir, argv[i + 1], sizeof(cdir));
          i++;
          i_incr = 1;
          break;
        case 'g':
          if (i + 1 >= argc
              || !(argv[i][j + 1] == ' ' || argv[i][j + 1] != '	')) {
            usage();
            exit(0);
          }
          strncpy(gdir, argv[i + 1], sizeof(gdir));
          i++;
          i_incr = 1;
          break;
        case 'v':
          if (i + 1 >= argc
              || !(argv[i][j + 1] == ' ' || argv[i][j + 1] != '	')) {
            usage();
            exit(0);
          }
          strncpy(zig.volid, argv[i + 1], sizeof(zig.volid));
          i++;
          i_incr = 1;
          break;
        case 'f':
          if (i + 1 >= argc
              || !(argv[i][j + 1] == ' ' || argv[i][j + 1] != '	')) {
            usage();
            exit(0);
          }
	  zig.filter_cnt = dcli_parse(argv[i + 1], ",", "", (char*)zig.filter,
	       sizeof(zig.filter) / sizeof(zig.filter[0]), sizeof(zig.filter[0]), 0);
          i++;
          i_incr = 1;
          break;
        case 'h':
          usage();
          exit(0);
        case 'w':
          zig.doc = 0;
          break;
        default:
          usage();
          exit(0);
        }
        if (i_incr)
          break;
      }
    }
  }

  if (strcmp(dir, "") != 0) {
    if (dir[strlen(dir)-1] != '/')
      strcat(dir, "/");
    strcpy(zig.outfile, dir);
  }
  else
    strcpy(zig.outfile, "");
  strcat(zig.outfile, zig.volume);
  strcpy(zig.outfile_cix, zig.outfile);
  strcat(zig.outfile, ".wb_load");
  for (char *s = zig.outfile; *s; s++)
    *s = tolower(*s);

  strcat(zig.outfile_cix, ".txt");
  for (char *s = zig.outfile_cix; *s; s++)
    *s = tolower(*s);

  if (strcmp(cdir, "") != 0) {
    if (cdir[strlen(cdir)-1] != '/')
      strcat(cdir, "/");
    strcpy(zig.outdir_c, cdir);
  } else
    strcpy(zig.outdir_c, "");

  if (strcmp(gdir, "") != 0) {
    if (cdir[strlen(gdir)-1] != '/')
      strcat(gdir, "/");
    strcpy(zig.outdir_ge, gdir);
  }
  else
    strcpy(zig.outdir_ge, "ge/");

  strcpy(zig.outfile_c, "z2m_plc_");
  strcat(zig.outfile_c, zig.volume);
  strcat(zig.outfile_c, ".c");
  for (char *s = zig.outfile_c; *s; s++)
    *s = tolower(*s);

  strcpy(zig.outfile_h, "z2m_plc_");
  strcat(zig.outfile_h, zig.volume);
  strcat(zig.outfile_h, ".h");
  for (char *s = zig.outfile_h; *s; s++)
    *s = tolower(*s);

  sts = zig.read_file();
  if (EVEN(sts))
    zig.error_msg(sts, 0, 0, 0);

  //zig.print_stat();

}

