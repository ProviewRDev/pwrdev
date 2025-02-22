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

#ifndef cnv_tops_h
#define cnv_tops_h

/* cnv_tops.h -- Postscript output */

#include <fstream>

#include "cnv_content.h"

#define ps_cMaxLevel 4
#define ps_cPageHeight 820
#define ps_cPageWidth 535
#define ps_cPageNumX (ps_cPageWidth - 10)
#define ps_cPageNumY (ps_cPageHeight - 13)
#define ps_cLeftMargin 100
#define ps_cTopMargin 100
#define ps_cBottomMargin 50
#define ps_cCellSize 110
#define ps_cTmpFile "/tmp/ptmp.ps"

typedef enum {
  ps_mPrintMode_Pos = 1 << 0,
  ps_mPrintMode_Start = 1 << 1,
  ps_mPrintMode_Continue = 1 << 2,
  ps_mPrintMode_End = 1 << 3,
  ps_mPrintMode_KeepY = 1 << 4,
  ps_mPrintMode_FixX = 1 << 5
} ps_mPrintMode;

typedef enum { ps_eFile_Info, ps_eFile_Body, ps_eFile__ } ps_eFile;

typedef enum {
  ps_eId_TitlePage,
  ps_eId_InfoPage,
  ps_eId_Content,
  ps_eId_Chapter,
  ps_eId_TopicL1,
  ps_eId_TopicL2,
  ps_eId_TopicL3,
  ps_eId_Function,
  ps_eId_Class,
  ps_eId_Report,
  ps_eId__
} ps_eId;

class CnvToPs {
  public:
  CnvToPs() : cf(0), ci(0), conf_pass(false)
  {
    for (int i = 0; i < ps_cMaxLevel; i++)
      header_number[i] = 0;
    for (int i = 0; i < ps_eFile__; i++)
      page_number[i] = 0;
    strcpy(previous_chapter, "");
    strcpy(current_chapter, "");

    style[ps_eId_TitlePage].h1 = CnvStyle("Helvetica-Bold-ISOLatin1", 35, 0,
        100, 20, cnv_eAlignment_Center, 0, 0, 0);
    style[ps_eId_TitlePage].h2 = CnvStyle("Helvetica-Bold-ISOLatin1", 25, 0, 50,
        20, cnv_eAlignment_Center, 0, 0, 0);
    style[ps_eId_TitlePage].h3 = CnvStyle("Helvetica-Bold-ISOLatin1", 15, 0, 25,
        10, cnv_eAlignment_Center, 0, 0, 0);
    style[ps_eId_TitlePage].text = CnvStyle(
        "Helvetica-ISOLatin1", 8, 0, 9, 1, cnv_eAlignment_Center, 0, 0, 0);
    style[ps_eId_TitlePage].boldtext = CnvStyle(
        "Helvetica-Bold-ISOLatin1", 8, 0, 9, 1, cnv_eAlignment_Center, 0, 0, 0);
    style[ps_eId_Chapter].h1 = CnvStyle("Helvetica-Bold-ISOLatin1", 24, 0, 24,
        40, cnv_eAlignment_Left, 0, 1, 1);
    style[ps_eId_TopicL2].h1 = CnvStyle("Helvetica-Bold-ISOLatin1", 12, 0, 20,
        10, cnv_eAlignment_Left, 0, 0, 1);
    style[ps_eId_TopicL2].h2 = CnvStyle(
        "Helvetica-Bold-ISOLatin1", 10, 0, 16, 8, cnv_eAlignment_Left, 0, 0, 0);
    style[ps_eId_TopicL3].h1 = CnvStyle(
        "Helvetica-Bold-ISOLatin1", 10, 0, 16, 8, cnv_eAlignment_Left, 0, 0, 1);
    style[ps_eId_TopicL3].h2 = CnvStyle(
        "Helvetica-Bold-ISOLatin1", 8, 0, 16, 5, cnv_eAlignment_Left, 0, 0, 0);
    style[ps_eId_Function].h1 = CnvStyle("Helvetica-Bold-ISOLatin1", 24, 0, 24,
        20, cnv_eAlignment_Left, 1, 0, 1);
    style[ps_eId_Class].h1 = CnvStyle("Helvetica-Bold-ISOLatin1", 24, 0, 24, 20,
        cnv_eAlignment_Left, 1, 0, 1);
    style[ps_eId_Class].h2 = CnvStyle(
        "Helvetica-Bold-ISOLatin1", 20, 0, 24, 8, cnv_eAlignment_Left, 0, 0, 0);
    style[ps_eId_Class].h3 = CnvStyle("Helvetica-Bold-ISOLatin1", 16, -40, 24,
        6, cnv_eAlignment_Left, 0, 0, 0);
    style[ps_eId_Content].boldtext = CnvStyle(
        "Helvetica-Bold-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
    style[ps_eId_Chapter].text = CnvStyle(
        "Times-Roman-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
    style[ps_eId_TopicL1].text = CnvStyle(
        "Times-Roman-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
    style[ps_eId_TopicL2].text = CnvStyle(
        "Times-Roman-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
    style[ps_eId_TopicL3].text = CnvStyle(
        "Times-Roman-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
    style[ps_eId_Report].h1 = CnvStyle("Helvetica-Bold-ISOLatin1", 24, 0, 24,
        20, cnv_eAlignment_Left, 0, 0, 0);
  }
  virtual ~CnvToPs();

  virtual void close();
  virtual void print_text(
      const char* text, CnvStyle& style, int mode = ps_mPrintMode_Pos);
  virtual void print_pagebreak(int print_num);
  virtual void print_content();
  void print_h1(const char* text, int hlevel, char* subject);
  void print_h2(const char* text);
  void print_h3(const char* text);
  virtual void print_horizontal_line();
  virtual int print_image(const char* filename);
  void cnv_text(char* to, const char* from);
  void set_confpass(bool conf)
  {
    conf_pass = conf;
    if (!conf) {
      // Reset
      for (int i = 0; i < ps_cMaxLevel; i++)
        header_number[i] = 0;
      for (int i = 0; i < ps_eFile__; i++)
        page_number[i] = 0;
      cf = 0;
      ci = 0;
    }
  }
  virtual void set_ci(int val)
  {
    ci = val;
  }
  virtual void set_cf(int val)
  {
    cf = val;
  }
  void set_filename(int idx, const char* name)
  {
    strcpy(filename[idx], name);
  }
  virtual void open();
  void incr_headerlevel();
  void decr_headerlevel();
  void reset_headernumbers(int level);
  void set_pageheader(const char* text);

  CnvContent content;
  CnvIdStyle style[ps_eId__];
  std::ofstream fp[ps_eFile__];
  pwr_tFileName filename[ps_eFile__];
  int cf;
  int ci;
  double x;
  double y;
  int page_number[ps_eFile__];
  int header_number[ps_cMaxLevel];
  bool conf_pass;
  char current_chapter[160];
  char previous_chapter[160];
};

#endif
