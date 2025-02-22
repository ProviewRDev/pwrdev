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

#ifndef wb_trv_h
#define wb_trv_h

#include "wb_ldh.h"

typedef enum {
  trv_eDepth_Deep,
  trv_eDepth_Children,
  trv_eDepth_Self
} trv_eDepth;

#ifdef __cplusplus
typedef int (*trv_tBcFunc)(pwr_sAttrRef*, void*, void*, void*, void*, void*);
#else
typedef int (*trv_tBcFunc)();
#endif

typedef struct {
  ldh_tSesContext ldhses;
  pwr_tObjid hierobjid;
  pwr_tClassId* classid;
  char* name;
  pwr_tVolumeId* volume;
} trv_sCtx;
typedef trv_sCtx* trv_tCtx;

int trv_get_rtnodes(ldh_tSesContext ldhses, unsigned long* rtnode_count,
    pwr_tObjid** rtnodelist);

int trv_get_plcpgms(
    ldh_tSesContext ldhses, unsigned long* plc_count, pwr_tObjid** plclist);

int trv_get_plc_window(
    ldh_tSesContext ldhses, pwr_tObjid plc, pwr_tObjid* window);

int trv_get_window_windows(ldh_tSesContext ldhses, pwr_tObjid window,
    unsigned long* wind_count, pwr_tObjid** windlist);

int trv_get_window_objects(ldh_tSesContext ldhses, pwr_tObjid window,
    unsigned long* object_count, pwr_tObjid** objectlist);

int trv_get_window_connections(ldh_tSesContext ldhses, pwr_tObjid window,
    unsigned long* object_count, pwr_tObjid** objectlist);

int trv_get_parentlist(ldh_tSesContext ldhses, pwr_tObjid object,
    unsigned long* object_count, pwr_tObjid** objectlist);

int trv_get_objects_hier_class_name(ldh_tSesContext ldhses,
    pwr_tObjid hierobjdid, pwr_tClassId* classid, char* name,
    trv_tBcFunc backcall, void* arg1, void* arg2, void* arg3, void* arg4,
    void* arg5);

/* cpp call to trv_get_objects_hier_class_namn */
int trv_get_objects_hcn(ldh_tSesContext ldhses, pwr_tObjid hierobjdid,
    pwr_tClassId* classid, char* name,
    int (*backcall)(pwr_sAttrRef*, void*, void*, void*, void*, void*),
    void* arg1, void* arg2, void* arg3, void* arg4, void* arg5);

int trv_get_objects_class(ldh_tSesContext ldhses, pwr_tClassId classid,
    trv_tBcFunc backcall, void* arg1, void* arg2, void* arg3, void* arg4,
    void* arg5);

int trv_get_children_class_name(ldh_tSesContext ldhses, pwr_tObjid parentobjdid,
    pwr_tClassId* classid, char* name, trv_tBcFunc backcall, void* arg1,
    void* arg2, void* arg3, void* arg4, void* arg5);

int trv_get_class_hier(ldh_tSesContext ldhses, pwr_tObjid hierobjdid,
    char* name, pwr_tClassId* classid, trv_tBcFunc backcall, void* arg1,
    void* arg2, void* arg3, void* arg4, void* arg5);

int trv_get_docobjects(ldh_tSesContext ldhses, pwr_tObjid hierobjdid,
    trv_tBcFunc backcall, void* arg1, void* arg2, void* arg3, void* arg4,
    void* arg5);

int trv_object_search(trv_tCtx trvctx, trv_tBcFunc backcall, void* arg1,
    void* arg2, void* arg3, void* arg4, void* arg5);

int trv_create_ctx(trv_tCtx* trvctx, ldh_tSesContext ldhses,
    pwr_tObjid hierobjid, pwr_tClassId* classid, char* name,
    pwr_tVolumeId* volume);

int trv_delete_ctx(trv_tCtx trvctx);

int trv_get_attrobjects(ldh_tSesContext ldhses, pwr_tOid oid, pwr_tCid* cid,
    char* name, trv_eDepth depth, trv_tBcFunc backcall, void* arg1, void* arg2,
    void* arg3, void* arg4, void* arg5);

int trv_aobject_search(trv_tCtx trvctx, trv_tBcFunc backcall, void* arg1,
    void* arg2, void* arg3, void* arg4, void* arg5);
#endif
