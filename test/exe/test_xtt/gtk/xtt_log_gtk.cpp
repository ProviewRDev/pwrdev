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

/* xtt_log_gtk.cpp -- Test log viewer */

#include "co_cdh.h"
#include "co_dcli.h"
#include "co_string.h"

#include "cow_xhelp_gtk.h"
#include "cow_wutl_gtk.h"

#include "flow_browwidget_gtk.h"
#include "flow_msg.h"

#include "xtt_log_gtk.h"

void XttLogGtk::message(char severity, char* message)
{
  gtk_label_set_text(GTK_LABEL(msg_label), message);
}

static gint delete_event(GtkWidget* w, GdkEvent* event, gpointer data)
{
  XttLogGtk* xttlog = (XttLogGtk*)data;

  if (xttlog->close_cb)
    (xttlog->close_cb)(xttlog);
  else
    delete xttlog;

  return FALSE;
}

static void destroy_event(GtkWidget* w, gpointer data)
{
  // XttLogGtk *xttlog = (XttLogGtk *)data;
  // delete xttlog;
}

static gboolean xttlog_focus_in_event(
    GtkWidget* w, GdkEvent* event, gpointer data)
{
  XttLogGtk* xttlog = (XttLogGtk*)data;

  if (xttlog->lognav)
    xttlog->lognav->set_inputfocus();

  return FALSE;
}

void XttLogGtk::activate_print(GtkWidget* w, gpointer data)
{
  XttLog* xtt = (XttLog*)data;

  xtt->activate_print();
}

void XttLogGtk::activate_close(GtkWidget* w, gpointer data)
{
  XttLog* xttlog = (XttLog*)data;
  if (xttlog->close_cb)
    (xttlog->close_cb)(xttlog);
  else
    delete xttlog;
}

void XttLogGtk::activate_zoom_in(GtkWidget* w, gpointer data)
{
  XttLog* xtt = (XttLog*)data;

  xtt->activate_zoom_in();
}

void XttLogGtk::activate_zoom_out(GtkWidget* w, gpointer data)
{
  XttLog* xtt = (XttLog*)data;

  xtt->activate_zoom_out();
}

void XttLogGtk::activate_zoom_reset(GtkWidget* w, gpointer data)
{
  XttLog* xtt = (XttLog*)data;

  xtt->activate_zoom_reset();
}

void XttLogGtk::activate_help(GtkWidget* w, gpointer data)
{
  XttLog* xtt = (XttLog*)data;

  if (!xtt->is_authorized())
    return;

  xtt->activate_help();
}

void XttLogGtk::activate_help_proview(GtkWidget* w, gpointer data)
{
  XttLog* xtt = (XttLog*)data;

  if (!xtt->is_authorized())
    return;

  xtt->activate_help_proview();
}

void XttLogGtk::activate_command(GtkWidget* w, gpointer data)
{
  XttLog* xtt = (XttLog*)data;

  if (!xtt->is_authorized())
    return;

  if (xtt->command_open) {
    g_object_set(((XttLogGtk*)xtt)->cmd_input, "visible", FALSE, NULL);
    xtt->set_prompt("");
    xtt->command_open = 0;
    return;
  }

  gtk_editable_delete_text(GTK_EDITABLE(((XttLogGtk*)xtt)->cmd_input), 0, -1);

  g_object_set(((XttLogGtk*)xtt)->cmd_input, "visible", TRUE, NULL);
  xtt->message(' ', "");
  gtk_widget_grab_focus(((XttLogGtk*)xtt)->cmd_input);

  xtt->set_prompt("xtt >");
  xtt->command_open = 1;
}

void XttLogGtk::set_prompt(const char* prompt)
{
  if (streq(prompt, "")) {
    g_object_set(cmd_prompt, "visible", FALSE, NULL);
    g_object_set(msg_label, "visible", TRUE, NULL);
  } else {
    char* promptutf8
        = g_convert(prompt, -1, "UTF-8", "ISO8859-1", NULL, NULL, NULL);

    g_object_set(msg_label, "visible", FALSE, NULL);
    g_object_set(cmd_prompt, "visible", TRUE, "label", promptutf8, NULL);
    g_free(promptutf8);
  }
}

void XttLogGtk::valchanged_cmd_input(GtkWidget* w, gpointer data)
{
  XttLog* xtt = (XttLog*)data;
  int sts;
  char *text, *textutf8;

  textutf8 = gtk_editable_get_chars(GTK_EDITABLE(w), 0, -1);
  text = g_convert(textutf8, -1, "ISO8859-1", "UTF-8", NULL, NULL, NULL);
  g_free(textutf8);

  sts = xtt->command(text);
  g_object_set(w, "visible", FALSE, NULL);
  xtt->set_prompt("");
  xtt->command_open = 0;
  xtt->lognav->set_inputfocus();
}

XttLogGtk::XttLogGtk(GtkWidget* a_parent_wid, void* a_parent_ctx,
		     LogNav_hier *a_tree)
  : XttLog(a_parent_ctx, a_tree), parent_wid(a_parent_wid)
{
  int sts;

  toplevel = (GtkWidget*)g_object_new(GTK_TYPE_WINDOW, "default-height", 700,
      "default-width", 500, "title", "Testlog Viewer", NULL);

  g_signal_connect(toplevel, "delete_event", G_CALLBACK(delete_event), this);
  g_signal_connect(toplevel, "destroy", G_CALLBACK(destroy_event), this);
  g_signal_connect(
      toplevel, "focus-in-event", G_CALLBACK(xttlog_focus_in_event), this);

  int dark_theme = CoWowGtk::GetDarkTheme(toplevel);
  CoWowGtk::SetWindowIcon(toplevel);

  GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  // Menu
  // Accelerators
  GtkAccelGroup* accel_g
      = (GtkAccelGroup*)g_object_new(GTK_TYPE_ACCEL_GROUP, NULL);
  gtk_window_add_accel_group(GTK_WINDOW(toplevel), accel_g);

  GtkMenuBar* menu_bar = (GtkMenuBar*)g_object_new(GTK_TYPE_MENU_BAR, NULL);

  // File entry
  GtkWidget* file_print = gtk_menu_item_new_with_mnemonic(
      CoWowGtk::translate_utf8("_Print"));
  g_signal_connect(file_print, "activate", G_CALLBACK(activate_print), this);

  GtkWidget* file_close
      = gtk_menu_item_new_with_mnemonic("_Close");
  g_signal_connect(file_close, "activate", G_CALLBACK(activate_close), this);
  gtk_widget_add_accelerator(file_close, "activate", accel_g, 'w',
      GdkModifierType(GDK_CONTROL_MASK), GTK_ACCEL_VISIBLE);

  GtkMenu* file_menu = (GtkMenu*)g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_print);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_close);

  GtkWidget* file = gtk_menu_item_new_with_mnemonic("_File");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), GTK_WIDGET(file_menu));

  // Functions entry
  GtkWidget* functions_command
      = gtk_menu_item_new_with_mnemonic(CoWowGtk::translate_utf8("Co_mmand"));
  g_signal_connect(
      functions_command, "activate", G_CALLBACK(activate_command), this);
  gtk_widget_add_accelerator(functions_command, "activate", accel_g, 'b',
      GdkModifierType(GDK_CONTROL_MASK), GTK_ACCEL_VISIBLE);

  GtkMenu* functions_menu = (GtkMenu*)g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(functions_menu), functions_command);

  GtkWidget* functions
      = gtk_menu_item_new_with_mnemonic(CoWowGtk::translate_utf8("_Functions"));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), functions);
  gtk_menu_item_set_submenu(
      GTK_MENU_ITEM(functions), GTK_WIDGET(functions_menu));

  // View menu
  GtkWidget* view_zoom_in = gtk_menu_item_new_with_mnemonic(
      CoWowGtk::translate_utf8("Zoom _In"));
  g_signal_connect(
      view_zoom_in, "activate", G_CALLBACK(activate_zoom_in), this);
  gtk_widget_add_accelerator(view_zoom_in, "activate", accel_g, 'i',
      GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* view_zoom_out = gtk_menu_item_new_with_mnemonic(
      CoWowGtk::translate_utf8("Zoom _Out"));
  g_signal_connect(
      view_zoom_out, "activate", G_CALLBACK(activate_zoom_out), this);
  gtk_widget_add_accelerator(view_zoom_out, "activate", accel_g, 'o',
      GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* view_zoom_reset = gtk_menu_item_new_with_mnemonic(
      CoWowGtk::translate_utf8("Zoom _Reset"));
  g_signal_connect(
      view_zoom_reset, "activate", G_CALLBACK(activate_zoom_reset), this);

  GtkMenu* view_menu = (GtkMenu*)g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), view_zoom_in);
  gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), view_zoom_out);
  gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), view_zoom_reset);

  GtkWidget* view
      = gtk_menu_item_new_with_mnemonic(CoWowGtk::translate_utf8("_View"));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(view), GTK_WIDGET(view_menu));

  // Help entry
  GtkWidget* help_overview = gtk_menu_item_new_with_mnemonic(
      CoWowGtk::translate_utf8("_Overview"));
  g_signal_connect(help_overview, "activate", G_CALLBACK(activate_help), this);
  gtk_widget_add_accelerator(help_overview, "activate", accel_g, 'h',
      GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget* help_proview = gtk_menu_item_new_with_mnemonic(
      CoWowGtk::translate_utf8("_About Proview"));
  g_signal_connect(
      help_proview, "activate", G_CALLBACK(activate_help_proview), this);

  GtkMenu* help_menu = (GtkMenu*)g_object_new(GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_overview);
  gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_proview);

  GtkWidget* help
      = gtk_menu_item_new_with_mnemonic(CoWowGtk::translate_utf8("_Help"));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), GTK_WIDGET(help_menu));

  // Toolbar
  GtkToolbar* tools = (GtkToolbar*)g_object_new(GTK_TYPE_TOOLBAR, NULL);

  wutl_tools_item(tools, 
      dark_theme ? "$pwr_exe/ico_zoomin_d_30.png" : "$pwr_exe/ico_zoomin_l_30.png", 
      G_CALLBACK(activate_zoom_in), "Zoom in", this, 1, 1);

  wutl_tools_item(tools,
      dark_theme ? "$pwr_exe/ico_zoomout_d_30.png" : "$pwr_exe/ico_zoomout_l_30.png", 
      G_CALLBACK(activate_zoom_out), "Zoom out", this, 1, 1);

  wutl_tools_item(tools,
      dark_theme ? "$pwr_exe/ico_zoomreset_d_30.png" : "$pwr_exe/ico_zoomreset_l_30.png", 
      G_CALLBACK(activate_zoom_reset), "Zoom reset", this, 1, 1);

  // Statusbar and cmd input
  GtkWidget* statusbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  msg_label = gtk_label_new("");
  gtk_widget_set_size_request(msg_label, -1, 25);
  cmd_prompt = gtk_label_new("xtt > ");
  gtk_widget_set_size_request(cmd_prompt, -1, 25);

  cmd_recall = new CoWowRecall();
  cmd_entry = new CoWowEntryGtk(cmd_recall);
  cmd_input = cmd_entry->widget();
  gtk_widget_set_size_request(cmd_input, -1, 25);
  g_signal_connect(
      cmd_input, "activate", G_CALLBACK(valchanged_cmd_input), this);

  gtk_box_pack_start(GTK_BOX(statusbar), msg_label, FALSE, FALSE, 20);
  gtk_box_pack_start(GTK_BOX(statusbar), cmd_prompt, FALSE, FALSE, 20);
  gtk_box_pack_start(GTK_BOX(statusbar), cmd_input, TRUE, TRUE, 20);
  gtk_widget_show_all(statusbar);

  lognav
    = new LogNavGtk(this, toplevel, tree, &brow_widget, &sts);
  lognav->message_cb = &XttLog::message;
  lognav->command_cb = &XttLog::command_cb;

  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(menu_bar), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(tools), FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(brow_widget), TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(statusbar), FALSE, FALSE, 3);

  gtk_container_add(GTK_CONTAINER(toplevel), vbox);

  gtk_widget_show_all(toplevel);

  g_object_set(cmd_prompt, "visible", FALSE, NULL);
  g_object_set(cmd_input, "visible", FALSE, NULL);

  // Create help window
  CoXHelp* xhelp = new CoXHelpGtk(toplevel, this, xhelp_eUtility_Xtt, &sts);
  CoXHelp::set_default(xhelp);

  wow = new CoWowGtk(toplevel);
}

XttLogGtk::~XttLogGtk()
{
  delete (LogNavGtk*)lognav;

  gtk_widget_destroy(toplevel);
}
