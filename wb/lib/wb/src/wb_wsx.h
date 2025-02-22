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

/* wb_wsx.h -- wb syntax control
   Wb syntax control.
   This module control the syntax for objects.  */

#ifndef wb_wsx_h
#define wb_wsx_h

#include "wb_ldh.h"

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  wsx_mCardOption_None = 0,
  wsx_mCardOption_DevName = 1 << 0,
  wsx_mCardOption_ErrorLimits = 1 << 1
} wsx_mCardOption;

pwr_tStatus wsx_error_msg(ldh_tSesContext sesctx, pwr_tStatus sts,
    pwr_tAttrRef aref, int* errorcount, int* warningcount);

pwr_tStatus wsx_error_msg_object(ldh_tSesContext sesctx, pwr_tStatus sts,
    pwr_tOid oid, int* errorcount, int* warningcount);

pwr_tStatus wsx_error_msg_str(ldh_tSesContext sesctx, const char* str,
    pwr_tAttrRef aref, int severity, int* errorcount, int* warningcount);

pwr_tStatus wsx_CheckCard(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    int* errorcount, int* warningcount, wsx_mCardOption options);

pwr_tStatus wsx_CheckCoCard(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    int* errorcount, int* warningcount);

pwr_tStatus wsx_CheckIoDevice(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    int* errorcount, int* warningcount, wsx_mCardOption options);

pwr_tStatus wsx_CheckSigChanCon(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    int* errorcount, int* warningcount);

pwr_tStatus wsx_CheckAttrRef(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    const pwr_tObjName attribute, pwr_tCid* cid_vect, int null_is_ok,
    int* errorcount, int* warningcount);

pwr_tStatus wsx_CheckXAttrRef(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    const pwr_tObjName attribute, const pwr_tObjName back_attribute,
    pwr_tCid* cid_vect, int null_is_ok, int* errorcount, int* warningcount);

pwr_tStatus wsx_CheckVolume(ldh_tSesContext sesctx, pwr_tObjid objid,
    int* errorcount, int* warningcount);

#if defined __cplusplus
}
#endif
#endif
