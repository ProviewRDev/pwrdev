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

#ifndef flow_pixmap_h
#define flow_pixmap_h

#include "flow_point.h"

class FlowPixmap : public FlowArrayElem {
public:
  FlowPixmap(FlowCtx* flow_ctx, flow_sPixmapData* pixmap_data, double x = 0,
      double y = 0, flow_eDrawType d_type = flow_eDrawType_Line, int size = 2);
  FlowPixmap(const FlowPixmap& p);
  ~FlowPixmap();
  friend std::ostream& operator<<(std::ostream& o, const FlowPixmap t);
  void zoom();
  void nav_zoom();
  void print_zoom();
  void traverse(int x, int y);
  int event_handler(void* pos, flow_eEvent event, int x, int y, void* node);
  void conpoint_select(void* pos, int x, int y, double* distance, void** cp){}
  void print(void* pos, void* node, int highlight);
  void save(std::ofstream& fp, flow_eSaveMode mode);
  void open(std::ifstream& fp);
  void draw(void* pos, int highlight, int dimmed, int hot, void* node);
  void nav_draw(void* pos, int highlight, void* node);
  void draw_inverse(void* pos, int hot, void* node);
  void erase(void* pos, int hot, void* node);
  void nav_erase(void* pos, void* node);
  void move(void* pos, double x, double y, int highlight, int dimmed, int hot);
  void shift(void* pos, double delta_x, double delta_y, int highlight,
      int dimmed, int hot);
  void get_borders(double pos_x, double pos_y, double* x_right, double* x_left,
      double* y_high, double* y_low, void* node);
  int get_conpoint(int num, double* x, double* y, flow_eDirection* dir)
  {
    return 0;
  }
  flow_eObjectType type()
  {
    return flow_eObjectType_Pixmap;
  }
  FlowCtx* ctx;
  FlowPoint p;
  void* pixmaps;
  flow_sPixmapData pixmap_data;
  int pixmap_size;
  flow_eDrawType draw_type;
};

#endif
