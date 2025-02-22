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

#ifndef wb_utility_h
#define wb_utility_h

/* wb_utility.h -- Baseclass for a workbench utility */

typedef enum {
  wb_eUtility_Wtt = 999,
  wb_eUtility_PlcEditor = 1000,
  wb_eUtility_AttributeEditor = 1001,
  wb_eUtility_Distributor = 1002,
  wb_eUtility_SpreadsheetEditor = 1003,
  wb_eUtility_Cmd = 1004,
  wb_eUtility_WNav = 1005,
  wb_eUtility_AttrTextEditor = 1006,
  wb_eUtility_Ge = 1007
} wb_eUtility;

class WUtility {
public:
  WUtility(wb_eUtility type);
  virtual ~WUtility();
  wb_eUtility utype;
};

#endif
