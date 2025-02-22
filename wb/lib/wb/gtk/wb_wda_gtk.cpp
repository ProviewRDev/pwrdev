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

/* wb_wda_gtk.cpp -- spreadsheet editor */

#include <stdlib.h>

#include "co_string.h"
#include "co_time.h"

#include "cow_xhelp.h"

#include "wb_wda_msg.h"
#include "wb_wda_gtk.h"
#include "wb_wdanav_gtk.h"
#include "wb_wtt.h"

CoWowRecall WdaGtk::value_recall;

void WdaGtk::message(char severity, const char* message)
{
  gtk_label_set_text(GTK_LABEL(msg_label), message);
}

void WdaGtk::set_prompt(const char* prompt)
{
  if (streq(prompt, "")) {
    g_object_set(cmd_prompt, "visible", FALSE, NULL);
    g_object_set(msg_label, "visible", TRUE, NULL);
  } else {
    g_object_set(msg_label, "visible", FALSE, NULL);
    g_object_set(cmd_prompt, "visible", TRUE, NULL);
  }
}

void WdaGtk::print(const char* title)
{
  pwr_tStatus sts;

  wow->CreateBrowPrintDialog(title, wdanav->brow->ctx,
      flow_eOrientation_Portrait, 1.0, (void*)toplevel, &sts);
}

void WdaGtk::change_value(int set_focus)
{
  int sts;
  GtkWidget* text_w;
  int multiline;
  char* value;
  int input_size;

  if (input_open) {
    g_object_set(cmd_input, "visible", FALSE, NULL);
    g_object_set(cmd_scrolledinput, "visible", FALSE, NULL);
    set_prompt("");
    input_open = 0;
    return;
  }

  sts = wdanav->check_attr(
      &multiline, &input_node, input_name, &value, &input_size);
  if (EVEN(sts)) {
    if (sts == WDA__NOATTRSEL)
      message('E', "No attribute is selected");
    else
      message('E', wnav_get_message(sts));
    return;
  }

  if (multiline) {
    text_w = cmd_scrolledinput;
    g_object_set(text_w, "visible", TRUE, NULL);

    int h = gdk_window_get_height(gtk_widget_get_window(pane));
    gtk_paned_set_position(GTK_PANED(pane), h - 170);
    gtk_widget_grab_focus(cmd_scrolledtextview);
    input_multiline = 1;
  } else {
    text_w = cmd_input;
    g_object_set(text_w, "visible", TRUE, NULL);
    gtk_widget_grab_focus(cmd_input);
    input_multiline = 0;
  }

  message(' ', "");

  if (value) {
    char* valueutf8
        = g_convert(value, -1, "UTF-8", "ISO8859-1", NULL, NULL, NULL);

    if (multiline) {
      GtkTextIter start_iter, end_iter;
      gtk_text_buffer_get_start_iter(cmd_scrolled_buffer, &start_iter);
      gtk_text_buffer_get_end_iter(cmd_scrolled_buffer, &end_iter);
      gtk_text_buffer_delete(cmd_scrolled_buffer, &start_iter, &end_iter);

      gtk_text_buffer_get_start_iter(cmd_scrolled_buffer, &start_iter);
      gtk_text_buffer_insert(cmd_scrolled_buffer, &start_iter, valueutf8, -1);

      // Select the text
      // gtk_text_buffer_get_start_iter( cmd_scrolled_buffer, &start_iter);
      // gtk_text_buffer_get_end_iter( cmd_scrolled_buffer, &end_iter);
      // gtk_text_buffer_select_range( cmd_scrolled_buffer, &start_iter,
      // &end_iter);
    } else {
      gint pos = 0;
      gtk_editable_delete_text(GTK_EDITABLE(cmd_input), 0, -1);
      gtk_editable_insert_text(
          GTK_EDITABLE(text_w), valueutf8, strlen(valueutf8), &pos);

      // Select the text
      gtk_editable_set_position(GTK_EDITABLE(cmd_input), -1);
      gtk_editable_select_region(GTK_EDITABLE(cmd_input), 0, -1);
    }
    g_free(valueutf8);
  } else {
    gtk_editable_delete_text(GTK_EDITABLE(cmd_input), 0, -1);
  }
  set_prompt("value >");
  input_open = 1;
}

//
//  Callbackfunctions from menu entries
//
void WdaGtk::activate_change_value(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;

  wda->change_value(1);
}

void WdaGtk::activate_close_changeval(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;

  wda->change_value_close();
}

void WdaGtk::activate_exit(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;

  if (wda->close_cb)
    (wda->close_cb)(wda);
  else
    delete wda;
}

void WdaGtk::activate_print(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;

  wda->activate_print();
}

void WdaGtk::activate_export_text(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;

  wda->print_textfile();
}

void WdaGtk::activate_import_text(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;

  wda->import_textfile();
}

void WdaGtk::activate_setclass(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;

  wda->open_class_dialog();
}

void WdaGtk::activate_setattr(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;

  wda->open_attr_dialog();
}

void WdaGtk::activate_nextattr(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;
  int sts;

  sts = wda->next_attr();
  if (EVEN(sts))
    wda->wow->DisplayError("Spreadsheet error", wnav_get_message(sts));
}

void WdaGtk::activate_prevattr(GtkWidget* w, gpointer data)
{
  Wda* wda = (Wda*)data;
  int sts;

  sts = wda->prev_attr();
  if (EVEN(sts))
    wda->wow->DisplayError("Spreadsheet error", wnav_get_message(sts));
}

void WdaGtk::activate_help(GtkWidget* w, gpointer data)
{
  CoXHelp::dhelp("spreadsheeteditor_refman", 0, navh_eHelpFile_Other,
      "$pwr_lang/man_dg.dat", true);
}

void WdaGtk::class_activate_ok(GtkWidget* w, gpointer data)
{
  WdaGtk* wda = (WdaGtk*)data;
  char* hiername;
  char* classname;
  char* searchname;
  char* utf8;
  int sts;
  pwr_tClassId new_classid;

  utf8 = gtk_editable_get_chars(GTK_EDITABLE(wda->wdaclass_hiervalue), 0, -1);
  hiername = g_convert(utf8, -1, "ISO8859-1", "UTF-8", NULL, NULL, NULL);
  g_free(utf8);
  utf8 = gtk_editable_get_chars(GTK_EDITABLE(wda->wdaclass_namevalue), 0, -1);
  searchname = g_convert(utf8, -1, "ISO8859-1", "UTF-8", NULL, NULL, NULL);
  g_free(utf8);
  utf8 = gtk_editable_get_chars(GTK_EDITABLE(wda->wdaclass_classvalue), 0, -1);
  classname = g_convert(utf8, -1, "ISO8859-1", "UTF-8", NULL, NULL, NULL);
  g_free(utf8);

  wda->attrobjects = (int)gtk_toggle_button_get_active(
      GTK_TOGGLE_BUTTON(wda->wdaclass_attrobjects));

  if (streq(hiername, ""))
    wda->objid = pwr_cNObjid;
  else {
    sts = ldh_NameToObjid(wda->ldhses, &wda->objid, hiername);
    if (EVEN(sts)) {
      CoWowGtk ww(wda->wdaclass_dia);
      ww.DisplayError("Hierarchy object error", wnav_get_message(sts));
      return;
    }
  }
  strncpy(wda->search_name, searchname, sizeof(wda->search_name));

  sts = ldh_ClassNameToId(wda->ldhses, &new_classid, classname);
  g_free(classname);
  g_free(hiername);
  g_free(searchname);
  if (EVEN(sts)) {
    CoWowGtk ww(wda->wdaclass_dia);
    ww.DisplayError("Class error", wnav_get_message(sts));
    return;
  }

  g_object_set(wda->wdaclass_dia, "visible", FALSE, NULL);

  if (wda->classid != new_classid) {
    // Enter attribute
    wda->classid = new_classid;
    wda->open_attr_dialog();
  } else {
    // Find new attributes
    sts = ((WdaNav*)wda->wdanav)
              ->update(wda->objid, wda->classid, wda->attribute,
                  wda->attrobjects, wda->search_name);
    if (EVEN(sts)) {
      CoWowGtk ww(wda->wdaclass_dia);
      ww.DisplayError("Spreadsheet error", wnav_get_message(sts));
    }
  }
}

void WdaGtk::class_activate_cancel(GtkWidget* w, gpointer data)
{
  WdaGtk* wda = (WdaGtk*)data;

  g_object_set(wda->wdaclass_dia, "visible", FALSE, NULL);
}

gboolean WdaGtk::action_inputfocus(GtkWidget* w, GdkEvent* event, gpointer data)
{
  WdaGtk* wda = (WdaGtk*)data;
  gboolean scrolledinput_visible;
  gboolean input_visible;

  if (wda->focustimer.disabled())
    return FALSE;

  g_object_get(wda->cmd_scrolledinput, "visible", &scrolledinput_visible, NULL);
  g_object_get(wda->cmd_input, "visible", &input_visible, NULL);
  if (scrolledinput_visible)
    gtk_widget_grab_focus(wda->cmd_scrolledtextview);
  else if (input_visible)
    gtk_widget_grab_focus(wda->cmd_input);
  else if (wda->wdanav)
    wda->wdanav->set_inputfocus();

  wda->focustimer.disable(400);

  return FALSE;
}

void WdaGtk::change_value_close()
{
  unsigned char* s;
  int sts;

  if (input_open) {
    if (input_multiline) {
      gchar* text;
      GtkTextIter start_iter, end_iter;
      gtk_text_buffer_get_start_iter(cmd_scrolled_buffer, &start_iter);
      gtk_text_buffer_get_end_iter(cmd_scrolled_buffer, &end_iter);

      text = gtk_text_buffer_get_text(
          cmd_scrolled_buffer, &start_iter, &end_iter, FALSE);
      // Replace ctrl characters with space
      for (s = (unsigned char*)text; *s; s++) {
        if (*s < ' ' && *s != 10 && *s != 13)
          *s = ' ';
      }

      sts = wdanav->set_attr_value(input_node, input_name, text);
      g_free(text);
      g_object_set(cmd_scrolledinput, "visible", FALSE, NULL);
      set_prompt("");
      input_open = 0;

      int h = gdk_window_get_height(gtk_widget_get_window(pane));
      gtk_paned_set_position(GTK_PANED(pane), h - 50);

      wdanav->redraw();
      wdanav->set_inputfocus();
    } else {
      char *text, *textutf8;
      textutf8 = gtk_editable_get_chars(GTK_EDITABLE(cmd_input), 0, -1);
      text = g_convert(textutf8, -1, "ISO8859-1", "UTF-8", NULL, NULL, NULL);
      g_free(textutf8);

      if (text) {
        sts = wdanav->set_attr_value(input_node, input_name, text);
        g_free(text);
      }
      g_object_set(cmd_input, "visible", FALSE, NULL);
      set_prompt("");
      input_open = 0;
      if (redraw_cb)
        (redraw_cb)(this);

      wdanav->set_inputfocus();

      if (!text)
        message('E', "Input error, invalid character");
    }
  }
}

void WdaGtk::activate_cmd_input(GtkWidget* w, gpointer data)
{
  char *text, *textutf8;
  WdaGtk* wda = (WdaGtk*)data;
  int sts;

  g_object_set(wda->cmd_prompt, "visible", FALSE, NULL);
  g_object_set(wda->cmd_input, "visible", FALSE, NULL);

  wda->wdanav->set_inputfocus();

  textutf8 = gtk_editable_get_chars(GTK_EDITABLE(w), 0, -1);
  text = g_convert(textutf8, -1, "ISO8859-1", "UTF-8", NULL, NULL, NULL);
  g_free(textutf8);

  if (wda->input_open) {
    if (text)
      sts = wda->wdanav->set_attr_value(wda->input_node, wda->input_name, text);
    g_object_set(w, "visible", FALSE, NULL);
    wda->set_prompt("");
    wda->input_open = 0;
    if (wda->redraw_cb)
      (wda->redraw_cb)(wda);

    if (!text)
      wda->message('E', "Input error, invalid character");
  }
  if (text)
    g_free(text);
}

void WdaGtk::activate_cmd_scrolled_ok(GtkWidget* w, gpointer data)
{
  WdaGtk* wda = (WdaGtk*)data;
  gchar *textutf8, *text;
  unsigned char* s;
  int sts;

  if (wda->input_open) {
    GtkTextIter start_iter, end_iter;
    gtk_text_buffer_get_start_iter(wda->cmd_scrolled_buffer, &start_iter);
    gtk_text_buffer_get_end_iter(wda->cmd_scrolled_buffer, &end_iter);

    textutf8 = gtk_text_buffer_get_text(
        wda->cmd_scrolled_buffer, &start_iter, &end_iter, FALSE);
    text = g_convert(textutf8, -1, "ISO8859-1", "UTF-8", NULL, NULL, NULL);
    g_free(textutf8);

    if (text) {
      // Replace ctrl characters with space
      for (s = (unsigned char*)text; *s; s++) {
        if (*s < ' ' && *s != 10 && *s != 13)
          *s = ' ';
      }

      sts = wda->wdanav->set_attr_value(wda->input_node, wda->input_name, text);
    }
    g_object_set(wda->cmd_scrolledinput, "visible", FALSE, NULL);
    wda->set_prompt("");
    wda->input_open = 0;

    int h = gdk_window_get_height(gtk_widget_get_window(wda->pane));
    gtk_paned_set_position(GTK_PANED(wda->pane), h - 50);

    wda->wdanav->redraw();
    wda->wdanav->set_inputfocus();
    if (text)
      g_free(text);
    else
      wda->message('E', "Input error, invalid character");
  }
}

void WdaGtk::activate_cmd_scrolled_ca(GtkWidget* w, gpointer data)
{
  WdaGtk* wda = (WdaGtk*)data;

  if (wda->input_open) {
    g_object_set(wda->cmd_scrolledinput, "visible", FALSE, NULL);

    int h = gdk_window_get_height(gtk_widget_get_window(wda->pane));
    gtk_paned_set_position(GTK_PANED(wda->pane), h - 50);

    wda->set_prompt("");
    wda->input_open = 0;
    wda->wdanav->set_inputfocus();
  }
}

void WdaGtk::pop()
{
  gtk_window_present(GTK_WINDOW(toplevel));
}

void WdaGtk::open_class_dialog(char* hierstr, char* classstr, char* namestr)
{
  gint pos = 0;
  gtk_editable_delete_text(GTK_EDITABLE(wdaclass_hiervalue), 0, -1);
  gtk_editable_insert_text(
      GTK_EDITABLE(wdaclass_hiervalue), hierstr, strlen(hierstr), &pos);
  pos = 0;
  gtk_editable_delete_text(GTK_EDITABLE(wdaclass_classvalue), 0, -1);
  gtk_editable_insert_text(
      GTK_EDITABLE(wdaclass_classvalue), classstr, strlen(classstr), &pos);
  pos = 0;
  gtk_editable_delete_text(GTK_EDITABLE(wdaclass_namevalue), 0, -1);
  gtk_editable_insert_text(
      GTK_EDITABLE(wdaclass_namevalue), namestr, strlen(namestr), &pos);
  gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON(wdaclass_attrobjects), attrobjects);
  g_object_set(wdaclass_dia, "visible", TRUE, NULL);
}

void WdaGtk::update_title()
{
  CoWowGtk::update_title(toplevel, editmode);
}

WdaGtk::~WdaGtk()
{
  if (wow)
    delete wow;
  delete cmd_entry;
  delete (WdaNav*)wdanav;
  gtk_widget_destroy(toplevel);
}

static gint delete_event(GtkWidget* w, GdkEvent* event, gpointer data)
{
  WdaGtk* wda = (WdaGtk*)data;

  if (wda->close_cb)
    (wda->close_cb)(wda);
  else
    delete wda;

  return FALSE;
}

static void destroy_event(GtkWidget* w, gpointer data)
{
}

WdaGtk::WdaGtk(GtkWidget* wa_parent_wid, void* wa_parent_ctx,
    ldh_tSesContext wa_ldhses, pwr_tObjid wa_objid, pwr_tClassId wa_classid,
    const char* wa_attribute, int wa_editmode, int wa_advanced_user,
    int wa_display_objectname)
    : Wda(wa_parent_ctx, wa_ldhses, wa_objid, wa_classid, wa_attribute,
          wa_editmode, wa_advanced_user, wa_display_objectname),
      parent_wid(wa_parent_wid), wdaclass_dia(0)
{
  int sts;

  toplevel = (GtkWidget*)g_object_new(GTK_TYPE_WINDOW, "default-height", 700,
      "default-width", 700, "title", "Spreadsheet Editor", NULL);

  g_signal_connect(toplevel, "delete_event", G_CALLBACK(delete_event), this);
  g_signal_connect(toplevel, "destroy", G_CALLBACK(destroy_event), this);
  g_signal_connect(
      toplevel, "focus-in-event", G_CALLBACK(action_inputfocus), this);

  CoWowGtk::SetWindowIcon(toplevel);

  GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  // Menu
  // Accelerators
  GtkAccelGroup* accel_g
      = (GtkAccelGroup*)g_object_new(GTK_TYPE_ACCEL_GROUP, NULL);
  gtk_window_add_accel_group(GTK_WINDOW(toplevel), accel_g);

  GtkMenuBar* menu_bar = (GtkMenuBar*)g_object_new(GTK_TYPE_MENU_BAR, NULL);

  // File entry
  GtkWidget* file_select_class
      = gtk_menu_item_new_with_mnemonic("_Select Class");
  g_signal_connect(
      file_select_class, "activate", G_CALLBACK(activate_setclass), this);
  gtk_widget_add_accelerator(file_select_class, "activate", accel_g, 'c',
      GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* file_select_attr
      = gtk_menu_item_new_with_mnemonic("Select _Attribute");
  g_signal_connect(
      file_select_attr, "activate", G_CALLBACK(activate_setattr), this);
  gtk_widget_add_accelerator(file_select_attr, "activate", accel_g, 'a',
      GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* file_next_attr = gtk_menu_item_new_with_mnemonic("Next Attribute");
  g_signal_connect(
      file_next_attr, "activate", G_CALLBACK(activate_nextattr), this);
  gtk_widget_add_accelerator(file_next_attr, "activate", accel_g, 'n',
      GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* file_prev_attr
      = gtk_menu_item_new_with_mnemonic("Previous Attribute");
  g_signal_connect(
      file_prev_attr, "activate", G_CALLBACK(activate_prevattr), this);
  gtk_widget_add_accelerator(file_prev_attr, "activate", accel_g, 'p',
      GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* file_export_text
      = gtk_menu_item_new_with_mnemonic("Export to Textfile");
  g_signal_connect(
      file_export_text, "activate", G_CALLBACK(activate_export_text), this);

  GtkWidget* file_import_text
      = gtk_menu_item_new_with_mnemonic("Import from Textfile");
  g_signal_connect(
      file_import_text, "activate", G_CALLBACK(activate_import_text), this);

  GtkWidget* file_print
      = gtk_menu_item_new_with_mnemonic("_Print");
  g_signal_connect(file_print, "activate", G_CALLBACK(activate_print), this);

  GtkWidget* file_close
      = gtk_menu_item_new_with_mnemonic("_Close");
  g_signal_connect(file_close, "activate", G_CALLBACK(activate_exit), this);
  gtk_widget_add_accelerator(file_close, "activate", accel_g, 'w',
      GdkModifierType(GDK_CONTROL_MASK), GTK_ACCEL_VISIBLE);

  GtkMenu* file_menu = (GtkMenu*)g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_select_class);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_select_attr);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_next_attr);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_prev_attr);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_export_text);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_import_text);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_print);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_close);

  GtkWidget* file = gtk_menu_item_new_with_mnemonic("_File");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), GTK_WIDGET(file_menu));

  // Functions entry
  GtkWidget* func_changevalue
      = gtk_menu_item_new_with_mnemonic("_Change Value");
  g_signal_connect(
      func_changevalue, "activate", G_CALLBACK(activate_change_value), this);
  gtk_widget_add_accelerator(func_changevalue, "activate", accel_g, 'q',
      GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* func_close_changeval
      = gtk_menu_item_new_with_mnemonic("C_lose Change Value");
  g_signal_connect(func_close_changeval, "activate",
      G_CALLBACK(activate_close_changeval), this);
  gtk_widget_add_accelerator(func_close_changeval, "activate", accel_g, 't',
      GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkMenu* func_menu = (GtkMenu*)g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(func_menu), func_changevalue);
  gtk_menu_shell_append(GTK_MENU_SHELL(func_menu), func_close_changeval);

  GtkWidget* functions = gtk_menu_item_new_with_mnemonic("_Functions");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), functions);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(functions), GTK_WIDGET(func_menu));

  // Help entry
  GtkWidget* help_help
      = gtk_menu_item_new_with_mnemonic("_Help");
  g_signal_connect(help_help, "activate", G_CALLBACK(activate_help), this);
  gtk_widget_add_accelerator(help_help, "activate", accel_g, 'h',
      GdkModifierType(GDK_CONTROL_MASK), GTK_ACCEL_VISIBLE);

  GtkMenu* help_menu = (GtkMenu*)g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_help);

  GtkWidget* help = gtk_menu_item_new_with_mnemonic("_Help");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), GTK_WIDGET(help_menu));

  pane = gtk_paned_new(GTK_ORIENTATION_VERTICAL);

  utility = ((WUtility*)parent_ctx)->utype;
  wdanav = new WdaNavGtk((void*)this, pane, "Plant", ldhses, objid, classid,
      attribute, wa_editmode, wa_advanced_user, wa_display_objectname, utility,
      &brow_widget, &sts);
  wdanav->message_cb = &Wda::message_cb;
  wdanav->change_value_cb = &Wda::change_value_cb;

  GtkWidget* statusbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  msg_label = gtk_label_new("");
  gtk_widget_set_size_request(msg_label, -1, 25);
  cmd_prompt = gtk_label_new("value > ");
  gtk_widget_set_size_request(cmd_prompt, -1, 25);
  cmd_entry = new CoWowEntryGtk(&value_recall);
  cmd_input = cmd_entry->widget();
  gtk_widget_set_size_request(cmd_input, -1, 25);
  g_signal_connect(cmd_input, "activate", G_CALLBACK(activate_cmd_input), this);

  gtk_box_pack_start(GTK_BOX(statusbar), msg_label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(statusbar), cmd_prompt, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(statusbar), cmd_input, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(menu_bar), FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(vbox), GTK_WIDGET(pane), TRUE, TRUE, 0);

  gtk_paned_pack1(GTK_PANED(pane), GTK_WIDGET(brow_widget), TRUE, TRUE);
  gtk_paned_pack2(GTK_PANED(pane), GTK_WIDGET(statusbar), FALSE, TRUE);

  gtk_container_add(GTK_CONTAINER(toplevel), vbox);

  cmd_scrolled_buffer = gtk_text_buffer_new(NULL);

  cmd_scrolledtextview = gtk_text_view_new_with_buffer(cmd_scrolled_buffer);
  GtkWidget* viewport = gtk_viewport_new(NULL, NULL);
  GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(viewport), cmd_scrolledtextview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), viewport);

  cmd_scrolled_ok = gtk_button_new_with_label("Ok");
  gtk_widget_set_size_request(cmd_scrolled_ok, 70, 25);
  g_signal_connect(
      cmd_scrolled_ok, "clicked", G_CALLBACK(activate_cmd_scrolled_ok), this);
  cmd_scrolled_ca = gtk_button_new_with_label("Cancel");
  gtk_widget_set_size_request(cmd_scrolled_ca, 70, 25);
  g_signal_connect(
      cmd_scrolled_ca, "clicked", G_CALLBACK(activate_cmd_scrolled_ca), this);

  GtkWidget* hboxbuttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 40);
  gtk_box_pack_start(GTK_BOX(hboxbuttons), cmd_scrolled_ok, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hboxbuttons), cmd_scrolled_ca, FALSE, FALSE, 0);

  cmd_scrolledinput = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(cmd_scrolledinput), scrolledwindow, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(cmd_scrolledinput), hboxbuttons, FALSE, FALSE, 5);

  gtk_box_pack_start(GTK_BOX(statusbar), cmd_scrolledinput, TRUE, TRUE, 0);

  gtk_widget_show_all(toplevel);

  g_object_set(cmd_prompt, "visible", FALSE, NULL);
  g_object_set(cmd_input, "visible", FALSE, NULL);
  g_object_set(cmd_scrolledinput, "visible", FALSE, NULL);

  int h = gdk_window_get_height(gtk_widget_get_window(pane));
  gtk_paned_set_position(GTK_PANED(pane), h - 50);

  create_class_dialog();
  wow = new CoWowGtk(toplevel);

  update_title();

  if (utility == wb_eUtility_Wtt) {
    ((Wtt*)parent_ctx)
        ->register_utility((void*)this, wb_eUtility_SpreadsheetEditor);
  }
  ((Wda*)this)->open_class_dialog();
}

static gint india_delete_event(GtkWidget* w, GdkEvent* event, gpointer wda)
{
  g_object_set(((WdaGtk*)wda)->wdaclass_dia, "visible", FALSE, NULL);
  return TRUE;
}

void WdaGtk::create_class_dialog()
{
  if (wdaclass_dia) {
    return;
  }

  // Create an input dialog
  wdaclass_dia = (GtkWidget*)g_object_new(GTK_TYPE_WINDOW, "default-height",
      150, "default-width", 550, "title", "Select Class", NULL);
  g_signal_connect(
      wdaclass_dia, "delete_event", G_CALLBACK(india_delete_event), this);

  wdaclass_classvalue = gtk_entry_new();
  GtkWidget* class_label = gtk_label_new("Class");
  gtk_widget_set_size_request(class_label, 95, -1);
  //gtk_misc_set_alignment(GTK_MISC(class_label), 0, 0.5);
  wdaclass_hiervalue = gtk_entry_new();
  GtkWidget* hier_label = gtk_label_new("Hierarchy");
  gtk_widget_set_size_request(hier_label, 95, -1);
  //gtk_misc_set_alignment(GTK_MISC(hier_label), 0, 0.5);
  wdaclass_namevalue = gtk_entry_new();
  GtkWidget* name_label = gtk_label_new("Name");
  gtk_widget_set_size_request(name_label, 95, -1);
  //gtk_misc_set_alignment(GTK_MISC(name_label), 0, 0.5);
  wdaclass_attrobjects = gtk_check_button_new_with_label("Attribute Objects");

  GtkWidget* india_hboxclass = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(india_hboxclass), class_label, FALSE, FALSE, 15);
  gtk_box_pack_end(
      GTK_BOX(india_hboxclass), wdaclass_classvalue, TRUE, TRUE, 30);

  GtkWidget* india_hboxhier = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(india_hboxhier), hier_label, FALSE, FALSE, 15);
  gtk_box_pack_end(GTK_BOX(india_hboxhier), wdaclass_hiervalue, TRUE, TRUE, 30);

  GtkWidget* india_hboxname = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(india_hboxname), name_label, FALSE, FALSE, 15);
  gtk_box_pack_end(GTK_BOX(india_hboxname), wdaclass_namevalue, TRUE, TRUE, 30);

  GtkWidget* india_hboxattrobj = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(
      GTK_BOX(india_hboxattrobj), wdaclass_attrobjects, FALSE, FALSE, 150);

  GtkWidget* india_ok = gtk_button_new_with_label("Ok");
  gtk_widget_set_size_request(india_ok, 70, 25);
  g_signal_connect(
      india_ok, "clicked", G_CALLBACK(WdaGtk::class_activate_ok), this);
  GtkWidget* india_cancel = gtk_button_new_with_label("Cancel");
  gtk_widget_set_size_request(india_cancel, 70, 25);
  g_signal_connect(
      india_cancel, "clicked", G_CALLBACK(WdaGtk::class_activate_cancel), this);

  GtkWidget* india_hboxbuttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 40);
  gtk_box_pack_start(GTK_BOX(india_hboxbuttons), india_ok, FALSE, FALSE, 40);
  gtk_box_pack_end(GTK_BOX(india_hboxbuttons), india_cancel, FALSE, FALSE, 40);

  GtkWidget* india_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(india_vbox), india_hboxclass, FALSE, FALSE, 15);
  gtk_box_pack_start(GTK_BOX(india_vbox), india_hboxhier, FALSE, FALSE, 15);
  gtk_box_pack_start(GTK_BOX(india_vbox), india_hboxname, FALSE, FALSE, 15);
  gtk_box_pack_start(GTK_BOX(india_vbox), india_hboxattrobj, TRUE, TRUE, 15);
  gtk_box_pack_start(
      GTK_BOX(india_vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(india_vbox), india_hboxbuttons, FALSE, FALSE, 15);
  gtk_container_add(GTK_CONTAINER(wdaclass_dia), india_vbox);
  gtk_widget_show_all(wdaclass_dia);
  g_object_set(wdaclass_dia, "visible", FALSE, NULL);
}
