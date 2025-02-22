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

#ifndef rt_san_h
#define rt_san_h

/* rt_san.h -- Subscribed alarm (and block) notification.  */

#include "rt_gdb.h"

typedef union {
  pwr_tBitMask m;
  pwr_32Bits(pwr_Bits(sansAct, 1), pwr_Bits(sansUpd, 1),

      pwr_Bits(fill, 30), , , , , , , , , , , , , , , , , , , , , , , , , , , ,
      , ) b;

#define san_mServer__ 0
#define san_mServer_sansAct pwr_Bit(0)
#define san_mServer_sansUpd pwr_Bit(1)
#define san_mServer_ (~san_mServer__)
} san_mServer;

typedef struct {
  net_sSanEntry sane; /* San entry.  */
  pool_sQlink sans_htl; /* Subscription server hash table link.  */
  pool_sQlink sansAct_ll; /* List of servers for one node.  */
  pool_sQlink
      sansUpd_ll; /* List of servers with pending update for one node.  */
  pwr_tUInt32 count; /* Update count */

  net_sAlarm al;
  gdb_sObject* op; /* May only be used by tmon */
  san_mServer flags;
} san_sServer;

#endif
