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

/* cnv_topdf.cpp --
   This file is a copy of cnv_topdf.cpp and cnv_content.cpp with the
   gdk_image calls removed.
*/

/*_Include files_________________________________________________________*/

#include <iomanip>
#include <iostream>

extern "C" {
#include "co_cdh.h"
#include "co_dcli.h"
}
#include "co_lng.h"
#include "co_string.h"

#include "flow_topdf.h"

#define pdf_cHead "%PDF-1.4"

CnvContentElem::CnvContentElem() : page_number(0), header_level(0)
{
  strcpy(text, "");
  strcpy(subject, "");
}

int CnvContent::find_link(char* subject, char* text, int* page)
{
  char key_part[4][40];
  char subject_part[4][40];
  int i, j;
  int subject_nr;
  int key_nr;
  int hit = 0;

  subject_nr = dcli_parse(subject, " 	", "", (char*)subject_part,
      sizeof(subject_part) / sizeof(subject_part[0]), sizeof(subject_part[0]),
      0);

  for (j = 0; j < (int)tab.size(); j++) {
    key_nr = dcli_parse(tab[j].subject, " 	", "", (char*)key_part,
        sizeof(key_part) / sizeof(key_part[0]), sizeof(key_part[0]), 0);
    if (key_nr == subject_nr) {
      for (i = 0; i < key_nr; i++) {
        if (str_NoCaseStrcmp(subject_part[i], key_part[i]) == 0) {
          if (i == key_nr - 1) {
            hit = 1;
            break;
          }
        } else
          break;
      }
    }
    if (hit)
      break;
  }
  if (!hit)
    return 0;
  strcpy(text, tab[j].text);
  *page = tab[j].page_number;
  return 1;
}

void CnvContent::add(CnvContentElem& elem)
{
  tab.push_back(elem);
}

CnvPdfObj::CnvPdfObj(CnvToPdf* otopdf, pdf_eObjType otype, int onumber)
    : topdf(otopdf), type(otype), number(onumber), length(0), offset(0),
      start(0), parent(0), first(0), last(0), next(0), prev(0), dest(0),
      count(0), resource(-1), xobject_cnt(0)
{
  strcpy(fontname, "");
  strcpy(text, "");
}

void CnvPdfObj::print_begin()
{
  offset = topdf->fp[topdf->cf].tellp();

  switch (type) {
  case pdf_eObjType_Catalog:
    topdf->fp[topdf->cf] << number << " 0 obj\n"
                         << "  << /Type /Catalog\n"
                         << "     /Outlines 2 0 R\n"
                         << "     /Pages " << number + topdf->v_outline.size()
                         << " 0 R\n";

    if (topdf->use_outlines)
      topdf->fp[topdf->cf] << "     /PageMode /UseOutlines\n";

    topdf->fp[topdf->cf] << "  >>\n"
                         << "endobj\n\n";
    break;

  case pdf_eObjType_Outline:
    topdf->fp[topdf->cf] << number << " 0 obj\n"
                         << "  << /Type /Outlines\n";
    if (first)
      topdf->fp[topdf->cf] << "     /First " << first + 1 << " 0 R\n";
    if (last)
      topdf->fp[topdf->cf] << "     /Last " << last + 1 << " 0 R\n";
    topdf->fp[topdf->cf] << "     /Count " << count << '\n'
                         << "  >>\n"
                         << "endobj\n"
                         << '\n';
    break;
  case pdf_eObjType_Pages:
    topdf->fp[topdf->cf] << number + topdf->v_outline.size() << " 0 obj\n"
                         << "  << /Type /Pages\n"
                         << "     /Kids [\n";
    for (int i = 1; i < (int)topdf->v_pages.size(); i++)
      topdf->fp[topdf->cf] << "              "
                           << topdf->v_outline.size() + topdf->v_pages[i].number
                           << " 0 R\n";

    topdf->fp[topdf->cf] << "           ]\n"
                         << "     /Count " << topdf->v_pages.size() - 1 << '\n'
                         << "  >>\n"
                         << "endobj\n"
                         << '\n';
    break;

  case pdf_eObjType_Page:
    topdf->fp[topdf->cf] << number + topdf->v_outline.size() << " 0 obj\n"
                         << "  << /Type /Page\n"
                         << "     /Parent " << topdf->v_outline.size() + 1
                         << " 0 R\n"
                         << "     /Mediabox [0 0 " << pdf_cPageWidth << " "
                         << pdf_cPageHeight << "]\n"
                         << "     /Contents "
                         << number - 1 + topdf->v_outline.size()
            + topdf->v_pages.size()
                         << " 0 R\n";
    if (resource == -1) {
      topdf->fp[topdf->cf] << "     /Resources << /Procset "
                           << topdf->v_outline.size() + topdf->v_pages.size()
              + topdf->v_content.size() + 1
                           << " 0 R\n"
                           << "                   /Font <<\n";
      for (int i = 1; i < (int)topdf->v_font.size(); i++)
        topdf->fp[topdf->cf]
            << "                         /F" << topdf->v_font[i].number - 1
            << " "
            << topdf->v_font[i].number + topdf->v_outline.size()
                + topdf->v_pages.size() + topdf->v_content.size()
            << " 0 R\n";

      topdf->fp[topdf->cf] << "                          >>\n"
                           << "                >>\n";
    } else {
      topdf->fp[topdf->cf] << "     /Resources "
                           << topdf->v_outline.size() + topdf->v_pages.size()
              + topdf->v_content.size() + +topdf->v_font.size() + resource + 1
                           << " 0 R\n";
    }
    topdf->fp[topdf->cf] << "  >>\n"
                         << "endobj\n\n";
    break;

  case pdf_eObjType_Content:
    topdf->fp[topdf->cf] << number + topdf->v_outline.size()
            + topdf->v_pages.size()
                         << " 0 obj\n"
                         << "  << /Length " << length << " >>\n"
                         << "stream\n";
    start = (int)topdf->fp[topdf->cf].tellp();
    break;

  case pdf_eObjType_Process:
    topdf->fp[topdf->cf] << number + topdf->v_outline.size()
            + topdf->v_pages.size() + topdf->v_content.size()
                         << " 0 obj\n"
                         << "  [/PDF /Text]\n"
                         << "endobj\n"
                         << '\n';
    break;

  case pdf_eObjType_Font:
    topdf->fp[topdf->cf] << number + topdf->v_outline.size()
            + topdf->v_pages.size() + topdf->v_content.size()
                         << " 0 obj\n"
                         << "  << /Type /Font\n"
                         << "     /Subtype /Type1\n"
                         << "     /Name /F" << number - 1 << '\n'
                         << "     /BaseFont /" << fontname << '\n'
                         << "     /Encoding /WinAnsiEncoding\n"
                         << "  >>\n"
                         << "endobj\n"
                         << '\n';
    break;

  case pdf_eObjType_OutlineO:
    topdf->fp[topdf->cf] << number << " 0 obj\n"
                         << "  << /Title (" << text << ")\n"
                         << "     /Parent " << parent + 1 << " 0 R\n";
    if (prev)
      topdf->fp[topdf->cf] << "     /Prev " << prev + 1 << " 0 R\n";
    if (next)
      topdf->fp[topdf->cf] << "     /Next " << next + 1 << " 0 R\n";
    if (first)
      topdf->fp[topdf->cf] << "     /First " << first + 1 << " 0 R\n";
    if (last)
      topdf->fp[topdf->cf] << "     /Last " << last + 1 << " 0 R\n";
    if (count)
      topdf->fp[topdf->cf] << "     /Count " << count << '\n';
    topdf->fp[topdf->cf] << "     /Dest [" << dest << " 0 R /XYZ null 700 null]"
                         << '\n'
                         << "  >>\n"
                         << "endobj\n"
                         << '\n';
    break;

  case pdf_eObjType_Image:
    print_image();
    break;

  case pdf_eObjType_Resource:
    topdf->fp[topdf->cf] << number + topdf->v_outline.size()
            + topdf->v_pages.size() + topdf->v_content.size()
            + topdf->v_font.size()
                         << " 0 obj\n"
                         << "  << /ProcSet [/PDF /Text /ImageB]\n"
                         << "     /XObject <<\n";

    for (int i = 0; i < xobject_cnt; i++) {
      topdf->fp[topdf->cf] << "                  /Im" << xobject[i] + 1 << " "
                           << xobject[i] + 1 + topdf->v_outline.size()
              + topdf->v_pages.size() + topdf->v_content.size()
              + topdf->v_font.size() + topdf->v_resource.size()
                           << " 0 R\n";
    }
    topdf->fp[topdf->cf] << "              >>\n"
                         << "     /Font <<\n";
    for (int i = 1; i < (int)topdf->v_font.size(); i++)
      topdf->fp[topdf->cf] << "               /F" << topdf->v_font[i].number - 1
                           << " "
                           << topdf->v_font[i].number + topdf->v_outline.size()
              + topdf->v_pages.size() + topdf->v_content.size()
                           << " 0 R\n";

    topdf->fp[topdf->cf] << "           >>\n"
                         << "  >>\n"
                         << "endobj\n";
    break;
  }
}

void CnvPdfObj::print_end()
{
  switch (type) {
  case pdf_eObjType_Content:
    length = (int)topdf->fp[topdf->cf].tellp() - start;
    topdf->fp[topdf->cf] << "endstream\n"
                         << "endobj\n\n";
    break;
  default:;
  }
}

int CnvPdfObj::print_image()
{
  return 1;
}

CnvToPdf::CnvToPdf()
    : cf(0), ci(0), prev_ci(0), conf_pass(false), xref_offset(0), im_cnt(0),
      use_outlines(1)
{
  for (int i = 0; i < pdf_cMaxLevel; i++)
    header_number[i] = 0;
  for (int i = 0; i < pdf_eFile__; i++)
    page_number[i] = 0;
  strcpy(previous_chapter, "");
  strcpy(current_chapter, "");

  style[pdf_eId_TitlePage].h1 = CnvStyle("Helvetica-Bold-ISOLatin1", 35, 0, 100,
      20, cnv_eAlignment_Center, 0, 0, 0);
  style[pdf_eId_TitlePage].h2 = CnvStyle("Helvetica-Bold-ISOLatin1", 25, 0, 50,
      20, cnv_eAlignment_Center, 0, 0, 0);
  style[pdf_eId_TitlePage].h3 = CnvStyle("Helvetica-Bold-ISOLatin1", 15, 0, 25,
      10, cnv_eAlignment_Center, 0, 0, 0);
  style[pdf_eId_TitlePage].text = CnvStyle(
      "Helvetica-ISOLatin1", 8, 0, 9, 1, cnv_eAlignment_Center, 0, 0, 0);
  style[pdf_eId_TitlePage].boldtext = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 8, 0, 9, 1, cnv_eAlignment_Center, 0, 0, 0);
  style[pdf_eId_InfoPage].h1 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 24, 0, 100, 20, cnv_eAlignment_Left, 0, 0, 0);
  style[pdf_eId_Chapter].h1 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 24, 0, 24, 40, cnv_eAlignment_Left, 0, 1, 1);
  style[pdf_eId_TopicL2].h1 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 12, 0, 20, 10, cnv_eAlignment_Left, 0, 0, 1);
  style[pdf_eId_TopicL2].h2 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 10, 0, 16, 8, cnv_eAlignment_Left, 0, 0, 0);
  style[pdf_eId_TopicL3].h1 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 10, 0, 16, 8, cnv_eAlignment_Left, 0, 0, 1);
  style[pdf_eId_TopicL3].h2 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 8, 0, 16, 5, cnv_eAlignment_Left, 0, 0, 0);
  style[pdf_eId_Function].h1 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 24, 0, 24, 20, cnv_eAlignment_Left, 1, 0, 1);
  style[pdf_eId_Class].h1 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 24, 0, 24, 20, cnv_eAlignment_Left, 1, 0, 1);
  style[pdf_eId_Class].h2 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 20, 0, 24, 8, cnv_eAlignment_Left, 0, 0, 0);
  style[pdf_eId_Class].h3 = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 16, -40, 24, 6, cnv_eAlignment_Left, 0, 0, 0);
  style[pdf_eId_Content].boldtext = CnvStyle(
      "Helvetica-Bold-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
  style[pdf_eId_Chapter].text = CnvStyle(
      "TimesNewRoman-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
  style[pdf_eId_TopicL1].text = CnvStyle(
      "TimesNewRoman-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
  style[pdf_eId_TopicL2].text = CnvStyle(
      "TimesNewRoman-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
  style[pdf_eId_TopicL3].text = CnvStyle(
      "TimesNewRoman-ISOLatin1", 10, 0, 11, 1, cnv_eAlignment_Left, 0, 0, 0);
}

void CnvToPdf::cnv_text(char* to, char* from)
{
  if (!from) {
    strcpy(to, "");
    return;
  }

  char* t = to;
  char* s = from;

  for (; *s; s++) {
    switch (*s) {
    case '(':
      *t++ = '\\';
      *t++ = '(';
      break;
    case ')':
      *t++ = '\\';
      *t++ = ')';
      break;
    case '\\':
      *t++ = '\\';
      *t++ = '\\';
    default:
      *t++ = *s;
    }
  }
  *t = 0;
}

void CnvToPdf::print_text(char* text, CnvStyle& style, int mode)
{
  char str[1000];

  cnv_text(str, text);

  if (style.sidebreak && mode & pdf_mPrintMode_Pos) {
    if (page_number[cf] == 0) {
      // First header, no pagebreak
      page_number[cf] = 1;
    } else {
      print_pagebreak(0);
    }
  } else if (style.pagebreak && mode & pdf_mPrintMode_Pos) {
    print_pagebreak(0);
  }

  if (mode & pdf_mPrintMode_Pos || mode & pdf_mPrintMode_Start) {
    y -= style.top_offset;

    if (y - style.bottom_offset < pdf_cBottomMargin) {
      print_pagebreak(0);
    }
  } else
    y += style.bottom_offset;

  if (!(mode & pdf_mPrintMode_FixX)) {
    if (style.alignment == cnv_eAlignment_Center) {
      x = pdf_cLeftMargin / 2 + (pdf_cPageWidth - pdf_cLeftMargin / 2) / 2
          - 0.50 * strlen(text) * style.font_size / 2;
      if (x < pdf_cLeftMargin / 2)
        x = pdf_cLeftMargin / 2;
    } else
      x = pdf_cLeftMargin + style.indentation;
  }

  int pmode = mode & 31;

  if (!streq(text, "")) {
    switch (pmode) {
    case pdf_mPrintMode_Pos:
    case pdf_mPrintMode_KeepY: {
      // Full path with beginning and end
      fp[cf] << "  BT\n"
             << "    " << fontname(style) << " " << style.font_size << " Tf"
             << '\n'
             << "    " << x << " " << y << " Td\n"
             << "    (" << str << ") Tj\n"
             << "  ET\n";
      break;
    }
    case pdf_mPrintMode_Start: {
      // Start new path
      fp[cf] << "  BT\n"
             << "    " << fontname(style) << " " << style.font_size << " Tf"
             << '\n'
             << "    " << x << " " << y << " Td\n"
             << "    (" << str << ") Tj\n";
      break;
    }
    case pdf_mPrintMode_Continue: {
      // Continue current path
      fp[cf] << "    " << fontname(style) << " " << style.font_size << " Tf"
             << '\n'
             << "    " << x << " " << y << " Td\n"
             << "    (" << str << ") Tj\n";
      break;
    }
    case pdf_mPrintMode_End: {
      // Continue and close current path
      fp[cf] << "    " << fontname(style) << " " << style.font_size << " Tf"
             << '\n'
             << "    (" << str << ") Tj\n"
             << "  ET\n";
      break;
    }
    default:;
    }
  } else {
    switch (pmode) {
    case pdf_mPrintMode_Start: {
      // Start new path
      fp[cf] << "  BT\n"
             << "    " << x << " " << y << " Td\n";
      break;
    }
    case pdf_mPrintMode_End: {
      // Continue and close current path
      fp[cf] << "  ET\n";
      break;
    }
    default:;
    }
  }
  y -= style.bottom_offset;
}

void CnvToPdf::draw_rect(double lw, double x, double y, double w, double h)
{
  fp[cf] << "  " << lw << " w\n"
         << "  " << x << " " << y << " " << w << " " << h << " re S\n";
}

void CnvToPdf::draw_triangle(double lw, double x, double y, double w, double h)
{
  fp[cf] << "  " << lw << " w\n"
         << "  " << x << " " << y << " m\n"
         << "  " << x + w / 2 << " " << y + h << " l\n"
         << "  " << x + w << " " << y << " l\n"
         << "  " << x << " " << y << " l\n"
         << "  S\n";
}

void CnvToPdf::draw_filled_triangle(
    flow_eDrawType color, double x, double y, double w, double h)
{
  if (color == flow_eDrawType_LineRed) {
    fp[cf] << "  /DeviceRGB cs\n"
           << "  " << 1.0 << " " << 0.2 << " " << 0.2 << " sc\n";
  } else if (color == flow_eDrawType_Yellow) {
    fp[cf] << "  /DeviceRGB cs\n"
           << "  " << 1.0 << " " << 1.0 << " " << 0.0 << " sc\n";
  }

  fp[cf] << "  " << x << " " << y << " m\n"
         << "  " << x + w / 2 << " " << y + h << " l\n"
         << "  " << x + w << " " << y << " l\n"
         << "  " << x << " " << y << " l\n"
         << "  f\n";

  if (color == flow_eDrawType_LineRed || color == flow_eDrawType_Yellow) {
    fp[cf] << "  " << 0.0 << " " << 0.0 << " " << 0.0 << " sc\n";
  }
}

void CnvToPdf::draw_arc(
    double lw, double x, double y, double w, double h, int angle1, int angle2)
{
  fp[cf] << "  " << lw << " w\n";
  if ((angle1 == 0 && angle2 >= 90) || (angle1 == 90 && angle2 >= 360)
      || (angle1 == 180 && angle2 >= 270) || (angle1 == 270 && angle2 >= 180))
    fp[cf] << "  " << x + w << " " << y + h / 2 << " m"
           << "  " << x + w << " " << y + h << " " << x + w << " " << y + h
           << " " << x + w / 2 << " " << y + h << " "
           << " c\n";
  if ((angle1 == 0 && angle2 >= 180) || (angle1 == 90 && angle2 >= 90)
      || (angle1 == 180 && angle2 >= 360) || (angle1 == 270 && angle2 >= 270))
    fp[cf] << "  " << x + w / 2 << " " << y + h << " m"
           << "  " << x << " " << y + h << " " << x << " " << y + h << " " << x
           << " " << y + h / 2 << " "
           << " c\n";
  if ((angle1 == 0 && angle2 >= 270) || (angle1 == 90 && angle2 >= 180)
      || (angle1 == 180 && angle2 >= 90) || (angle1 == 270 && angle2 >= 360))
    fp[cf] << "  " << x << " " << y + h / 2 << " m"
           << "  " << x << " " << y << " " << x << " " << y << " " << x + w / 2
           << " " << y << " "
           << " c\n";
  if ((angle1 == 0 && angle2 >= 360) || (angle1 == 90 && angle2 >= 270)
      || (angle1 == 180 && angle2 >= 180) || (angle1 == 270 && angle2 >= 90))
    fp[cf] << "  " << x + w / 2 << " " << y << " m"
           << "  " << x + w << " " << y << " " << x + w << " " << y << " "
           << x + w << " " << y + h / 2 << " "
           << " c\n";
  fp[cf] << "  S\n";
}

void CnvToPdf::draw_line(
    double lw, double x1, double y1, double x2, double y2, int dashed, int gray)
{
  fp[cf] << "  " << lw << " w\n";
  if (dashed)
    fp[cf] << "  [2 3] 1 d\n";
  if (gray)
    fp[cf] << " 0.7 G\n";
  fp[cf] << "  " << x1 << " " << y1 << " m\n"
         << "  " << x2 << " " << y2 << " l S\n";
  if (dashed)
    fp[cf] << "  [] 0 d\n";
  if (gray)
    fp[cf] << "  0 G\n";
}
void CnvToPdf::draw_arrow(
    double x1, double y1, double x2, double y2, double x3, double y3, int gray)
{
  if (gray)
    fp[cf] << " 0.7 G\n"
           << " 0.7 g\n";
  fp[cf] << "  " << x1 << " " << y1 << " m\n"
         << "  " << x2 << " " << y2 << " l\n"
         << "  " << x3 << " " << y3 << " l\n"
         << "  " << x1 << " " << y1 << " l\n"
         << "  B\n";
  if (gray)
    fp[cf] << "  0 G\n"
           << "  0 g\n";
}

void CnvToPdf::draw_text(double x, double y, char* text, int bold, double size)
{
  char fontname[20];

  if (bold)
    strcpy(fontname, "/F2");
  else
    strcpy(fontname, "/F1");

  fp[cf] << "  BT\n"
         << "    " << fontname << " " << size << " Tf\n"
         << "    " << x << " " << y << " Td\n"
         << "    (" << text << ") Tj\n"
         << "  ET\n";
}

void CnvToPdf::print_pagebreak(int last)
{
  if (page_number[cf] == 0)
    page_number[cf] = 1;
  if (page_number[cf] != 1 || last) {
    double page_x;

    page_x = pdf_cPageNumX;

    if (!(prev_ci == pdf_eId_TitlePage || prev_ci == pdf_eId_InfoPage)) {
      prev_ci = ci;

      fp[cf] << "  1 w\n"
             << "  " << 10 << " " << pdf_cPageHeight - 20 << " m\n"
             << "  " << pdf_cPageWidth + 65 << " " << pdf_cPageHeight - 20
             << " l\n"
             << "  S\n"
             << "  BT\n"
             << "    /F1 10 Tf\n"
             << "    "
             << pdf_cPageWidth / 2 - 10 * 0.5 * strlen(previous_chapter) << " "
             << pdf_cPageNumY << " Td\n"
             << "    (" << previous_chapter << ") Tj\n"
             << "  ET\n"
             << "  BT\n"
             << "    /F1 10 Tf\n"
             << "    " << page_x << " " << pdf_cPageNumY << " Td\n"
             << "    (" << page_number[cf] - 1 << ") Tj\n"
             << "  ET\n";
    }

    if (page_number[cf] > 1 && v_content.size() > 0)
      v_content[page_number[cf] - 2].print_end();
  }

  if (last)
    return;

  if (conf_pass) {
    CnvPdfObj o1 = CnvPdfObj(this, pdf_eObjType_Page, v_pages.size() + 1);
    v_pages.push_back(o1);

    CnvPdfObj o2 = CnvPdfObj(this, pdf_eObjType_Content, v_content.size() + 1);
    v_content.push_back(o2);
  }

  if ((int)v_content.size() > page_number[cf] - 1)
    v_content[page_number[cf] - 1].print_begin();

  page_number[cf]++;

  y = pdf_cPageHeight - pdf_cTopMargin;
}

void CnvToPdf::print_content()
{
  int size = content.tab.size();
  int level = 0;
  int prev[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  int current = v_outline.size();
  int root = 1;
  int parent[4] = { 0, 0, 0, 0 };
  int offset = current;

  if (!current)
    return;

  v_outline[root].first = current;
  v_outline[root].last = current + size - 1;
  v_outline[root].count = size;
  parent[level] = root;

  for (int i = 0; i < size; i++) {
    level = content.tab[i].header_level;
    if (level < 0)
      level = 0;

    CnvPdfObj o1 = CnvPdfObj(this, pdf_eObjType_OutlineO, current + 1);
    strcpy(o1.text, content.tab[i].text);
    o1.parent = parent[level];
    if (parent[level] != root) {
      v_outline[parent[level]].last = current;
      v_outline[parent[level]].count++;
    }
    if (i != size - 1) {
      if (content.tab[i + 1].header_level > content.tab[i].header_level) {
        parent[level + 1] = current;
        o1.first = current + 1;
      }
    }
    if (prev[level]) {
      o1.prev = prev[level];
      v_outline[o1.prev].next = current;
    }
    o1.dest = size + offset + content.tab[i].page_number;

    prev[level] = current;
    v_outline.push_back(o1);
    current++;

    if (i != size - 1) {
      int next_level = content.tab[i + 1].header_level;
      if (next_level < 0)
        next_level = 0;
      if (level > next_level) {
        for (int j = next_level + 1; j <= level; j++)
          prev[j] = 0;
      }
    }
  }
}

CnvToPdf::~CnvToPdf()
{
}

void CnvToPdf::close()
{
  cf = pdf_eFile_Body;
  print_pagebreak(1);
  // print_content();

  for (int i = 0; i < (int)v_font.size(); i++) {
    v_font[i].print_begin();
    v_font[i].print_end();
  }

  for (int i = 0; i < (int)v_resource.size(); i++) {
    v_resource[i].print_begin();
    v_resource[i].print_end();
  }

  for (int i = 0; i < (int)v_image.size(); i++) {
    v_image[i].print_begin();
    v_image[i].print_end();
  }

  xref_offset = fp[cf].tellp();
  fp[cf] << '\n'
         << "xref\n"
         << "0 "
         << v_outline.size() + v_pages.size() + v_content.size() + v_font.size()
          + v_resource.size() + v_image.size() + 1
         << '\n'
         << "0000000000 65535 f \n";

  for (int i = 0; i < (int)v_outline.size(); i++) {
    fp[cf].fill('0');
    fp[cf].width(10);
    fp[cf] << v_outline[i].offset << " 00000 n \n";
  }
  for (int i = 0; i < (int)v_pages.size(); i++) {
    fp[cf].fill('0');
    fp[cf].width(10);
    fp[cf] << v_pages[i].offset << " 00000 n \n";
  }
  for (int i = 0; i < (int)v_content.size(); i++) {
    fp[cf].fill('0');
    fp[cf].width(10);
    fp[cf] << v_content[i].offset << " 00000 n \n";
  }
  for (int i = 0; i < (int)v_font.size(); i++) {
    fp[cf].fill('0');
    fp[cf].width(10);
    fp[cf] << v_font[i].offset - start_offset << " 00000 n \n";
  }
  for (int i = 0; i < (int)v_resource.size(); i++) {
    fp[cf].fill('0');
    fp[cf].width(10);
    fp[cf] << v_resource[i].offset - start_offset << " 00000 n \n";
  }
  for (int i = 0; i < (int)v_image.size(); i++) {
    fp[cf].fill('0');
    fp[cf].width(10);
    fp[cf] << v_image[i].offset - start_offset << " 00000 n \n";
  }

  fp[cf] << '\n'
         << "trailer\n"
         << "  << /Size "
         << v_outline.size() + v_pages.size() + v_content.size() + v_font.size()
         << '\n'
         << "     /Root 1 0 R\n"
         << "  >>\n"
         << "startxref\n"
         << xref_offset - start_offset << '\n'
         << "%%EOF\n";

  fp[pdf_eFile_Body].close();
}

void CnvToPdf::print_horizontal_line()
{
  y -= 3;
  fp[cf] << "  1 w\n"
         << "  " << pdf_cLeftMargin - 50 << " " << y << " m\n"
         << "  " << pdf_cPageWidth << " " << y << " l\n"
         << "  S\n";

  y -= 3;
}

int CnvToPdf::print_image_inline(char* filename)
{
  return 1;
}

int CnvToPdf::print_image(char* filename)
{
  return 1;
}

void CnvToPdf::set_pageheader(char* text)
{
  strcpy(previous_chapter, current_chapter);
  strcpy(current_chapter, text);
}

void CnvToPdf::print_h1(char* text, int hlevel, char* subject)
{
  char hnum[40];

  if (ci == pdf_eId_Chapter) {
    set_pageheader(text);
  }

  if (style[ci].h1.display_number) {
    if (hlevel < 0)
      hlevel = 0;
    if (hlevel > pdf_cMaxLevel - 1)
      hlevel = pdf_cMaxLevel - 1;
    header_number[hlevel]++;
    switch (hlevel) {
    case 0:
      sprintf(hnum, "%d", header_number[0]);
      break;
    case 1:
      sprintf(hnum, "%d.%d", header_number[0], header_number[1]);
      break;
    case 2:
      sprintf(hnum, "%d.%d.%d", header_number[0], header_number[1],
          header_number[2]);
      break;
    case 3:
      sprintf(hnum, "%d.%d.%d.%d", header_number[0], header_number[1],
          header_number[2], header_number[3]);
      break;
    default:;
    }
    x = pdf_cLeftMargin - 50;
    print_text(hnum, style[ci].h1, pdf_mPrintMode_Pos | pdf_mPrintMode_FixX);

    double x0 = x + 12.0 * (strlen(hnum) + 1) * style[ci].h1.font_size / 24;
    x = pdf_cLeftMargin;
    if (x0 > x)
      x = x0;

    print_text(text, style[ci].h1, pdf_mPrintMode_KeepY | pdf_mPrintMode_FixX);
  } else
    print_text(text, style[ci].h1);

  if (conf_pass && !(ci == pdf_eId_TitlePage || ci == pdf_eId_InfoPage)) {
    CnvContentElem cnt;
    cnt.page_number = page_number[cf];
    cnt.header_level = hlevel;
    strcpy(cnt.header_number, hnum);
    strcpy(cnt.text, text);
    strcpy(cnt.subject, subject);
    content.add(cnt);
  }
  strcpy(previous_chapter, current_chapter);
}

void CnvToPdf::print_h2(char* text)
{
  print_text(text, style[ci].h2);
}

void CnvToPdf::print_h3(char* text)
{
  print_text(text, style[ci].h3);
}

void CnvToPdf::set_filename(int idx, char* name)
{
  strcpy(filename[idx], name);
}

void CnvToPdf::open()
{
  y = pdf_cPageHeight - pdf_cTopMargin;
  im_cnt = 0;

  if (conf_pass) {
    fp[pdf_eFile_Body].open(filename[pdf_eFile_Body]);
    fp[pdf_eFile_Body] << setiosflags(std::ios::fixed) << std::setprecision(6);
    start_offset = fp[pdf_eFile_Body].tellp();
  } else {
    print_content();

    conf_pass = true;
    page_number[cf] = v_content.size() + 1;
    print_pagebreak(1);
    conf_pass = false;
    for (int i = 0; i < (int)v_image.size(); i++) {
      v_image[i].print_begin();
      v_image[i].print_end();
    }
    // if ( v_content.size() > 0 && v_content[v_content.size()-1].length == 0)
    //  v_content[v_content.size()-1].length = (int) fp[pdf_eFile_Body].tellp()
    //  -
    //	v_content[v_content.size()-1].start;
    fp[pdf_eFile_Body].seekp(start_offset);
    strcpy(previous_chapter, "");
    strcpy(current_chapter, "");
  }
  cf = pdf_eFile_Body;
  page_number[cf] = 0;

  if (conf_pass) {
    CnvPdfObj o1 = CnvPdfObj(this, pdf_eObjType_Catalog, v_outline.size() + 1);
    v_outline.push_back(o1);

    CnvPdfObj o2 = CnvPdfObj(this, pdf_eObjType_Outline, v_outline.size() + 1);
    v_outline.push_back(o2);

    CnvPdfObj o3 = CnvPdfObj(this, pdf_eObjType_Pages, v_pages.size() + 1);
    v_pages.push_back(o3);

    CnvPdfObj o4 = CnvPdfObj(this, pdf_eObjType_Process, v_font.size() + 1);
    v_font.push_back(o4);

    CnvPdfObj o5 = CnvPdfObj(this, pdf_eObjType_Font, v_font.size() + 1);
    strcpy(o5.fontname, "Helvetica");
    v_font.push_back(o5);

    CnvPdfObj o6 = CnvPdfObj(this, pdf_eObjType_Font, v_font.size() + 1);
    strcpy(o6.fontname, "Helvetica-Bold");
    v_font.push_back(o6);

    CnvPdfObj o7 = CnvPdfObj(this, pdf_eObjType_Font, v_font.size() + 1);
    strcpy(o7.fontname, "Helvetica-Oblique");
    v_font.push_back(o7);

    CnvPdfObj o8 = CnvPdfObj(this, pdf_eObjType_Font, v_font.size() + 1);
    strcpy(o8.fontname, "TimesNewRoman");
    v_font.push_back(o8);

    CnvPdfObj o9 = CnvPdfObj(this, pdf_eObjType_Font, v_font.size() + 1);
    strcpy(o9.fontname, "TimesNewRoman,Bold");
    v_font.push_back(o9);

    CnvPdfObj o10 = CnvPdfObj(this, pdf_eObjType_Font, v_font.size() + 1);
    strcpy(o10.fontname, "Courier");
    v_font.push_back(o10);
  }

  fp[cf] << pdf_cHead << '\n';

  for (int i = 0; i < (int)v_outline.size(); i++) {
    v_outline[i].print_begin();
    v_outline[i].print_end();
  }
  for (int i = 0; i < (int)v_pages.size(); i++) {
    v_pages[i].print_begin();
    v_pages[i].print_end();
  }

  print_pagebreak(0);

  for (int i = 1; i < pdf_cMaxLevel; i++)
    header_number[i] = 0;
}

void CnvToPdf::incr_headerlevel()
{
  ci++;
  if (ci > pdf_eId_TopicL3)
    ci = pdf_eId_TopicL3;
  if (ci < pdf_eId_TopicL1)
    ci = pdf_eId_TopicL1;

  header_number[ci - (int)pdf_eId_Chapter] = 0;
}

void CnvToPdf::decr_headerlevel()
{
  ci--;
  if (ci < pdf_eId_TopicL1)
    ci = pdf_eId_TopicL1;
}

void CnvToPdf::reset_headernumbers(int level)
{
  for (int i = level; i < pdf_cMaxLevel; i++)
    header_number[i] = 0;
}

char* CnvToPdf::fontname(CnvStyle& style)
{
  static char str[80];
  char name[80];

  strcpy(name, style.font.c_str());
  if (str_StartsWith(name, "Helvetica-Bold"))
    strcpy(str, "/F2");
  else if (str_StartsWith(name, "Helvetica-Oblique"))
    strcpy(str, "/F3");
  else if (str_StartsWith(name, "Helvetica"))
    strcpy(str, "/F1");
  else if (str_StartsWith(name, "TimesNewRoman-Bold"))
    strcpy(str, "/F5");
  else if (str_StartsWith(name, "TimesNewRoman"))
    strcpy(str, "/F4");
  else if (str_StartsWith(name, "Courier"))
    strcpy(str, "/F6");
  else
    strcpy(str, "/F1");

  return str;
}
