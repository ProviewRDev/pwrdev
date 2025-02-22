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

#ifndef xtt_log_h
#define xtt_log_h

#include "cow_log.h"

#define xttlog_cLogFile "$pwrp_log/xtt_%s.log"

typedef enum {
  xttlog_eCategory_,
  xttlog_eCategory_Command,
  xttlog_eCategory_OpenGraph,
  xttlog_eCategory_CloseGraph,
  xttlog_eCategory_ApplNew,
  xttlog_eCategory_ApplDelete,
  xttlog_eCategory_SetObjectInfo,
  xttlog_eCategory_LogStart,
  xttlog_eCategory_Event,
  xttlog_eCategory_GeConfirmOk,
  xttlog_eCategory_GeConfirmCancel,
  xttlog_eCategory_User,
  xttlog_eCategory__
} xttlog_eCategory;

typedef enum { xttlog_mOption_Binary = 1 << 0 } xttlog_mOption;

class XttLog;
class XNav;

class XttLog {
  pwr_tFileName m_filename;
  int m_event;
  int m_level;
  char m_pid[40];
  static XttLog* m_default_log;

public:
  void push()
  {
    m_level++;
  }
  void pull()
  {
    m_level--;
    if (m_level < 1)
      m_level = 1;
  }

  XttLog(const char* filename, int event);
  ~XttLog()
  {
    if (this == m_default_log)
      m_default_log = 0;
  }

  void set_default()
  {
    m_default_log = this;
  }
  void log(const char* category, const char* str, const char* value,
      unsigned int opt, unsigned int size);

  static void dlog(xttlog_eCategory category, const char* str,
      const char* value, unsigned int opt = 0, unsigned int size = 0);
  static void category_to_string(xttlog_eCategory category, char* str);
  static void string_to_category(char* str, xttlog_eCategory* category);
  static void dpush()
  {
    if (m_default_log)
      m_default_log->push();
  }
  static void dpull()
  {
    if (m_default_log)
      m_default_log->pull();
  }
  static void value_to_octstring(const void* value, unsigned int value_size,
      char* str, unsigned int str_size);
  static void octstring_to_value(
      char* str, void* value, unsigned int size, unsigned int* value_size);
  static void gdh_log_bc(char* name, void* buf, unsigned int bufsize);
  static int play(XNav* xnav, char* filename, double speed, int pid);
  static void delete_default();
};

#endif
