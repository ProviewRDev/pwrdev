#include <stdio.h>
#include "pwr.h"
#include "rt_bckdef.h"
#include "wb_ldh_msg.h"
#include "co_time.h"
#include "co_dcli.h"


/* 
  Convert 32-bit backupfile to 64-bit backupfile.
  
  The size of bck_t_cycleheader is 4 bytes larger on 64-bit, but offsets
  and sizes of elements are the same.
*/

pwr_tStatus bck_cnv(char* filename, char* out, int from64)
{
  pwr_tFileName fname;
  FILE *f, *fout;
  BCK_FILEHEAD_STRUCT fh;
  bck_t_cycleheader ch;
  bck_t_writeheader dh;
  char timstr[24];
  int c, d;
  unsigned char *datap = NULL, *p;
  int i;
  int csts;
  char* namep = NULL;
  long pos, pos2;

  // Open file

  dcli_translate_filename(fname, filename);
  f = fopen(fname, "rb");
  if (!f)
    return LDH__NOSUCHFILE;

  dcli_translate_filename(fname, out);
  fout = fopen(fname, "w");
  if (!fout) {
    fclose(f);
    return LDH__NOSUCHFILE;
  }

  // Read header and print it

  fseek(f, 0, 0);
  fread(&fh, sizeof fh, 1, f);
  fwrite(&fh, sizeof fh, 1, fout);
  //fprintf(fout, "Layout version:       %d\n", fh.version);
  if (fh.version != BCK_FILE_VERSION) {
    printf("This program is built with header version %d\n", BCK_FILE_VERSION);
    fclose(f);
    fclose(fout);
    return LDH__BCKVERSION;
  }

  time_AtoAscii(
      &fh.creationtime, time_eFormat_DateAndTime, timstr, sizeof(timstr));

  //fprintf(fout, "Created:              %s\n", timstr);

  unsigned int to_chsize = sizeof(ch);
  unsigned int from_chsize = to_chsize;
  if (!from64) 
    from_chsize -= 4;

  for (c = 0; c < 2; c++) {
    fseek(f, fh.curdata[c], 0);
    fread(&ch, from_chsize, 1, f);

    pos = ftell(fout);
    for (int i = pos; i < fh.curdata[c]; i++)
      fputc(0, fout);
    fwrite(&ch, to_chsize, 1, fout);

    /* Work thru the data segments */

    for (d = 0; d < (int)ch.segments; d++) {
      csts = fread(&dh, sizeof dh, 1, f);
      csts = fwrite(&dh, sizeof dh, 1, fout);
      if (csts != 0) {
        if (dh.namesize > 0) {
          namep = (char*)malloc(dh.namesize + 1);
          csts = fread(namep, dh.namesize + 1, 1, f);
          csts = fwrite(namep, dh.namesize + 1, 1, fout);
        } else
          namep = NULL;
        datap = (unsigned char*)malloc(dh.size);
        csts = fread(datap, dh.size, 1, f);
        csts = fwrite(datap, dh.size, 1, fout);
      }
      if (csts == 0) {
        printf("Read error\n");
        break;
      }

      if (dh.valid) {

	// Print as hex code
	p = datap;
	for (i = 0; i < (int)dh.size; i++, p++) {
	  //if ((i % 16) == 0)
	  //  fprintf(fout, "\n	");
	  //fprintf(fout, "%02x ", *p);
	}
	//fprintf(fout, "\n");
      }
    }

    free(datap);
    free(namep);
  }

  fseek(f, fh.curdata[c], 0);
  fseek(f, 0, SEEK_END);
  pos = ftell(f);
  pos2 = ftell(fout);
  if (!from64)
    pos += 4;
  for (int i = pos2; i < pos; i++)
    fputc(0, fout);

  fclose(f);
  fclose(fout);

  return LDH__SUCCESS;
}


void usage()
{
    printf("\n\
Usage convert_bckcnv\n\nConvert backup file from 32 to 64 bit.\n\n\
upgrade_bckcnv \'from-file\' \'to-file\'\n\n");
    exit(0);
}

int main(int argc, char* argv[])
{
  pwr_tFileName fname, outfname;
  int from64;

  if (argc < 3)
    usage();
  if (argc == 4) {
    if (strcmp(argv[3], "-64") == 0)
      // Test for 64 to 64 bit (creating identical file). 
      from64 = 1;
    else
      usage();
  }

  dcli_translate_filename(fname, argv[1]);
  dcli_translate_filename(outfname, argv[2]);
  
  bck_cnv(fname, outfname, from64);

}
