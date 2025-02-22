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

/* co_msgwindow.cpp -- Message window */

#include <stdlib.h>
#include <string.h>

#include "co_cdh.h"
#include "co_time.h"
#include "co_dcli.h"

#include "co_lng.h"
#include "cow_xhelp.h"
#include "cow_msgwindow.h"

MsgWindow* MsgWindow::default_window = 0;
int MsgWindow::hide_info = 0;

MsgWindow::MsgWindow(
    void* msg_parent_ctx, const char* msg_name, pwr_tStatus* status)
    : parent_ctx(msg_parent_ctx), msgnav(NULL), displayed(0), deferred_map(0),
      nodraw(0), size(0), max_size(500), find_wnav_cb(0), find_plc_cb(0),
      find_ge_cb(0), wow(0)
{
  *status = 1;
  strcpy(name, msg_name);
}

MsgWindow::~MsgWindow()
{
}

void MsgWindow::map()
{
}

void MsgWindow::unmap()
{
}

void MsgWindow::print()
{
}

int MsgWindow::is_mapped()
{
  return displayed;
}

void MsgWindow::insert(
    int severity, const char* text, pwr_tOid oid, msgw_eRow row)
{
  if (size > max_size - 1)
    msgnav->set_nodraw();

  if (cdh_ObjidIsNull(oid))
    new ItemMsg(msgnav, "", (char*)text, severity, NULL, flow_eDest_Before);
  else if (row == msgw_eRow_Plc)
    new ItemMsgObjectPlc(
        msgnav, "", (char*)text, severity, oid, NULL, flow_eDest_Before);
  else
    new ItemMsgObject(
        msgnav, "", (char*)text, severity, oid, NULL, flow_eDest_Before);

  if (size > max_size - 1) {
    msgnav->remove_oldest();
    msgnav->reset_nodraw();
  } else
    size++;
}

void MsgWindow::insert(
    int severity, const char* text, char* object, void* utility, msgw_eRow row)
{
  if (size > max_size - 1)
    msgnav->set_nodraw();

  if (!object)
    new ItemMsg(msgnav, "", (char*)text, severity, NULL, flow_eDest_Before);
  else if (row == msgw_eRow_Ge)
    new ItemMsgObjectGe(msgnav, "", (char*)text, severity, object, utility,
        NULL, flow_eDest_Before);
  else
    new ItemMsg(msgnav, "", (char*)text, severity, NULL, flow_eDest_Before);

  if (size > max_size - 1) {
    msgnav->remove_oldest();
    msgnav->reset_nodraw();
  } else
    size++;
}

void MsgWindow::set_nodraw()
{
  msgnav->set_nodraw();
  nodraw++;
}

void MsgWindow::reset_nodraw()
{
  msgnav->reset_nodraw();
  nodraw--;
  if (!nodraw && deferred_map)
    map();
}

void MsgWindow::msg(
    int severity, const char* text, msgw_ePop pop, pwr_tOid oid, msgw_eRow row)
{
  if (severity == 'O')
    severity = 'I';
  insert(severity, text, oid, row);
  if ((pop == msgw_ePop_Yes
          || (pop == msgw_ePop_Default
                 && (severity == 'E' || severity == 'F' || severity == 'W'))))
    map();
}

void MsgWindow::activate_print()
{
  print();
}

void MsgWindow::set_default(MsgWindow* msgw)
{
  default_window = msgw;
}

int MsgWindow::has_default()
{
  return default_window ? 1 : 0;
}

void MsgWindow::message(
    int severity, const char* text, msgw_ePop pop, pwr_tOid oid, msgw_eRow row)
{
  if (default_window) {
    if (severity == 'O')
      severity = 'I';
    default_window->insert(severity, text, oid, row);
  } else {
    if ((hide_info && severity == 'I') || severity == 'O')
      return;
    if (severity == 'E' || severity == 'W' || severity == 'F' || severity == 'I'
        || severity == 'S')
      printf("%c %s\n", severity, text);
    else
      printf("%s\n", text);
  }
  if (default_window
      && (pop == msgw_ePop_Yes
             || (pop == msgw_ePop_Default && (severity == 'E' || severity == 'F'
                                                 || severity == 'W'))))
    default_window->map();
}

void MsgWindow::message(int severity, const char* text, msgw_ePop pop,
    char* object, void* utility, msgw_eRow row)
{
  if (default_window) {
    if (severity == 'O')
      severity = 'I';
    default_window->insert(severity, text, object, utility, row);
  } else {
    if ((hide_info && severity == 'I') || severity == 'O')
      return;
    if (severity == 'E' || severity == 'W' || severity == 'F' || severity == 'I'
        || severity == 'S')
      printf("%c %s\n", severity, text);
    else
      printf("%s\n", text);
  }
  if (default_window
      && (pop == msgw_ePop_Yes
             || (pop == msgw_ePop_Default && (severity == 'E' || severity == 'F'
                                                 || severity == 'W'))))
    default_window->map();
}

void MsgWindow::message(const co_error& e, const char* text1, const char* text2,
    pwr_tOid oid, msgw_eRow row)
{
  pwr_tStatus sts = e.sts();
  int severity;
  switch (sts & 7) {
  case 0:
    severity = 'W';
    break;
  case 1:
    severity = 'S';
    break;
  case 2:
    severity = 'E';
    break;
  case 3:
    severity = 'I';
    break;
  case 4:
    severity = 'F';
    break;
  default:
    severity = ' ';
  }
  message(severity, e.what().c_str(), text1, text2, oid, row);
}

void MsgWindow::message(int severity, const char* text1, const char* text2,
    const char* text3, pwr_tOid oid, msgw_eRow row)
{
  char text[400];
  strncpy(text, text1, sizeof(text));
  text[sizeof(text) - 1] = 0;
  if (text2) {
    strncat(text, " ", 400 - strlen(text));
    strncat(text, text2, 400 - strlen(text));
    text[sizeof(text) - 1] = 0;
  }
  if (text3) {
    strncat(text, " ", 400 - strlen(text));
    strncat(text, text3, 400 - strlen(text));
    text[sizeof(text) - 1] = 0;
  }
  MsgWindow::message(severity, text, msgw_ePop_Default, oid, row);
}

void MsgWindow::message(int severity, const char* text1, const char* text2,
    const char* text3, char* object, void* utility, msgw_eRow row)
{
  char text[400];
  strncpy(text, text1, sizeof(text));
  text[sizeof(text) - 1] = 0;
  if (text2) {
    strncat(text, " ", 400 - strlen(text));
    strncat(text, text2, 400 - strlen(text));
    text[sizeof(text) - 1] = 0;
  }
  if (text3) {
    strncat(text, " ", 400 - strlen(text));
    strncat(text, text3, 400 - strlen(text));
    text[sizeof(text) - 1] = 0;
  }
  MsgWindow::message(severity, text, msgw_ePop_Default, object, utility, row);
}

bool MsgWindow::has_window()
{
  return default_window != 0;
}

CoWow* MsgWindow::get_wow()
{
  return default_window ? default_window->wow : 0;
}

void MsgWindow::map_default()
{
  if (default_window)
    default_window->map();
}

void MsgWindow::dset_nodraw()
{
  if (default_window)
    default_window->set_nodraw();
}

void MsgWindow::dreset_nodraw()
{
  if (default_window)
    default_window->reset_nodraw();
}

void MsgWindow::hide_info_messages(int hide)
{
  hide_info = hide;
}

void MsgWindow::msgw_find_wnav_cb(void* ctx, pwr_tOid oid)
{
  MsgWindow* msgw = (MsgWindow*)ctx;
  if (msgw->find_wnav_cb)
    (msgw->find_wnav_cb)(msgw->parent_ctx, oid);
}

void MsgWindow::msgw_find_plc_cb(void* ctx, pwr_tOid oid)
{
  MsgWindow* msgw = (MsgWindow*)ctx;
  if (msgw->find_plc_cb)
    (msgw->find_plc_cb)(msgw->parent_ctx, oid);
}

void MsgWindow::msgw_find_ge_cb(void* ctx, char* object, void* utility)
{
  MsgWindow* msgw = (MsgWindow*)ctx;
  if (msgw->find_ge_cb)
    (msgw->find_ge_cb)(msgw->parent_ctx, object, utility);
}
