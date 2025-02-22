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

#ifndef glow_annot_h
#define glow_annot_h

#include "glow_growctx.h"

class GlowAnnot : public GlowArrayElem {
public:
  GlowAnnot(GrowCtx* glow_ctx, double x = 0, double y = 0, int annot_num = 0,
      glow_eDrawType d_type = glow_eDrawType_TextHelveticaBold,
      glow_eDrawType color_d_type = glow_eDrawType_Line, int t_size = 2,
      glow_eAnnotType a_type = glow_eAnnotType_OneLine, int rel_pos = 0,
      glow_mDisplayLevel display_lev = glow_mDisplayLevel_1)
      : GlowArrayElem(glow_ctx), number(annot_num), p(glow_ctx, x, y), draw_type(d_type),
        text_size(t_size), annot_type(a_type), relative_pos(rel_pos),
        display_level(display_lev), color_drawtype(color_d_type),
        font(glow_eFont_Helvetica), protect(0){}
  void save(std::ofstream& fp, glow_eSaveMode mode);
  void open(std::ifstream& fp);
  void traverse(int x, int y)
  {
    p.traverse(x, y);
  }

  int number;
  GlowPoint p;
  glow_eDrawType draw_type;
  int text_size;
  glow_eAnnotType annot_type;
  int relative_pos;
  glow_mDisplayLevel display_level;
  glow_eDrawType color_drawtype;
  glow_eFont font;
  int protect;
};

void glow_measure_annot_text(GrowCtx* ctx, char* text, glow_eDrawType draw_type,
    int text_size, glow_eAnnotType annot_type, glow_eFont font, double* width,
    double* height, int* rows);

#endif
