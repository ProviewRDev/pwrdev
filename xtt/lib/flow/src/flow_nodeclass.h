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

#ifndef flow_nodeclass_h
#define flow_nodeclass_h

#include "flow_rect.h"

class FlowNodeClass : public FlowArrayElem {
public:
  FlowNodeClass(FlowCtx* flow_ctx, const char* name,
      flow_eNodeGroup grp = flow_eNodeGroup_Common);
  void insert(FlowArrayElem* element)
  {
    a.insert(element);
  }
  void zoom()
  {
    a.zoom();
  }
  void nav_zoom()
  {
    a.nav_zoom();
  }
  void print_zoom()
  {
    a.print_zoom();
  }
  void traverse(int x, int y)
  {
    a.traverse(x, y);
  }
  void get_borders(double pos_x, double pos_y, double* x_right, double* x_left,
      double* y_high, double* y_low, void* node)
  {
    a.get_borders(pos_x, pos_y, x_right, x_left, y_high, y_low, node);
  }
  void get_obstacle_borders(double pos_x, double pos_y, double* x_right,
      double* x_left, double* y_high, double* y_low, void* node);
  int event_handler(void* pos, flow_eEvent event, int x, int y, void* node);
  void print(FlowPoint* pos, void* node, int highlight);
  void save(std::ofstream& fp, flow_eSaveMode mode);
  void open(std::ifstream& fp);
  int load(char* filename);
  void draw(FlowPoint* pos, int highlight, int dimmed, int hot, void* node);
  void nav_draw(FlowPoint* pos, int highlight, void* node);
  void draw_inverse(FlowPoint* pos, int hot, void* node);
  void erase(FlowPoint* pos, int hot, void* node);
  void nav_erase(FlowPoint* pos, void* node);
  int get_conpoint(int num, double* x, double* y, flow_eDirection* dir);
  flow_eObjectType type()
  {
    return flow_eObjectType_NodeClass;
  }
  void erase_annotation(void* pos, int highlight, int hot, void* node, int num);
  void draw_annotation(void* pos, int highlight, int hot, void* node, int num);
  void configure_annotations(void* pos, void* node);
  void measure_annotation(int num, char* text, double* width, double* height);
  void get_object_name(char* name);
  void open_annotation_input(void* pos, void* node, int num);
  void close_annotation_input(void* node, int num);
  int get_annotation_input(void* node, int num, char** text);
  void move_widgets(void* node, int x, int y);
  FlowTraceAttr get_conpoint_trace_attr(int num);
  void set_no_con_obstacle(int no_obstacle)
  {
    no_con_obstacle = no_obstacle;
  }
  int get_next_conpoint(
      int cp_num, flow_eDirection dir, double x0, double y0, int* next_cp_num);

  FlowCtx* ctx;
  FlowArray a;
  flow_tName nc_name;
  flow_eNodeGroup group;
  int no_con_obstacle;
};

#endif
