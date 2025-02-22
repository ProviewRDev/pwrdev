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

#include <iostream>

#include "co_string.h"

#include "xtt_log.h"
#include "xtt_xnav.h"

XttLog* XttLog::m_default_log = 0;

XttLog::XttLog(const char* filename, int event) : m_event(event), m_level(1)
{
  char category_str[20];

  dcli_translate_filename(m_filename, filename);
  strncpy(m_pid, syi_ProcessId(), sizeof(m_pid));

  gdh_RegisterLogFunction(gdh_log_bc);

  category_to_string(xttlog_eCategory_LogStart, category_str);
  log(category_str, 0, 0, 0, 0);
}

void XttLog::delete_default()
{
  if (m_default_log) {
    delete m_default_log;
    m_default_log = 0;
  }
}

void XttLog::value_to_octstring(const void* value, unsigned int value_size,
    char* str, unsigned int str_size)
{
  unsigned int len = 0;
  for (unsigned int i = 0; i < value_size; i++) {
    if (i == value_size - 1) {
      if (len + 4 >= str_size)
        break;
      len += sprintf(&str[i * 5], "0x%02hhx", *(((unsigned char*)value) + i));
    } else {
      if (len + 5 >= str_size)
        break;
      len += sprintf(&str[i * 5], "0x%02hhx,", *(((unsigned char*)value) + i));
    }
  }
}

void XttLog::octstring_to_value(
    char* str, void* value, unsigned int size, unsigned int* value_size)
{
  char* buf = (char*)value;
  unsigned int len;

  unsigned int i = 0;
  while (1) {
    len = sscanf(&str[i * 5 + 1], "0x%2hhx", &buf[i]);
    if (len != 1)
      break;
    i++;
    if (str[i * 5] != ',' || i >= size)
      break;
  }
  *value_size = i;
}

void XttLog::gdh_log_bc(char* name, void* value, unsigned int size)
{
  char str[1000];

  value_to_octstring(value, size, str, sizeof(str));
  dlog(xttlog_eCategory_SetObjectInfo, name, str, 0);
}

void XttLog::dlog(xttlog_eCategory category, const char* str, const char* value,
    unsigned int opt, unsigned int size)
{
  char category_str[40];

  category_to_string(category, category_str);
  if (m_default_log) {
    if (!m_default_log->m_event && category == xttlog_eCategory_Event)
      return;
    m_default_log->log(category_str, str, value, opt, size);
  }
}

void XttLog::category_to_string(xttlog_eCategory category, char* str)
{
  switch (category) {
  case xttlog_eCategory_OpenGraph:
    strcpy(str, "OpenGraph");
    break;
  case xttlog_eCategory_CloseGraph:
    strcpy(str, "CloseGraph");
    break;
  case xttlog_eCategory_SetObjectInfo:
    strcpy(str, "SetObjectInfo");
    break;
  case xttlog_eCategory_Command:
    strcpy(str, "Command");
    break;
  case xttlog_eCategory_ApplNew:
    strcpy(str, "ApplNew");
    break;
  case xttlog_eCategory_ApplDelete:
    strcpy(str, "ApplDelete");
    break;
  case xttlog_eCategory_LogStart:
    strcpy(str, "LogStart");
    break;
  case xttlog_eCategory_Event:
    strcpy(str, "Event");
    break;
  case xttlog_eCategory_GeConfirmOk:
    strcpy(str, "GeConfirmOk");
    break;
  case xttlog_eCategory_GeConfirmCancel:
    strcpy(str, "GeConfirmCancel");
    break;
  case xttlog_eCategory_User:
    strcpy(str, "User");
    break;
  default:
    strcpy(str, "");
  }
}

void XttLog::string_to_category(char* str, xttlog_eCategory* category)
{
  if (streq(str, "OpenGraph"))
    *category = xttlog_eCategory_OpenGraph;
  else if (streq(str, "CloseGraph"))
    *category = xttlog_eCategory_CloseGraph;
  else if (streq(str, "SetObjectInfo"))
    *category = xttlog_eCategory_SetObjectInfo;
  else if (streq(str, "Command"))
    *category = xttlog_eCategory_Command;
  else if (streq(str, "ApplNew"))
    *category = xttlog_eCategory_ApplNew;
  else if (streq(str, "ApplDelete"))
    *category = xttlog_eCategory_ApplDelete;
  else if (streq(str, "LogStart"))
    *category = xttlog_eCategory_LogStart;
  else if (streq(str, "Event"))
    *category = xttlog_eCategory_Event;
  else if (streq(str, "GeConfirmOk"))
    *category = xttlog_eCategory_GeConfirmOk;
  else if (streq(str, "GeConfirmCancel"))
    *category = xttlog_eCategory_GeConfirmCancel;
  else if (streq(str, "User"))
    *category = xttlog_eCategory_User;
  else
    *category = xttlog_eCategory_;
}

void XttLog::log(const char* category, const char* str, const char* value,
    unsigned int opt, unsigned int size)
{
  std::ofstream fp;
  pwr_tStatus sts;
  char timstr[40];
  char username[80];

  time_AtoAscii(0, time_eFormat_DateAndTime, timstr, sizeof(timstr));
  sts = syi_UserName(username, sizeof(username));
  if (EVEN(sts))
    strcpy(username, "Unknown");

  fp.open(m_filename, std::ios::out | std::ios::app);
  if (!fp)
    return;

  if (m_level == 1)
    fp << "1  ";
  else
    fp << " " << m_level << " ";

  fp << timstr << " " << m_pid;
  for (int i = strlen(m_pid); i < 7; i++)
    fp << " ";
  fp << " " << category;
  for (int i = strlen(category); i < 11; i++)
    fp << " ";
  fp << " ";
  fp << " " << username;
  for (int i = strlen(username); i < 11; i++)
    fp << " ";
  fp << " ";
  if (str)
    fp << str;
  if (value) {
    if (opt & xttlog_mOption_Binary) {
      char str[1000];

      value_to_octstring(value, size, str, sizeof(str));

      fp << " \"" << str << "\"";
    } else
      fp << " \"" << value << "\"";
  }
  fp << '\n';
  fp.close();
}

int XttLog::play(XNav* xnav, char* filename, double speed, int pid)
{
  std::ifstream fp;
  pwr_tFileName fname;
  char line[1000];
  int ind;
  int lpid;
  char t1[20], t2[20];
  char type[40];
  char user[40];
  char attr[512];
  char value[1000];
  xttlog_eCategory category;
  int num;
  char timstr[40];
  pwr_tTime log_time;
  pwr_tTime prev_time;
  pwr_tDeltaTime diff_time;
  float diff_time_f;
  int first = 1;

  if (speed < 0.01)
    speed = 1;

  dcli_translate_filename(fname, filename);

  fp.open(fname);
  if (!fp)
    return 0;

  CoWow::SetAutoRemove(1);

  while (fp.getline(line, sizeof(line))) {
    num = sscanf(line, "%d %s %s %d %s %s", &ind, t1, t2, &lpid, type, user);

    if (pid != 0 && pid != lpid)
      continue;

    strcpy(timstr, t1);
    strcat(timstr, " ");
    strcat(timstr, t2);

    time_AsciiToA(timstr, &log_time);
    if (!first) {
      time_Adiff(&diff_time, &log_time, &prev_time);
      diff_time_f = time_DToFloat(0, &diff_time);
    } else
      diff_time_f = 0;

    if (diff_time_f / speed > 0.01) {
      xnav->wow->Wait(diff_time_f / speed);
    }

    char* s;
    int cnt = 0;
    for (s = line; *s; s++) {
      if (s > line && (*(s - 1) == ' ' || *(s - 1) == '	')
          && (*s != ' ' && *s != '	'))
        cnt++;
      if (cnt == num)
        break;
    }

    string_to_category(type, &category);
    switch (category) {
    case xttlog_eCategory_Command:
    case xttlog_eCategory_OpenGraph:
    case xttlog_eCategory_CloseGraph:
    case xttlog_eCategory_ApplNew:
    case xttlog_eCategory_ApplDelete: {
      int sts = xnav->command(s);
      printf("%8.3f %-14s %9d %s\n", diff_time_f, type, sts, s);
      break;
    }
    case xttlog_eCategory_SetObjectInfo: {
      unsigned char buf[500];
      unsigned int size;
      pwr_tStatus sts = 0;

      sscanf(s, "%s %s", attr, value);

      octstring_to_value(value, buf, sizeof(buf), &size);

      if (size) {
        sts = gdh_SetObjectInfo(attr, buf, size);
        if (EVEN(sts)) {
          printf("gdh_SetObjectInfo error: %s %s\n", attr, value);
        }
      }
      printf("%8.3f %-14s %9d %s %s\n", diff_time_f, type, sts, attr, value);
      break;
    }
    case xttlog_eCategory_Event: {
      unsigned char buf[500];
      unsigned int size;
      pwr_tStatus sts;
      char graph[600];
      pwr_tAName instance;
      char* t;

      sts = 0;
      sscanf(s, "%s %s", graph, value);

      octstring_to_value(value, buf, sizeof(buf), &size);

      t = strchr(graph, ':');
      if (!t)
        break;

      *t = 0;
      strcpy(instance, t + 1);

      xnav->ge_event_exec(xttlog_eCategory_Event, graph, instance, buf, size);

      printf(
          "%8.3f %-14s %9d %s %10.10s\n", diff_time_f, type, sts, graph, value);
      break;
    }
    case xttlog_eCategory_GeConfirmOk:
    case xttlog_eCategory_GeConfirmCancel: {
      pwr_tStatus sts;
      char graph[600];
      pwr_tAName instance;
      char* t;

      sts = 0;
      strcpy(graph, s);

      t = strchr(graph, ':');
      if (!t)
        break;

      *t = 0;
      strcpy(instance, t + 1);

      xnav->ge_event_exec(category, graph, instance, 0, 0);

      printf("%8.3f %-14s %9d %s\n", diff_time_f, type, sts, graph);
      break;
    }
    case xttlog_eCategory_LogStart:
      printf("%8.3f %-14s\n", diff_time_f, type);
      break;
    default:;
    }

    prev_time = log_time;
    if (first)
      first = 0;
  }
  CoWow::SetAutoRemove(0);
  printf("         %-14s\n", "EndOfFile");
  return 1;
}
