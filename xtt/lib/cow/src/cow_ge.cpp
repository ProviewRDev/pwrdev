/*
 * ProviewR   Open Source Process Control.
 * Copyright (C) 2005-2023 SSAB EMEA AB.
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

#include "co_cdh.h"
#include "co_string.h"
#include "co_cnf.h"
#include "co_msg.h"
#include "co_dcli.h"

#include "rt_gdh_msg.h"

#include "ge_graph.h"

#include "cow_ge.h"
#include "cow_wow.h"
#include "cow_xhelp.h"

void CowGe::eventlog_enable(int enable)
{
  Graph::eventlog_enable(enable);
}

void CowGe::graph_init_cb(void* client_data)
{
  CowGe* ge = (CowGe*)client_data;
  char fname[120];
  int default_width;
  int default_height;
  int sts;
  int path_cnt = 2;
  char path[10][80] = {"$pwrp_exe/", "$pwr_exe/",};

  ge->graph->set_subgraph_path(path_cnt, (char *)path);
  strncpy(fname, ge->filename, sizeof(fname));
  if (fname[0] == '@') {
    ge->graph->read_scriptfile(&fname[1]);
    ge->graph->set_modified(0);
  }
  else {
    if (!strrchr(fname, '.')) {
      if (ge->graph->is_dashboard())
	strcat(fname, ".pwd");
      else
	strcat(fname, ".pwg");
    }
    sts = ge->graph->open(fname);
    if (EVEN(sts) && ge->graph->is_dashboard())
      ge->graph->set_name(ge->filename);
  }
  if (ge->width == 0 || ge->height == 0) {
    sts = ge->graph->get_default_size(&default_width, &default_height);
    if (ODD(sts)) {
      ge->set_size(default_width, default_height);
    }
  }
  ge->graph->set_default_layout();

  ge->graph->init_trace();
}

int CowGe::graph_close_cb(void* client_data)
{
  CowGe* ge = (CowGe*)client_data;

  delete ge;
  return 1;
}

int CowGe::ge_command_cb(void* ge_ctx, char* cmd, char* script, char* scriptargs)
{
  CowGe* ge = (CowGe*)ge_ctx;
  int sts;

  if (ge->command_cb) {
    sts = (ge->command_cb)(ge->parent_ctx, cmd, script, scriptargs, ge_ctx);
    return sts;
  }
  return 0;
}

int CowGe::ge_sound_cb(void* ge_ctx, pwr_tAttrRef* aref)
{
  CowGe* ge = (CowGe*)ge_ctx;

  if (ge->sound_cb)
    return (ge->sound_cb)(ge->parent_ctx, aref);

  return 0;
}

void CowGe::ge_display_in_xnav_cb(void* ge_ctx, pwr_sAttrRef* arp)
{
  CowGe* ge = (CowGe*)ge_ctx;

  if (ge->display_in_xnav_cb)
    (ge->display_in_xnav_cb)(ge->parent_ctx, arp);
}

void CowGe::ge_popup_menu_cb(void* ge_ctx, pwr_sAttrRef attrref,
    unsigned long item_type, unsigned long utility, char* arg, int x, int y)
{
  CowGe* ge = (CowGe*)ge_ctx;

  if (ge->popup_menu_cb)
    (ge->popup_menu_cb)(ge->parent_ctx, attrref, item_type, utility, arg, x, y);
}

int CowGe::ge_call_method_cb(void* ge_ctx, char* method, char* filter,
    pwr_sAttrRef attrref, unsigned long item_type, unsigned long utility,
    char* arg)
{
  CowGe* ge = (CowGe*)ge_ctx;

  if (ge->call_method_cb)
    return (ge->call_method_cb)(
        ge->parent_ctx, method, filter, attrref, item_type, utility, arg);
  else
    return 0;
}

int CowGe::ge_is_authorized_cb(void* ge_ctx, unsigned int access)
{
  CowGe* ge = (CowGe*)ge_ctx;

  if (ge->is_authorized_cb)
    return (ge->is_authorized_cb)(ge->parent_ctx, access);
  return 0;
}

int CowGe::ge_get_current_objects_cb(
    void* ge_ctx, pwr_sAttrRef** alist, int** is_alist)
{
  CowGe* ge = (CowGe*)ge_ctx;

  if (ge->get_current_objects_cb)
    return (ge->get_current_objects_cb)(ge->parent_ctx, alist, is_alist);
  return 0;
}

void CowGe::ge_eventlog_cb(void* ge_ctx, void* data, unsigned int size)
{
}

void CowGe::ge_keyboard_cb(void* ge_ctx, int action, int type)
{
  CowGe* ge = (CowGe*)ge_ctx;

  if (ge->keyboard_cb)
    (ge->keyboard_cb)(ge->parent_ctx, ge, action, type);
}

void CowGe::ge_resize_cb(void* ge_ctx, int width, int height)
{
  CowGe* ge = (CowGe*)ge_ctx;

  //ge->resize(width, height);
  ge->set_size(width, height);
}

int CowGe::ge_get_rtplant_select_cb(
      void* ge_ctx, char* attr_name, int size, pwr_tTypeId *type)
{
  CowGe* ge = (CowGe*)ge_ctx;

  if (!ge->get_select_cb)
    return 0;
  return (ge->get_select_cb)(ge->parent_ctx, attr_name, type);
}

int CowGe::ge_extern_connect_cb(
      void* ge_ctx, char* name, void** p, pwr_tRefId *id)
{
  CowGe* ge = (CowGe*)ge_ctx;

  if (!ge->extern_connect_cb)
    return 0;
  return (ge->extern_connect_cb)(ge->parent_ctx, name, p, id);
}


void CowGe::message_cb(void* ctx, char severity, const char* msg)
{
  ((CowGe*)ctx)->message(severity, msg);
}

void CowGe::message(char severity, const char* msg)
{
  if (!streq(msg, "")) {
    if (wow)
      wow->DisplayError("Ge Message", msg, lng_eCoding_ISO8859_1, 0);
    else
      printf("** CowGe: %s\n", msg);
  }
}

void CowGe::get_current_colors_cb(void* ctx, glow_eDrawType* fill_color,
      glow_eDrawType* border_color, glow_eDrawType* text_color)
{
  *fill_color = ((CowGe *)ctx)->default_fill_color;
  *border_color = ((CowGe *)ctx)->default_border_color;
  *text_color = ((CowGe *)ctx)->default_text_color;
}

void CowGe::set_current_colors_cb(void* ctx, glow_eDrawType fill_color,
      glow_eDrawType border_color, glow_eDrawType text_color)
{
  ((CowGe *)ctx)->default_fill_color = fill_color;
  ((CowGe *)ctx)->default_border_color = border_color;
  ((CowGe *)ctx)->default_text_color = text_color;
}

int CowGe::set_object_focus(const char* name, int empty)
{
  return graph->set_object_focus(name, empty);
}

int CowGe::set_folder_index(const char* name, int idx)
{
  return graph->set_folder_index(name, idx);
}

int CowGe::set_subwindow_source(const char* name, char* source, char* object)
{
  return graph->set_subwindow_source(name, source, object);
}

void CowGe::disable_subwindow_events(int disable)
{
  return graph->disable_subwindow_events(disable);
}

CowGe::~CowGe()
{
}

void CowGe::print()
{
  pwr_tFileName filename;
  pwr_tCmd cmd;

  dcli_translate_filename(filename, "$pwrp_tmp/graph.ps");
  graph->print(filename);

  sprintf(cmd, "$pwr_exe/rt_print.sh %s 1", filename);
  system(cmd);
}

void CowGe::export_image(char* filename)
{
  pwr_tFileName fname;

  dcli_translate_filename(fname, filename);
  graph->export_image(fname);
}

void CowGe::swap(int mode)
{
  graph->swap(mode);
}

void CowGe::update_color_theme(int theme)
{
  color_theme = theme;
  graph->update_color_theme(color_theme);
}

void CowGe::signal_send(char* signalname)
{
  graph->signal_send(signalname);
}

void CowGe::event_exec(int type, void* event, unsigned int size)
{
}

void CowGe::set_text_coding(lng_eCoding coding)
{
  graph->set_text_coding(coding);
}

int CowGe::key_pressed(int key)
{
  return graph->key_pressed(key);
}

void CowGe::close_input_all()
{
  graph->close_input_all();
}

int CowGe::get_object_name(unsigned int idx, int size, char* name)
{
  return graph->get_object_name(idx, size, name);
}

int CowGe::in_edit_mode() 
{
  return (graph->mode == graph_eMode_Development);
}

void CowGe::activate_edit(int edit)
{
  if (edit) {
    graph->close_trace(0);
    graph->mode = graph_eMode_Development;
    graph->grow->grow_setup();
  }
  else {
    graph->select_clear();
    graph->mode = graph_eMode_Runtime;
    graph->init_trace();
  }
}

void CowGe::activate_open()
{
  pwr_tCmd cmd = "open dash";
  if (command_cb)
    (command_cb)(parent_ctx, cmd, 0, 0, this);
}

void CowGe::activate_add()
{
  grow_tObject o;
  int sts;

  if (graph->mode == graph_eMode_Runtime)
    return;

  sts = graph->create_dashcell_next(&o, 1, 1, 0, 0);
  if (EVEN(sts)) {
    char msg[100];
    msg_GetText(sts, msg, sizeof(msg));
    message('E', msg);
  }
}

void CowGe::activate_delete()
{
  if (graph->mode == graph_eMode_Runtime)
    return;

  graph->delete_select();
}

void CowGe::activate_copy()
{
  if (graph->mode == graph_eMode_Runtime)
    return;

  graph->copy();
}

void CowGe::activate_paste()
{
  if (graph->mode == graph_eMode_Runtime)
    return;

  if (graph->is_dashboard() && graph->dashboard_is_full()) {
    message('E', "Dashboard is full");
    return;
  }

  graph->paste();
}

void CowGe::activate_connect()
{
  int sts;
  pwr_tAName aname;
  pwr_tTypeId type;
  grow_tObject gobject;

  if (graph->mode == graph_eMode_Runtime)
    return;

  sts = (get_select_cb)(parent_ctx, aname, &type);
  if (EVEN(sts)) {
    message('E', "Select an object in the navigator");
    return;
  }

  sts = graph->get_selected_object(&gobject);
  if (EVEN(sts)) {
    message('E', "Select one object in the dashboard");
    return;
  }

  sts = graph->dashboard_connect(gobject, 0, aname, type);
  printf("aname: %s\n", aname);
}

void CowGe::activate_merge()
{
  int sts;

  if (graph->mode == graph_eMode_Runtime)
    return;

  sts = graph->merge_dashcells();
  if (EVEN(sts)) {
    char msg[100];
    msg_GetText(sts, msg, sizeof(msg));
    message('E', msg);
  }
}

void CowGe::activate_clear()
{
  if (graph->mode == graph_eMode_Runtime)
    graph->close_trace(0);

  graph->delete_all();

  if (graph->mode == graph_eMode_Runtime)
    graph->init_trace();
}

int CowGe::dash_insert(char *name, pwr_tTypeId type)
{
  grow_tObject o;

  return graph->create_dashcell_next(&o, 1, 1, name, type);
}

void CowGe::activate_cellattributes()
{
  grow_tObject gobject;
  int sts;

  sts = graph->get_selected_object(&gobject);
  if (EVEN(sts)) {
    message('E', "Select one object in the dashboard");
    return;
  }

  graph->edit_attributes(gobject);
}

void CowGe::activate_graphattributes()
{
  graph->edit_graph_attributes();
}

void CowGe::activate_help()
{
  char key[80];

  if (help_cb) {
    if (graph->is_dashboard()) {
      strcpy(key, "opg_dashboard");
      CoXHelp::dhelp("opg_dashboard", "", navh_eHelpFile_Base, 0, 0);
    }
    else {
      str_ToLower(key, name);
      (help_cb)(parent_ctx, key);
    }
  }
}

void CowGe::activate_save()
{
  char name[40];

  graph->get_name(name);
  if (streq(name, ""))
    return;


  if (graph->mode == graph_eMode_Runtime)
    graph->close_trace(0);

  graph->save(name);

  if (graph->mode == graph_eMode_Runtime)
    graph->init_trace();
}

void CowGe::file_selected_cb(void* ctx, void* data, char* text)
{
  CowGe* ge = (CowGe*)ctx;
  ge->graph->set_name(text);
  ge->set_title(text);
  ge->activate_save();
  if (ge->namechanged_cb)
    (ge->namechanged_cb)(ge->parent_ctx, ge, text);
}

void CowGe::activate_saveas()
{
  wow->CreateInputDialog(
      this, "Save as", "Enter filename", file_selected_cb, 0, 40, 0, 0);
}

typedef struct {
  pwr_tString32 name;
  int idx;
} tThemes;

static tThemes themes[] = { { "Standard", 0 }, { "Sand", 1 }, { "Maroon", 2 },
  { "Sienna", 3 }, { "DarkBlue", 4 }, { "Classic", 5 }, { "Midnight", 6 },
  { "PlayRoom", 7 }, { "NordicLight", 8 }, { "Contrast", 9 },
  { "AzureContrast", 10 }, { "OchreContrast", 11 }, { "Chesterfield", 12 },
  { "TerraVerte", 13 }, { "Polar", 14 }, { "Custom", 100 } };

void CowGe::ge_colortheme_selector_ok_cb(void* ctx, char* text, int ok_pressed)
{
  CowGe* gectx = (CowGe*)ctx;
  int idx = -1;

  for (unsigned int i = 0; i < sizeof(themes) / sizeof(themes[0]); i++) {
    if (streq(text, themes[i].name)) {
      idx = themes[i].idx;
      break;
    }
  }

  if (idx >= 0)
    gectx->graph->update_color_theme(idx);
}

void CowGe::activate_setcolortheme()
{
  pwr_tString80 names[30];

  memset(names, 0, sizeof(names));
  for (unsigned int i = 0; i < sizeof(themes) / sizeof(themes[0]); i++) {
    strcpy(names[i], themes[i].name);
  }

  wow->CreateList("ColorTheme Selector", (char*)names, sizeof(names[0]),
      ge_colortheme_selector_ok_cb, 0, this);
}

static void exit_ok(void *ctx, void *data)
{
  CowGe *ge = (CowGe *)ctx;
  delete ge;
}

void CowGe::activate_exit_modified()
{
  wow->DisplayQuestion(this, "Save", "Dashboard is not saved\nDo you want to close?",
      exit_ok, 0, 0);
}

CowGe::CowGe(void* xg_parent_ctx, const char* xg_name, const char* xg_filename,
    int xg_scrollbar, int xg_menu, int xg_navigator, int xg_width,
    int xg_height, int x, int y, double scan_time, const char* object_name,
    int use_default_access, unsigned int access, unsigned int xg_options,
    int xg_color_theme, int (*xg_command_cb)(void*, char*, char*, char *, void*),
    int (*xg_get_current_objects_cb)(void*, pwr_sAttrRef**, int**),
    int (*xg_is_authorized_cb)(void*, unsigned int),
    void (*xg_keyboard_cb)(void*, void*, int, int),
    int (*xg_extern_connect_cb)(void*, char*, void**, pwr_tRefId*))
    : parent_ctx(xg_parent_ctx), scrollbar(xg_scrollbar),
      navigator(xg_navigator), menu(xg_menu), current_value_object(0),
      current_confirm_object(0), value_input_open(0), confirm_open(0),
      command_cb(xg_command_cb), close_cb(0), help_cb(0), display_in_xnav_cb(0),
      is_authorized_cb(xg_is_authorized_cb), popup_menu_cb(0),
      call_method_cb(0), get_current_objects_cb(xg_get_current_objects_cb),
      sound_cb(0), eventlog_cb(0), keyboard_cb(xg_keyboard_cb), namechanged_cb(0), 
      get_select_cb(0), extern_connect_cb(xg_extern_connect_cb),
      width(xg_width), height(xg_height), options(xg_options), 
      color_theme(xg_color_theme), default_fill_color(glow_eDrawType_Line),
      default_border_color(glow_eDrawType_Line), default_text_color(glow_eDrawType_Line),
      resize_restrictions_set(0), window_resize_delta(0.01), wow(0)
{
  char value_str[40];

  strcpy(filename, xg_filename);
  strcpy(name, xg_name);
  if (cnf_get_value("graphWindowResizeDelta", value_str, sizeof(value_str))) {
    float value;
    int nr = sscanf(value_str, "%f", &value);
    if (nr == 1)
      window_resize_delta = value;
  }
}