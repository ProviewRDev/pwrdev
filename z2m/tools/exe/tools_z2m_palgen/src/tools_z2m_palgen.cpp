#include <stdio.h>
#include <string.h>
#include <vector>
#include "pwr.h"
#include "co_dcli.h"

class palgen_device {
public:
  char type[80];
  char vendor[80];
  char classname[80];
  int cix;
  palgen_device() : cix(0) {
    strcpy(type, "");
    strcpy(vendor, "");
    strcpy(classname, "");
  }
  palgen_device(const palgen_device& x) : cix(x.cix) {
    strcpy(type, x.type);
    strcpy(vendor, x.vendor);
    strcpy(classname, x.classname);
  }
};

static std::vector<palgen_device> devices;

int main()
{
  pwr_tFileName infiles[9] = {
    "$pwre_sroot/wbl/z2m1/src/zigbee2mqtt1.txt",
    "$pwre_sroot/wbl/z2m2/src/zigbee2mqtt2.txt",
    "$pwre_sroot/wbl/z2m3/src/zigbee2mqtt3.txt",
    "$pwre_sroot/wbl/z2m4/src/zigbee2mqtt4.txt",
    "$pwre_sroot/wbl/z2m5/src/zigbee2mqtt5.txt",
    "$pwre_sroot/wbl/z2m6/src/zigbee2mqtt6.txt",
    "$pwre_sroot/wbl/z2m7/src/zigbee2mqtt7.txt",
    "$pwre_sroot/wbl/z2m8/src/zigbee2mqtt8.txt",
    "$pwre_sroot/wbl/z2m9/src/zigbee2mqtt9.txt"};
  pwr_tFileName outfile = "$pwre_sroot/exp/z2m/src/pwr_z2m_palette.cnf";
  FILE *fp;
  FILE *fpo;
  char line[400];
  char prev_vendor[80];
  int first;
  int num;
  
  dcli_translate_filename(outfile, outfile);
  fpo = fopen(outfile, "w");
  if (!fpo) {
    printf("** Unable to open file %s\n", outfile);
    exit(0);
  }

  for (unsigned int i = 0; i < sizeof(infiles)/sizeof(infiles[0]); i++) {
    dcli_translate_filename(infiles[i], infiles[i]);

    fp = fopen(infiles[i], "r");
    if (!fp) {
      printf("** Unable to open file %s\n", infiles[i]);
      exit(0);
    }


    while (dcli_read_line(line, sizeof(line), fp)) {
      palgen_device d;

      num = sscanf(line, "%s %s %s %d", d.type, d.vendor, d.classname, &d.cix);
      if (num != 4)
	continue;

      devices.push_back(d);
    }
    fclose(fp);
  }

  fprintf(fpo, "palette NavigatorPalette\n");
  fprintf(fpo, "{\n");
  fprintf(fpo, "  menu Plant\n");
  fprintf(fpo, "  {\n");
  fprintf(fpo, "    menu Components\n");
  fprintf(fpo, "    {\n");
  fprintf(fpo, "      menu Zigbee2mqtt\n");
  fprintf(fpo, "      {\n");

  strcpy(prev_vendor, "");
  first = 1;
  for (unsigned int i = 0; i < devices.size(); i++) {
    if (strcmp(devices[i].type, "Device") != 0)
      continue;

    if (strcmp(devices[i].vendor, prev_vendor) != 0) {
      strcpy(prev_vendor, devices[i].vendor);
      
      if (!first)
	fprintf(fpo, "        }\n");
      fprintf(fpo, "        Menu %s\n", devices[i].vendor);
      fprintf(fpo, "        {\n");
    }
    fprintf(fpo, "          class %s\n", devices[i].classname);
    first = 0;
  }
  fprintf(fpo, "        }\n");
  fprintf(fpo, "      }\n");
  fprintf(fpo, "    }\n");
  fprintf(fpo, "  }\n");
  fprintf(fpo, "  menu Node\n");
  fprintf(fpo, "  {\n");
  fprintf(fpo, "    menu IO\n");
  fprintf(fpo, "    {\n");
  fprintf(fpo, "      menu Zigbee2mqtt\n");
  fprintf(fpo, "      {\n");
  fprintf(fpo, "        class Zigbee2MQTT_Client\n");
  fprintf(fpo, "        class Zigbee2MQTT_Device\n");
  fprintf(fpo, "        menu Devices\n");
  fprintf(fpo, "        {\n");

  strcpy(prev_vendor, "");
  first = 1;
  for (unsigned int i = 0; i < devices.size(); i++) {
    if (strcmp(devices[i].type, "DeviceIo") != 0)
      continue;

    if (strcmp(devices[i].vendor, prev_vendor) != 0) {
      strcpy(prev_vendor, devices[i].vendor);
      
      if (!first)
	fprintf(fpo, "          }\n");
      fprintf(fpo, "          Menu %s\n", devices[i].vendor);
      fprintf(fpo, "          {\n");
    }
    fprintf(fpo, "            class %s\n", devices[i].classname);
    first = 0;
  }

  fprintf(fpo, "          }\n");
  fprintf(fpo, "        }\n");
  fprintf(fpo, "      }\n");
  fprintf(fpo, "    }\n");
  fprintf(fpo, "  }\n");

  fprintf(fpo, "  menu AllClasses\n");
  fprintf(fpo, "  {\n");
  fprintf(fpo, "    menu Zigbee2mqtt\n");
  fprintf(fpo, "    {\n");
  fprintf(fpo, "      classvolume Zigbee2mqtt1 Zigbee2mqtt1\n");
  fprintf(fpo, "      classvolume Zigbee2mqtt2 Zigbee2mqtt2\n");
  fprintf(fpo, "      classvolume Zigbee2mqtt3 Zigbee2mqtt3\n");
  fprintf(fpo, "      classvolume Zigbee2mqtt4 Zigbee2mqtt4\n");
  fprintf(fpo, "      classvolume Zigbee2mqtt5 Zigbee2mqtt5\n");
  fprintf(fpo, "      classvolume Zigbee2mqtt6 Zigbee2mqtt6\n");
  fprintf(fpo, "      classvolume Zigbee2mqtt7 Zigbee2mqtt7\n");
  fprintf(fpo, "      classvolume Zigbee2mqtt8 Zigbee2mqtt8\n");
  fprintf(fpo, "      classvolume Zigbee2mqtt9 Zigbee2mqtt9\n");
  fprintf(fpo, "    }\n");
  fprintf(fpo, "  }\n");
  fprintf(fpo, "}\n");
  
  fprintf(fpo, "palette PlcEditorPalette\n");
  fprintf(fpo, "{\n");
  fprintf(fpo, "  menu Components\n");
  fprintf(fpo, "  {\n");
  fprintf(fpo, "    menu Zigbee2mqtt\n");
  fprintf(fpo, "    {\n");

  strcpy(prev_vendor, "");
  first = 1;
  for (unsigned int i = 0; i < devices.size(); i++) {
    if (strcmp(devices[i].type, "DeviceFo") != 0)
      continue;

    if (strcmp(devices[i].vendor, prev_vendor) != 0) {
      strcpy(prev_vendor, devices[i].vendor);
      
      if (!first)
	fprintf(fpo, "      }\n");
      fprintf(fpo, "      Menu %s\n", devices[i].vendor);
      fprintf(fpo, "      {\n");
    }
    fprintf(fpo, "        class %s\n", devices[i].classname);
    first = 0;
  }
  fprintf(fpo, "      }\n");
  fprintf(fpo, "    }\n");
  fprintf(fpo, "  }\n");
  fprintf(fpo, "}\n");

  fclose(fpo);
}
