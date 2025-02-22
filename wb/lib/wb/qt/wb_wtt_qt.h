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

#ifndef wb_wtt_qt_h
#define wb_wtt_qt_h

/* wb_wtt_qt.h -- Simple navigator */

#include "cow_wow_qt.h"

#include "wb_wtt.h"

#include <QCheckBox>
#include <QLabel>
#include <QSplitter>

class WttQt;

class BootDialog : public QDialog {
  Q_OBJECT

public:
  BootDialog(WttQt* parent_ctx, QWidget* parent);

protected:
  void closeEvent(QCloseEvent* event);

private:
  WttQt* wtt;
  QTreeWidget* boot_list;

public slots:
  void boot_row_activated_cb(QTreeWidgetItem* item, int idx);
  void boot_ok_cb();
  void boot_cancel_cb();
};

class OptionsDialog : public QDialog {
  Q_OBJECT

public:
  OptionsDialog(WttQt* parent_ctx, QWidget* parent);

protected:
  void closeEvent(QCloseEvent* event);

private:
  WttQt* wtt;

public slots:
  void activate_options_ok();
  void activate_options_apply();
  void activate_options_cancel();
};

class WttQtWidget;

class WttQt : public Wtt {
public:
  WttQt(void* wt_parent_ctx, const char* wt_name, const char* iconname,
      ldh_tWBContext wt_wbctx, pwr_tVolumeId wt_volid, ldh_tVolume wt_volctx,
      wnav_sStartMenu* root_menu, pwr_tStatus* status);

  WttQt() {}
  ~WttQt();

  QWidget* wnav_brow_widget;
  QWidget* wnavnode_brow_widget;
  QSplitter* wnav_paned;
  QWidget* palette_widget;
  QLabel* msg_label;
  QLabel* cmd_prompt;
  QSplitter* palette_paned;
  QWidget* boot_dia;
  QAction* cm_normal_syntax;
  QAction* cm_gms_syntax;
  QAction* cm_extern_syntax;
  QAction* cm_add_attribute;
  QAction* cm_add_type;
  QAction* cm_add_volume;
  QWidget* options_form;
  QCheckBox* enable_comment_w;
  QCheckBox* show_plant_w;
  QCheckBox* show_node_w;
  QCheckBox* show_class_w;
  QCheckBox* show_alias_w;
  QCheckBox* show_descrip_w;
  QCheckBox* show_objref_w;
  QCheckBox* show_objxref_w;
  QCheckBox* show_attrref_w;
  QCheckBox* show_attrxref_w;
  QCheckBox* build_force_w;
  QCheckBox* build_debug_w;
  QCheckBox* build_crossref_w;
  QCheckBox* build_crossrefsim_w;
  QCheckBox* build_crossrefgraph_w;
  QCheckBox* build_manual_w;
  QCheckBox* build_nocopy_w;
  QCheckBox* enable_revisions_w;
  QAction* menu_save_w;
  QAction* menu_revert_w;
  QAction* menu_cut_w;
  QAction* menu_copy_w;
  QAction* menu_copykeep_w;
  QAction* menu_paste_w;
  QAction* menu_pasteinto_w;
  QAction* menu_rename_w;
  QAction* menu_utilities_w;
  QAction* menu_openplc_w;
  QAction* menu_buildobject_w;
  QAction* menu_buildvolume_w;
  QAction* menu_buildnode_w;
  QAction* menu_buildproject_w;
  QAction* menu_buildexport_w;
  QAction* menu_buildimport_w;
  QAction* menu_distribute_w;
  QAction* menu_change_value_w;
  QAction* menu_edit_w;
  QAction* menu_classeditor_w;
  QAction* menu_updateclasses_w;
  QAction* menu_clonevolume_w;
  QAction* menu_creaobjafter_w;
  QAction* menu_creaobjfirst_w;
  QAction* menu_deleteobj_w;
  QAction* menu_moveobjup_w;
  QAction* menu_moveobjdown_w;
  QAction* menu_moveobjinto_w;
  QAction* menu_moveobjontop_w;
  QAction* tools_edit_w;
  QAction* tools_buildnode_w;
  QAction* tools_save_w;
  QAction* view_alltoplevel_w;
  int set_focus_disabled;
  QTimer* disfocus_timerid;
  QTimer* selection_timerid;
  int avoid_deadlock;
  int popupmenu_x;
  int popupmenu_y;
  static CoWowRecall cmd_recall;
  static CoWowRecall value_recall;
  static CoWowRecall name_recall;
  CoWowEntryQt* cmd_entry;
  CoWowFocusTimerQt focustimer;
  int realized;

  int build_menu(int x, int y);
  QMenu* build_submenu(const char* Callback, ldh_sMenuItem* Items, int* idx);

  virtual void set_clock_cursor();
  virtual void reset_cursor();
  virtual void free_cursor();
  virtual void set_window_char(int width, int height);
  virtual void get_window_char(int* width, int* height);
  virtual void menu_setup();
  virtual void set_selection_owner();
  virtual void set_palette_selection_owner();
  virtual int create_popup_menu(pwr_sAttrRef attrref, int x, int y);
  virtual int create_pal_popup_menu(pwr_tCid cid, int x, int y);
  virtual void set_noedit_show();
  virtual void set_edit_show();
  virtual void set_twowindows(int two, int display_wnav, int display_wnavnode);
  virtual void message(char severity, const char* message);
  virtual void set_prompt(const char* prompt);
  virtual void open_change_value();
  virtual void close_change_value();
  virtual void open_change_name();
  virtual void watt_new(pwr_tAttrRef aref);
  virtual void wda_new(pwr_tOid oid);
  virtual Ge* ge_new(char* graphname);
  virtual void wcast_new(pwr_tAttrRef aref, pwr_tStatus* sts);
  virtual wb_build* build_new();
  virtual void wpkg_new();
  virtual int ute_new(char* title);
  virtual void open_input_dialog(const char* text, const char* title,
      const char* init_text, void (*ok_cb)(Wtt*, char*));
  virtual void open_confirm(const char* text, const char* title,
      void (*ok_cb)(Wtt*), void (*no_cb)(Wtt*));
  virtual void open_boot_window();
  virtual void update_options_form();
  virtual void set_options();
  virtual void pop();
  virtual void disable_focus();
  virtual void update_title();

  void create_options_dialog();

private:
  WttQtWidget* toplevel;
};

class WttQtWidget : public QWidget {
  Q_OBJECT

public:
  WttQtWidget(WttQt* parent_ctx) : QWidget(), wtt(parent_ctx) {}

protected:
  void focusInEvent(QFocusEvent* event);
  void closeEvent(QCloseEvent* event);

public slots:
  void set_selection_owner_proc();
  void set_palette_selection_owner_proc();

  void activate_change_value();
  void activate_command();
  void activate_print();
  void activate_collapse();
  void activate_save();
  void activate_revert();
  void activate_syntax();
  void activate_history();
  void activate_revisions();
  void activate_find();
  void activate_findregex();
  void activate_findnext();
  void activate_copy();
  void activate_cut();
  void activate_paste();
  void activate_pasteinto();
  void activate_copykeep();
  void activate_rename();
  void activate_creaobj();
  void activate_moveobj();
  void activate_deleteobj();
  void activate_configure();
  void activate_utilities();
  void activate_openobject();
  void activate_openvolobject();
  void activate_openplc();
  void activate_buildobject();
  void activate_openvolume();
  void activate_openbuffer();
  void activate_confproject();
  void activate_openfile_dbs();
  void activate_openfile_wbl();
  void activate_openpl();
  void activate_opengvl();
  void activate_openudb();
  void activate_spreadsheet();
  void activate_openge();
  void activate_openclasseditor();
  void activate_buildvolume();
  void activate_buildnode();
  void activate_builddirectories();
  void activate_buildimport();
  void activate_buildexport();
  void activate_distribute();
  void activate_showcrossref();
  void activate_updateclasses();
  void activate_clonevolume();
  void activate_backupfile();
  void activate_zoom_in();
  void activate_zoom_out();
  void activate_zoom_reset();
  void activate_twowindows();
  void activate_set_alltoplevel();
  void activate_messages();
  void activate_view();
  void activate_savesettings();
  void activate_restoresettings();
  void activate_scriptproj();
  void activate_scriptbase();
  void activate_set_advuser();
  void activate_rtmon();
  void activate_statusmon();
  void activate_help();
  void activate_help_project();
  void activate_help_proview();
  void valchanged_cmd_entry();
  void activate_selmode(bool set = false);

  void hide_cmd_entry();
  void popup_button_cb();

private:
  WttQt* wtt;
};

#endif
