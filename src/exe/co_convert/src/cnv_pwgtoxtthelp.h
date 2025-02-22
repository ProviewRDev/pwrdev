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

#ifndef cnv_pwgxtthelp_h
#define cnv_pwgxtthelp_h

#include <fstream>
#include <vector>

#include "pwr.h"

class CnvCtx;

class PwgFile {
  public:
  PwgFile(char* name, char* title, char* group);
  pwr_tFileName m_fname;
  char m_title[200];
  char m_group[200];
};

class CnvPwgToXtthelp {
  public:
  CnvPwgToXtthelp(CnvCtx* cnv_ctx);
  int exec_file(char* fname);
  int exec_filelist(char* title);
  int exec_group(int idx, int* next_idx);
  char* fname_to_topic(char* fname);
  int get_title(char* fname, char* title, int tsize, char* group, int gsize);
  void sort();
  void header();
  void chapter();
  void chapter_end();
  void headerlevel();
  void headerlevel_end();
  void print_enable();
  void print_disable();
  char* topic_name(char* str);

  std::vector<PwgFile> m_filelist;
  std::ofstream m_fp;
  char m_current_title[200];
  CnvCtx* ctx;
};

#endif
