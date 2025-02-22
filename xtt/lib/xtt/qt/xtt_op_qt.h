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

#ifndef xtt_op_qt_h
#define xtt_op_qt_h

/* xtt_op_qt.h -- Operator window in xtt */

#include <vector>

#include "xtt_op.h"

#include "cow_wow_qt.h"

#include <QLabel>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>

class OpCmd {
public:
  QMenu* w;
  pwr_tCmd cmd;

  OpCmd(QMenu* widget, const char* command) : w(widget)
  {
    strncpy(cmd, command, sizeof(cmd));
  }
};

class OpQtWidget;

class OpQt : public Op {
public:
  OpQt(void* op_parent_ctx, char* opplace, pwr_tStatus* status);
  ~OpQt();

  QLabel* alarmcnt_label;
  QWidget* aalarm_mark;
  QLabel* aalarm_label[5];
  QWidget* aalarm_active[5];
  QWidget* aalarm_info[5];
  QWidget* aalarm_box[5];
  QLabel* balarm_label;
  QWidget* balarm_active;
  QWidget* balarm_info;
  QWidget* balarm_box;
  QLabel* balarm_mark;
  QVBoxLayout* appl_form;
  QWidget* decr_button;
  QAction* tools_close;
  QAction* functions_close;
  QWidget* funcbox[5];
  QMenuBar* menu_bar;
  QLabel* title_label;
  QPushButton* appl_buttons[25];
  QToolBar* tools;
  QToolBar* tools2;
  QColor red_color;
  QColor yellow_color;
  QColor green_color;
  QColor gray_color;
  QColor white_color;
  int a_height;
  int a_exist[5];
  int a_active[5];
  char a_alarm_moretext[5][256];
  char b_alarm_moretext[256];
  int text_size;
  QRect monitor_geometry;
  CoWowFocusTimerQt poptimer;
  std::vector<OpCmd> cmd_vect;

  void map();
  int configure(char* opplace_str);
  void update_alarm_info();
  void add_close_button();
  int get_cmd(QMenu* w, char* cmd);
  int create_menu_item(
      const char* name, int pixmap, int append, const char* cmd);
  int delete_menu_item(const char* name);
  void change_sup_color(void* imagew, op_eSupColor color);
  void set_title(char* user);
  void set_color_theme(int idx);
  void set_text_size();

private:
  void alarm_box_helper(int i);

  OpQtWidget* toplevel;
};

class OpQtWidget : public QFrame {
  Q_OBJECT

public:
  OpQtWidget(OpQt* parent_ctx) : QFrame(), op(parent_ctx) {}

protected:
  void closeEvent(QCloseEvent* event);

public slots:
  void activate_aalarm_ack();
  void activate_balarm_ack();
  void activate_aalarm_incr();
  void activate_aalarm_decr();
  void activate_zoom_in();
  void activate_zoom_out();
  void activate_colortheme();
  void activate_alarmlist();
  void activate_eventlist();
  void activate_eventlog();
  void activate_blocklist();
  void activate_navigator();
  void activate_help();
  void activate_help_overview();
  void activate_help_opwin();
  void activate_help_proview();
  void activate_trend();
  void activate_fast();
  void activate_history();
  void activate_switch_user();
  void activate_show_user();
  void activate_logout();
  void activate_cmd_menu_item();
  void activate_sup_node();
  void activate_graph();
  void activate_appl();
  void activate_info();

private:
  OpQt* op;
};

class ClickableFrame : public QFrame {
  Q_OBJECT

public:
  ClickableFrame();
  ~ClickableFrame();

signals:
  void clicked();

protected:
  void mousePressEvent(QMouseEvent* event);
};

#endif
