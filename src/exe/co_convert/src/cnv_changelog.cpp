/*
 * ProviewR   Open Source Process Control.
 * Copyright (C) 2005-2025 SSAB EMEA AB.
 *
 * This file is part of ProviewR.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ProviewR. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking ProviewR statically or dynamically with other modules is
 * making a combined work based on ProviewR. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * ProviewR give you permission to, from the build function in the
 * ProviewR Configurator, combine ProviewR with modules generated by the
 * ProviewR PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every
 * copy of the combined work is accompanied by a complete copy of
 * the source code of ProviewR (the version used to produce the
 * combined work), being distributed under the terms of the GNU
 * General Public License plus this exception.
 */

#include <stdio.h>

#include <fstream>

extern "C" {
#include "co_dcli.h"
#include "co_cdh.h"
#include "co_time.h"
}
#include "co_string.h"
#include "cnv_changelog.h"

CnvChangeLog::CnvChangeLog(CnvCtx* cnv_ctx, char* from_str)
    : ctx(cnv_ctx), from(0)
{
  pwr_tStatus sts;

  if (!streq(from_str, "")) {
    sts = time_AsciiToA(from_str, &from_time);
    if (ODD(sts))
      from = 1;
  }

  read(0);
  read("src");
  read("xtt");
  read("wb");
  read("nmps");
  read("remote");
  read("misc");
  read("profibus");
  read("opc");
  read("java");
  read("tlog");
  read("bcomp");
  read("otherio");
  read("sev");
  read("othermanu");
  read("abb");
  read("siemens");
  read("ssabox");
  read("telemecanique");
  read("inor");
  read("klocknermoeller");
  // print();
  print_html();
}

int CnvChangeLog::read(const char* module)
{
  int sts;
  char orig_line[400];
  char line[400];
  char timstr1[40];
  char timstr2[40];
  FILE* fp;
  int i;
  pwr_tFileName fname;
  char* s;

  if (!module)
    sprintf(fname, "$pwre_croot/changelog.txt");
  else
    sprintf(fname, "$pwre_croot/%s/changelog.txt", module);
  dcli_translate_filename(fname, fname);
  fp = fopen(fname, "r");
  if (!fp)
    return 0;

  while (1) {
    sts = CnvCtx::read_line(orig_line, sizeof(orig_line), fp);
    if (!sts)
      break;
    else if (orig_line[0] == '#')
      continue;
    else {
      if (isdigit(orig_line[0])) {
        LogEntry p;

        // New log entry
        i = 0;
        for (s = orig_line; *s && *s != 32 && *s != 9; s++) {
          if (i >= (int)sizeof(timstr1))
            break;
          timstr1[i++] = *s;
        }
        timstr1[i] = 0;

        for (; *s && (*s == 32 || *s == 9); s++)
          ;

        i = 0;
        for (; *s && *s != 32 && *s != 9; s++) {
          if (i >= (int)sizeof(p.signature))
            break;
          p.signature[i++] = *s;
        }
        p.signature[i] = 0;

        for (; *s && (*s == 32 || *s == 9); s++)
          ;

        i = 0;
        for (; *s && *s != 32 && *s != 9; s++) {
          if (i >= (int)sizeof(p.component))
            break;
          p.component[i++] = *s;
        }
        p.component[i] = 0;

        for (; *s && (*s == 32 || *s == 9); s++)
          ;

        strncpy(p.text, s, sizeof(p.text));
        if (!module)
          strncpy(p.module, "", sizeof(p.module));
        else
          strncpy(p.module, module, sizeof(p.module));

        sprintf(timstr2, "20%c%c-%c%c-%c%c 00:00", timstr1[0], timstr1[1],
            timstr1[2], timstr1[3], timstr1[4], timstr1[5]);
        time_FormAsciiToA(timstr2, MINUTE, 0, &p.time);

        entries.push_back(p);
      } else {
        // Continuation of log entry
        str_trim(line, orig_line);
        if (streq(line, ""))
          continue;

        if (entries.size() == 0)
          continue;
        strcat(entries[entries.size() - 1].text, " ");
        strcat(entries[entries.size() - 1].text, line);
      }
    }
  }
  fclose(fp);

  return 1;
}

void CnvChangeLog::sort_time()
{
  int n = entries.size();

  for (int gap = n / 2; 0 < gap; gap /= 2) {
    for (int i = gap; i < n; i++) {
      for (int j = i - gap; 0 <= j; j -= gap) {
        if (entries[j + gap].time.tv_sec < entries[j].time.tv_sec) {
          LogEntry temp = entries[j];
          entries[j] = entries[j + gap];
          entries[j + gap] = temp;
        }
      }
    }
  }
}

void CnvChangeLog::sort_module()
{
  int n = entries.size();

  for (int gap = n / 2; 0 < gap; gap /= 2) {
    for (int i = gap; i < n; i++) {
      for (int j = i - gap; 0 <= j; j -= gap) {
        if (strcmp(entries[j + gap].module, entries[j].module) > 0) {
          LogEntry temp = entries[j];
          entries[j] = entries[j + gap];
          entries[j + gap] = temp;
        }
      }
    }
  }
}

void CnvChangeLog::sort_component()
{
  int n = entries.size();

  for (int gap = n / 2; 0 < gap; gap /= 2) {
    for (int i = gap; i < n; i++) {
      for (int j = i - gap; 0 <= j; j -= gap) {
        if (strcmp(entries[j + gap].component, entries[j].component) > 0) {
          LogEntry temp = entries[j];
          entries[j] = entries[j + gap];
          entries[j + gap] = temp;
        }
      }
    }
  }
}

void CnvChangeLog::sort_signature()
{
  int n = entries.size();

  for (int gap = n / 2; 0 < gap; gap /= 2) {
    for (int i = gap; i < n; i++) {
      for (int j = i - gap; 0 <= j; j -= gap) {
        if (strcmp(entries[j + gap].signature, entries[j].signature) > 0) {
          LogEntry temp = entries[j];
          entries[j] = entries[j + gap];
          entries[j + gap] = temp;
        }
      }
    }
  }
}

void CnvChangeLog::print()
{
  char timstr1[40];

  sort_time();
  for (int i = 0; i < (int)entries.size(); i++) {
    if (from) {
      if (time_Acomp(&entries[i].time, &from_time) < 0)
        continue;
    }

    time_AtoAscii(
        &entries[i].time, time_eFormat_DateAndTime, timstr1, sizeof(timstr1));
    timstr1[11] = 0;

    printf("%s %4s %-8s %-8s %s\n", timstr1, entries[i].signature,
        entries[i].module, entries[i].component, entries[i].text);
  }
}

void CnvChangeLog::print_docbook()
{
  char timstr1[40];
  pwr_tFileName fname = "$pwre_croot/src/doc/man/en_us/changelog.xml";
  dcli_translate_filename(fname, fname);

  std::ofstream fp(fname);

  fp << "<?xml version=\"1.0\" encoding=\"iso-latin-1\"?>\n"
     << "<!DOCTYPE book [\n"
     << "<!ENTITY % isopub PUBLIC\n"
     << "\"ISO 8879:1986//ENTITIES Publishing//EN//XML\"\n"
     << "\"/usr/share/xml/entities/xml-iso-entities-8879.1986/isopub.ent\"> "
        "<!-- \"http://www.w3.org/2003/entities/iso8879/isopub.ent\"> -->\n"
     << "%isopub;\n"
     << "]>\n"
     << "<article>\n"
     << "<title>Proview Changelog</title>\n"
     << "<section><title>Changelog entries</title>\n";

  sort_time();

  fp << "<table xml:id=\"changelog_\" border=\"1\"><tbody>\n"
     << "<tr><td><classname>Date_________</classname></td>\n"
     << "<td><classname>Module_____</classname></td>\n"
     << "<td><classname>Change</classname></td></tr>\n";

  for (int i = (int)entries.size() - 1; i >= 0; i--) {
    if (from) {
      if (time_Acomp(&entries[i].time, &from_time) < 0)
        continue;
    }

    time_AtoAscii(
        &entries[i].time, time_eFormat_DateAndTime, timstr1, sizeof(timstr1));
    timstr1[11] = 0;

    fp << "<tr><td>" << timstr1 << "</td><td>" << entries[i].module << "/"
       << entries[i].component << "</td>\n"
       << "<td>" << entries[i].text << " /" << entries[i].signature
       << "</td></tr>\n";
  }
  fp << "</tbody></table></section></article>\n";
}

void CnvChangeLog::print_html()
{
  char timstr1[40];
  pwr_tFileName fname = "$pwre_croot/src/doc/man/en_us/changelog.html";
  dcli_translate_filename(fname, fname);

  std::ofstream fp(fname);

  fp << "<html>\n"
     << "  <head>\n"
     << "    <link rel=\"stylesheet\" type=\"text/css\" href=\"../pcss.css\">\n"
     << "    <title>Proview Changelog</title>\n"
     << "  </head>\n"
     << "  <body id=\"news\">\n"
     << "    <div id=\"news\"><p id=\"news\">\n"
     << "    <h1>Proview Changelog</h1>\n"
     << "    <table border=\"1\" cellpadding=\"3\">\n";

  sort_time();

  fp << "     <tr><td><b>Date</b></td>\n"
     << "         <td><b>Module</b></td>\n"
     << "         <td><b>Change</b></td>\n"
     << "         <td><b>Sign</b></td></tr>\n";

  for (int i = (int)entries.size() - 1; i >= 0; i--) {
    if (from) {
      if (time_Acomp(&entries[i].time, &from_time) < 0)
        continue;
    }

    time_AtoAscii(
        &entries[i].time, time_eFormat_DateAndTime, timstr1, sizeof(timstr1));
    timstr1[11] = 0;

    fp << "<tr><td>" << timstr1 << "</td><td>" << entries[i].module << "/"
       << entries[i].component << "</td>\n"
       << "<td>" << entries[i].text << "</td><td>" << entries[i].signature
       << "</td></tr>\n";
  }
  fp << "    </table></div>\n"
     << "  </body>\n"
     << "</html>\n";
}

void CnvChangeLog::from_git()
{
  pwr_tFileName tmpfile = "/tmp/cnv_from_git.txt";
  pwr_tCmd cmd = "git log --pretty=format:\"%ai \'%cN\' %s\" >";
  char line[1000];
  char date[20];
  char user[80];
  char cuser[80];
  char *s1, *s2, *textp = 0;

  strcat(cmd, tmpfile);
  system(cmd);

  std::ifstream fp(tmpfile);
  while (fp.getline(line, sizeof(line))) {
    date[0] = line[2];
    date[1] = line[3];
    date[2] = line[5];
    date[3] = line[6];
    date[4] = line[8];
    date[5] = line[9];
    date[6] = 0;

    if ((s1 = strchr(line, '\'')) && (s2 = strchr(s1 + 1, '\''))) {
      *s2 = 0;
      strncpy(user, s1 + 1, sizeof(user));

      if (user[0] == 'R' || user[0] == 'r')
        strcpy(cuser, "rk");
      else
        strcpy(cuser, "cs");

      textp = s2 + 2;
    }

    if (textp)
      printf("%s %s  x         %s\n", date, cuser, textp);
  }

  fp.close();

  strcpy(cmd, "rm ");
  strcat(cmd, tmpfile);
  system(cmd);
}
