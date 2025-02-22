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

#ifndef xtt_trace_h
#define xtt_trace_h

#include "flow_api.h"
#include "flow_ctx.h"

class RtTrace;
class CoWow;
class CoWowTimer;

typedef struct trace_sNode trace_tNode;

struct trace_sNode {
  trace_tNode* Next;
  RtTrace* tractx;
};

class RtTrace {
public:
  void* parent_ctx;
  pwr_tOName name;
  flow_tNodeClass trace_analyse_nc;
  flow_tConClass trace_con_cc;
  int trace_started;
  CoWowTimer* trace_timerid;
  flow_tCtx flow_ctx;
  flow_tNode trace_changenode;
  pwr_tObjid objid;
  double scan_time;
  void (*close_cb)(RtTrace*);
  void (*help_cb)(RtTrace*, const char*, const char*);
  void (*subwindow_cb)(void* parent_ctx, pwr_tObjid objid);
  void (*display_object_cb)(void* parent_ctx, pwr_tObjid objid);
  void (*collect_insert_cb)(void* parent_ctx, pwr_tObjid objid);
  int (*is_authorized_cb)(void* parent_ctx, unsigned int access);
  void (*popup_menu_cb)(void* parent_ctx, pwr_sAttrRef attrref,
      unsigned long item_type, unsigned long utility, char* arg, int x, int y);
  int (*call_method_cb)(void* parent_ctx, const char* method,
      const char* filter, pwr_sAttrRef attrref, unsigned long item_type,
      unsigned long utility, char* arg);
  trace_tNode* trace_list;
  pwr_tFileName filename;
  int version;
  int m_has_host;
  pwr_tAName m_hostname;
  pwr_tAName m_plcconnect;
  CoWow* wow;

  RtTrace(void* parent_ctx, pwr_tObjid objid, pwr_tStatus* status);
  virtual ~RtTrace();

  virtual void pop()
  {
  }
  virtual RtTrace* subwindow_new(void* ctx, pwr_tObjid oid, pwr_tStatus* sts)
  {
    return 0;
  }
  virtual void popup_menu_position(int event_x, int event_y, int* x, int* y)
  {
  }
  int search_object(char* object_str);
  void swap(int mode);

  void activate_close();
  void activate_print();
  void activate_printselect();
  void activate_savetrace();
  void activate_restoretrace();
  void activate_cleartrace();
  void activate_parent_window();
  void activate_display_object();
  void activate_collect_insert();
  void activate_open_object();
  void activate_open_subwindow();
  void activate_show_cross();
  void activate_open_classgraph();
  void activate_trace();
  void activate_simulate();
  void activate_view();
  void activate_help();
  void activate_helpplc();
  void activate_helpplclist();

protected:
  FlowTraceAttr get_trace_attr(flow_tObject object, unsigned int* options);
  int get_objid(flow_tObject node, pwr_tObjid* objid);
  int get_attrref(flow_tObject node, pwr_tAttrRef* aref);
  int get_selected_node(flow_tObject* node);
  pwr_tStatus viewsetup();
  pwr_tStatus simsetup();
  pwr_tStatus trasetup();
  int trace_start();
  int trace_stop();
  void changevalue(flow_tNode fnode);

  static void get_save_filename(pwr_tObjid window_objid, char* filename);
  static int get_filename(pwr_tObjid window_objid, char* filename,
      int* has_host, char* hostname, char* plcconnect);
  static int connect_bc(flow_tObject object, char* name, char* attr,
      flow_eTraceType type, void** p);
  static int disconnect_bc(flow_tObject object);
  static int scan_bc(flow_tObject object, void* p);
  static void trace_scan(void* data);
  static int init_flow(FlowCtx* ctx, void* client_data);
  static int flow_cb(FlowCtx* ctx, flow_tEvent event);
  static void trace_close_cb(RtTrace* child_tractx);
  static char* id_to_str(pwr_tObjid objid);
};

#endif
