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

#ifndef cow_wow_gtk_h
#define cow_wow_gtk_h

/* cow_wow_gtk.h -- useful windows */

#include "cow_wow.h"

#include "cow_gtk.h"

typedef struct {
  void* ctx;
  void* data;
  void (*questionbox_ok)(void*, void*);
  void (*questionbox_cancel)(void*, void*);
  void (*questionbox_help)(void*, void*);
  GtkWidget* question_widget;
} wow_t_question_cb;

typedef struct {
  void* ctx;
  void* data;
  void (*inputdialogbox_ok)(void*, void*, char*);
  void (*inputdialogbox_cancel)(void*, void*);
  void (*inputdialogbox_help)(void*, void*);
  GtkWidget* inputdialog_widget;
  GtkWidget* input_widget;
} wow_t_inputdialog_cb;

class CoWowEntryGtk {
public:
  CoWowRecall* m_re;
  GtkWidget* w;
  bool m_re_alloc;
  bool m_hide_on_esc;

  CoWowEntryGtk(CoWowRecall* re = 0);
  ~CoWowEntryGtk();
  GtkWidget* widget()
  {
    return w;
  }
  void set_recall_buffer(CoWowRecall* re)
  {
    if (!m_re_alloc)
      m_re = re;
  }
  void set_hide_on_esc(bool hide)
  {
    m_hide_on_esc = hide;
  }
  static gboolean event_cb(GtkWidget* w, GdkEvent* event, gpointer data);
};

class CoWowFocusTimerGtk {
public:
  CoWowFocusTimerGtk() : set_focus_disabled(0), request_cnt(0), focus_timerid(0)
  {
  }
  ~CoWowFocusTimerGtk();
  void disable(int time);
  int disabled();

private:
  int set_focus_disabled;
  int request_cnt;
  gint focus_timerid;

  static gboolean enable_set_focus(void* ft);
};

class CoWowTimerGtk : public CoWowTimer {
public:
  gint m_timerid;

  CoWowTimerGtk() : m_timerid(0)
  {
  }
  ~CoWowTimerGtk();
  void add(int time, void (*callback)(void* data), void* data);
  void remove();
  static gboolean timer_cb(void* data);
};

class CoWowGtk : public CoWow {
public:
  GtkWidget* m_parent;
  gint m_wait_timerid;

  CoWowGtk(GtkWidget* parent) : m_parent(parent), m_wait_timerid(0)
  {
  }
  ~CoWowGtk();
  void DisplayQuestion(void* ctx, const char* title, const char* text,
      void (*questionbox_ok)(void*, void*),
      void (*questionbox_cancel)(void*, void*), void* data);
  void DisplayError(const char* title, const char* text,
      lng_eCoding coding = lng_eCoding_ISO8859_1, int modal = 1);
  void DisplayText(
      const char* title, const char* text, int width = 0, int height = 0,
      wow_eImage image = wow_eImage_Info);
  void CreateInputDialog(void* ctx, const char* title, const char* text,
      void (*inputdialogbox_ok)(void*, void*, char*),
      void (*inputdialogbox_cancel)(void*, void*), int input_length,
      char* init_text, void* data);
  void* CreateList(const char* title, const char* texts, int textsize,
      void(action_cb)(void*, char*, int), void(cancel_cb)(void*), void* ctx,
      int show_apply_button = 0);
  void PopList(void* ctx);
  void DeleteList(void* ctx);
  void CreateFileSelDia(const char* title, void* parent_ctx,
      void (*file_selected_cb)(void*, char*, wow_eFileSelType),
      wow_eFileSelType file_type, wow_eFileSelAction action);
  int CreateModalDialog(const char* title, const char* text,
      const char* button1, const char* button2, const char* button3,
      const char* image);
  wow_sModalInputDialog* CreateModalInputDialog(const char* title,
      const char* text, const char* button1, const char* button2,
      const char* button3, const char* image, int input_length,
      CoWowRecall* recall = 0);
  int DisplayWarranty();
  void DisplayLicense();
  pwr_tStatus CreateMenuItem(
      const char* name, void* menu, int pixmap, int append, void* w);
  pwr_tStatus DeleteMenuItem(const char* name, void* menu);
  void CreateBrowPrintDialog(const char* title, void* brow_ctx, int orientation,
      double scale, void* parent_widget, pwr_tStatus* sts)
  {
    CreateBrowPrintDialogGtk(
        title, brow_ctx, orientation, scale, parent_widget, sts);
  }
  static void CreateBrowPrintDialogGtk(const char* title, void* brow_ctx,
      int orientation, double scale, void* parent_widget, pwr_tStatus* sts);
  void CreateFlowPrintDialog(const char* title, void* flow_ctx, int orientation,
      double scale, void* parent_widget, pwr_tStatus* sts)
  {
    CreateFlowPrintDialogGtk(
        title, flow_ctx, orientation, scale, parent_widget, sts);
  }
  static void CreateFlowPrintDialogGtk(const char* title, void* flow_ctx,
      int orientation, double scale, void* parent_widget, pwr_tStatus* sts);
  CoWowTimer* timer_new();
  void SetParent(GtkWidget* parent)
  {
    m_parent = parent;
  }
  void Wait(float time);

  static void PopupPosition(
      GtkWidget* parent, int x_event, int y_event, int* x, int* y);
  static void GetAtoms(
      GdkAtom* graph_atom, GdkAtom* objid_atom, GdkAtom* attrref_atom);
  static int GetSelection(GtkWidget* w, char* str, int size, GdkAtom atom);
  static void SetWindowIcon(GtkWidget* w);
  static int GetDarkTheme(GtkWidget *w);
  static void question_ok_cb(GtkWidget* w, gpointer data);
  static void question_cancel_cb(GtkWidget* w, gpointer data);
  static void inputdialog_ok_cb(GtkWidget* w, gpointer data);
  static void inputdialog_cancel_cb(GtkWidget* w, gpointer data);
  static void warranty_cb(GtkWidget* w, gint response, gpointer data);
  static void license_cb(GtkWidget* w, gint response, gpointer data);
  static char* translate_utf8(const char* str);
  static char* convert_utf8(const char* str);

  static void list_row_activated_cb(GtkTreeView* tree_view, GtkTreePath* path,
      GtkTreeViewColumn* column, gpointer data);
  static void list_apply_cb(GtkWidget* w, gpointer data);
  static void list_ok_cb(GtkWidget* w, gpointer data);
  static void list_cancel_cb(GtkWidget* w, gpointer data);
  static gboolean wait_cb(void* data);
  static void update_title(GtkWidget* w, int editmode);
};

#endif
