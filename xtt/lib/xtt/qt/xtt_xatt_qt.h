/*
 * Proview   Open Source Process Control.
 * Copyright (C) 2005-2017 SSAB EMEA AB.
 *
 * This file is part of Proview.
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
 * along with Proview. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking Proview statically or dynamically with other modules is
 * making a combined work based on Proview. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * Proview give you permission to, from the build function in the
 * Proview Configurator, combine Proview with modules generated by the
 * Proview PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every
 * copy of the combined work is accompanied by a complete copy of
 * the source code of Proview (the version used to produce the
 * combined work), being distributed under the terms of the GNU
 * General Public License plus this exception.
 */

#ifndef xtt_xatt_qt_h
#define xtt_xatt_qt_h

/* xtt_xatt_qt.h -- Object attribute editor */

#include "cow_wow_qt.h"

#ifndef xtt_xatt_h
#include "xtt_xatt.h"
#endif

#include <QLabel>
#include <QSplitter>
#include <QTextEdit>

class XAttQtWidget;

class XAttQt : public XAtt {
public:
  XAttQt(QWidget* xa_parent_wid, void* xa_parent_ctx, pwr_sAttrRef* xa_objar,
      int xa_advanced_user, int* xa_sts);
  QWidget* brow_widget;
  QWidget* form_widget;
  QLabel* msg_label;
  QLabel* cmd_prompt;
  QWidget* cmd_scrolledinput;
  QTextEdit* cmd_scrolled_buffer;
  QSplitter* pane;
  static CoWowRecall value_recall;
  CoWowEntryQt* cmd_entry;
  CoWowFocusTimerQt focustimer;
  int input_max_length;

  void message(char severity, const char* message);
  void set_prompt(const char* prompt);
  void change_value(int set_focus);
  int open_changevalue(char* name);
  void change_value_close();
  void pop();
  void print();

private:
  XAttQtWidget* toplevel;
};

class XAttQtWidget : public QWidget {
  Q_OBJECT

public:
  XAttQtWidget(XAttQt* parent_ctx, QWidget* parent)
      : QWidget(), xatt(parent_ctx)
  {
  }

protected:
  void focusInEvent(QFocusEvent* event);
  void closeEvent(QCloseEvent* event);

public slots:
  void activate_change_value();
  void activate_close_changeval();
  void activate_print();
  void activate_display_object();
  void activate_show_cross();
  void activate_open_classgraph();
  void activate_open_plc();
  void activate_help();
  void activate_cmd_entry();
  void activate_cmd_scrolled_ok();
  void activate_cmd_scrolled_ca();
  void action_text_inserted();

private:
  XAttQt* xatt;
};

#endif