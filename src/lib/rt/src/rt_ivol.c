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

/* rt_ivol.c -- Initiate Volumes
   This module contains service routines to initiate volumes
   from loadfiles into the global PROVIEW/R database.  */

#include <string.h>

#include "rt_gdh_msg.h"
#include "rt_hash_msg.h"
#include "rt_ivol.h"
#include "rt_sub.h"
#include "co_time.h"

static pwr_tBoolean buildScObjects(pwr_tStatus* status, gdb_sVolume* vp);

static gdb_sObject* loadObject(pwr_tStatus* sts, gdb_sVolume* vp, char* name,
    pwr_tObjid oid, pwr_tClassId cid, int size, pwr_tObjid poid,
    pwr_tBitMask flags, pwr_tObjid soid);

static pwr_tBoolean mountClients(pwr_tStatus* sts, gdb_sVolume* vp);

static gdb_sVolume* mountVolume(pwr_tStatus* sts, gdb_sObject* op);

static pwr_tBoolean updateObject(ivol_sVolume*, ivol_sObject*);

/* Build ScObjects in a native volume.  */

static pwr_tBoolean buildScObjects(pwr_tStatus* status, gdb_sVolume* vp)
{
  pool_sQlink* scl;
  gdb_sScObject* scp;

  pwr_dStatus(sts, status, GDH__SUCCESS);

  /* Link all sc objects.  */

  for (scl = pool_Qsucc(sts, gdbroot->pool, &vp->u.n.sc_lh);
       scl != &vp->u.n.sc_lh; scl = pool_Qsucc(sts, gdbroot->pool, scl)) {
    scp = pool_Qitem(scl, gdb_sScObject, sc_ll);

    vol_LinkScObject(sts, vp, scp, vol_mLinkSc_build);
    pwr_Assert(ODD(*sts));
  }
  return YES;
}

/* .  */

static gdb_sObject* loadObject(pwr_tStatus* sts, gdb_sVolume* vp, char* name,
    pwr_tObjid oid, pwr_tClassId cid, int size, pwr_tObjid poid,
    pwr_tBitMask flags, pwr_tObjid soid)
{
  gdb_sObject* op;

  pwr_Assert(hash_Search(NULL, gdbroot->oid_ht, &oid) == NULL);

  op = gdb_AddObject(sts, name, oid, cid, size, poid, flags, soid);
  if (op == NULL)
    return NULL;
  op->u.n.flags.b.systemCreated = 1;
  return vol_LinkObject(sts, vp, op, vol_mLink_load);
}

/* Mount clients.  */

static pwr_tBoolean mountClients(pwr_tStatus* sts, gdb_sVolume* vp)
{
  pool_sQlink* ol;
  gdb_sObject* op;

  /* First link the volume block.  */

  /* Now link all objects.  */

  for (ol = pool_Qsucc(sts, gdbroot->pool, &vp->l.obj_lh); ol != &vp->l.obj_lh;
       ol = pool_Qsucc(sts, gdbroot->pool, ol)) {
    op = pool_Qitem(ol, gdb_sObject, l.obj_ll);

    if (op->g.flags.b.isMountClient && op->g.oid.vid == gdbroot->db->vid) {
      /* Only root volumes can mount.  */
      mountVolume(sts, op);
      if (*sts != GDH__NOMOUNTOBJECT)
        pwr_Assert(ODD(*sts));
    }
  }
  return YES;
}

/* Mount a volume.
   This should maybe be moved to vol_LinkObject.  */

static gdb_sVolume* mountVolume(pwr_tStatus* sts, gdb_sObject* op)
{
  gdb_sVolume* vp = NULL;
  gdb_sMountServer* msp;
  void* p;
  pwr_tObjid soid = pwr_cNObjid;
  gdb_sObject* vop;
  co_mFormat fm;
  pwr_tTime time;

  if (op->g.oid.vid != gdbroot->db->vid)
    errh_Bugcheck(GDH__WEIRD, "only root volumes can mount");

  p = pool_Address(sts, gdbroot->rtdb, op->u.n.body);
  if (p == NULL)
    return NULL;

  switch (op->g.cid) {
  case pwr_eClass_MountObject:
    soid = ((pwr_sMountObject*)p)->Object;
    if (cdh_ObjidIsNull(soid)) {
      *sts = GDH__NOMOUNTOBJECT;
      return NULL;
    }
    vp = vol_MountVolume(sts, soid.vid);
    break;
  case pwr_eClass_MountDynObject: {
    cdh_sParseName parseName, *pn;
    gdb_sObject* sop = NULL;

    pn = cdh_ParseName(sts, &parseName, pwr_cNObjid, ((pwr_sMountDynObject*)p)->Object, 0);
    if (pn == NULL)
      return NULL;

    sop = vol_NameToObject(sts, pn, gdb_mLo_global, vol_mTrans_all);
    if (sop == NULL || cdh_ObjidIsNull(sop->g.oid)) {
      *sts = GDH__NOMOUNTOBJECT;
      return NULL;      
    }

    soid = sop->g.oid;
    op->g.soid = soid;
    vp = vol_MountVolume(sts, soid.vid);
    break;
  }
  case pwr_eClass_MountVolume:
    soid.vid = ((pwr_sMountVolume*)p)->Volume;
    if (soid.vid == 0) {
      *sts = GDH__NOMOUNTOBJECT;
      return NULL;
    }
    vp = vol_MountVolume(sts, soid.vid);
    break;
  case pwr_eClass_CreateVolume:
    soid.vid = ((pwr_sCreateVolume*)p)->Volume;
    time_GetTime(&time);
    vp = gdb_LoadVolume(sts, soid.vid, op->g.f.name.orig,
        pwr_eClass_DynamicVolume, gdbroot->db->nid, time, gdb_mLoad_build,
        co_GetOwnFormat(&fm));

    /* Create the volume object.  */

    vop = loadObject(sts, vp, vp->g.name.orig, soid, pwr_eClass_DynamicVolume,
        sizeof(pwr_sDynamicVolume), pwr_cNObjid, net_mGo__, pwr_cNObjid);
    if (vop == NULL)
      errh_Bugcheck(*sts, "");
    break;
  }

  if (vp == NULL)
    return NULL;

  pwr_Assert(cdh_ObjidIsEqual(op->g.soid, soid));

  if (!op->u.n.flags.b.inMountClientList) {
    msp = vol_AddMountClient(sts, op);
    if (msp == NULL)
      return NULL;
  }

  return vp;
}

static pwr_tBoolean updateObject(ivol_sVolume* lv, ivol_sObject* iop)
{
  dbs_sObject* o = &iop->oh;
  gdb_sObject* op = iop->op;

  if (iop->flags.b.classid)
    op->g.cid = o->cid;
  if (iop->flags.m & gdb_mChange_family)
    cdh_Family(&op->g.f, o->name, o->poid);
  if (iop->flags.b.server)
    op->g.soid = o->soid;
  if (iop->flags.b.flags) {
    op->g.flags.b.isAliasClient = o->flags.b.isAliasClient;
    op->g.flags.b.isMountClient = o->flags.b.isMountClient;
  }

  if (iop->flags.b.size) {
    if (!op->g.flags.b.isAliasClient)
      op->g.size = o->rbody.size;
    else
      op->g.size = 0;
  }

  op->u.n.flags.b.isMountClean = o->flags.b.isMountClean;
  op->u.n.time = o->time;
  op->u.n.lflags.m = o->flags.m;

  return YES;
}

/* This routine builds all linkage between objects after
   the initial load is done.
   The database has to be locked by the caller.  */

void ivol_BuildNode(
    pwr_tStatus* status, ivol_sNode* lnp, const co_mFormat* formatp)
{
  gdb_sVolume* vp;
  pwr_sMountObject* MountObject;
  cdh_uVolumeId sys_vid;
  pwr_tObjid sys_oid;
  pwr_tObjid oid;
  gdb_sObject* op;
  gdb_sObject* vop;
  gdb_sObject* mop;
  pool_sQlink* vl;
  gdb_sClass* cp;
  pool_sQlink* cl;
  co_mFormat fm;
  pwr_tTime time;

  pwr_dStatus(sts, status, GDH__SUCCESS);

  /* Fill in remaining node information.  */

  gdbroot->db->nod_oid = lnp->nod_oid;

  gdbroot->my_node->nod_oid = gdbroot->db->nod_oid;
  gdbroot->my_node->vol_oid = gdbroot->db->vol_oid;

  /* Create the system volume and mount it in the root voulme.  */

  sys_vid.pwr = gdbroot->db->vid;
  sys_vid.v.vid_3 = cdh_eVid3_local;
  sys_oid.vid = sys_vid.pwr;
  sys_oid.oix = pwr_cNObjectIx;
  time_GetTime(&time);

  vp = gdb_LoadVolume(sts, sys_vid.pwr, "", pwr_eClass_SystemVolume,
      gdbroot->db->nid, time, gdb_mLoad_build, co_GetOwnFormat(&fm));
  if (vp == NULL)
    errh_Bugcheck(*sts, "");

  /* Create the volume object.  */

  vop = loadObject(sts, vp, vp->g.name.orig, sys_oid, pwr_eClass_SystemVolume,
      sizeof(pwr_sSystemVolume), pwr_cNObjid, net_mGo__, pwr_cNObjid);
  if (vop == NULL)
    errh_Bugcheck(*sts, "");
  vop->u.n.flags.b.bodyDecoded = 1;

  /* Create the 'pwrNode' object.  */

  oid = vol_Oid(sts, vp, pwr_eClass_NodeHier);
  op = loadObject(sts, vp, "pwrNode", oid, pwr_eClass_NodeHier,
      sizeof(pwr_sNodeHier), sys_oid, net_mGo__, pwr_cNObjid);
  if (op == NULL)
    errh_Bugcheck(*sts, "");
  op->u.n.flags.b.bodyDecoded = 1;

  errh_Info("Created pwrNode, oid: %s", cdh_ObjidToString(oid, 0));
  /* Create a mount object in the root volume, to mount the 'pwrNode' object. */

  pwr_Assert(gdbroot->my_volume != NULL);

  oid = vol_Oid(sts, gdbroot->my_volume, pwr_eClass_MountObject);
  mop = loadObject(sts, gdbroot->my_volume, "pwrNode", oid,
      pwr_eClass_MountObject, sizeof(pwr_sMountObject), gdbroot->db->vol_oid,
      net_mGo_isMountClient, op->g.oid);
  if (mop == NULL)
    errh_Bugcheck(*sts, "");
  mop->u.n.flags.b.bodyDecoded = 1;

  MountObject = pool_Address(NULL, gdbroot->rtdb, mop->u.n.body);
  strcpy(MountObject->Description, "Mounts the system volume object pwr_Node.");
  MountObject->Object = op->g.oid;

  /* Build all native volumes.  */

  for (vl = pool_Qsucc(sts, gdbroot->pool, &gdbroot->db->vol_lh);
       vl != &gdbroot->db->vol_lh; vl = pool_Qsucc(sts, gdbroot->pool, vl)) {
    vp = pool_Qitem(vl, gdb_sVolume, l.vol_ll);

    if (vp->l.flags.b.isNative)
      ivol_BuildVolume(sts, vp);
  }

  /* Link class definitions.  */

  for (cl = pool_Qsucc(sts, gdbroot->pool, &gdbroot->db->class_lh);
       cl != &gdbroot->db->class_lh;) {
    cp = pool_Qitem(cl, gdb_sClass, class_ll);
    /* NOTA BENE !! mvol_LinkClass will change the linkage.  */
    cl = pool_Qsucc(sts, gdbroot->pool, cl);

    mvol_LinkClass(sts, cp, gdb_mAdd__);
  }

  /* Link Sub classes to attributes.  */

  for (cl = pool_Qsucc(sts, gdbroot->pool, &gdbroot->db->class_lh);
       cl != &gdbroot->db->class_lh;) {
    cp = pool_Qitem(cl, gdb_sClass, class_ll);
    cl = pool_Qsucc(sts, gdbroot->pool, cl);

    if (cp->hasSc)
      mvol_LinkSubClassToAttribute(sts, cp);
  }

  /* Build ScObjects for native volumes.  */

  for (vl = pool_Qsucc(sts, gdbroot->pool, &gdbroot->db->vol_lh);
       vl != &gdbroot->db->vol_lh; vl = pool_Qsucc(sts, gdbroot->pool, vl)) {
    vp = pool_Qitem(vl, gdb_sVolume, l.vol_ll);

    if (vp->l.flags.b.isNative)
      buildScObjects(sts, vp);
  }

  /* Build class attribute tree */
  mvol_BuildCatt(sts);

  mountClients(sts, gdbroot->my_volume);
}

/* Build a volume.  */

pwr_tBoolean ivol_BuildVolume(pwr_tStatus* status, gdb_sVolume* vp)
{
  pool_sQlink* ol;
  gdb_sObject* op;

  pwr_dStatus(sts, status, GDH__SUCCESS);

  /* First link the volume block.  */

  /* Now link all objects.  */

  for (ol = pool_Qsucc(sts, gdbroot->pool, &vp->l.obj_lh); ol != &vp->l.obj_lh;
       ol = pool_Qsucc(sts, gdbroot->pool, ol)) {
    op = pool_Qitem(ol, gdb_sObject, l.obj_ll);

    vol_LinkObject(sts, vp, op, vol_mLink_build);
    pwr_Assert(ODD(*sts));
  }
  return YES;
}

/* .  */

ivol_sBody* ivol_GetBody(pwr_tStatus* status, pwr_tObjid oid, ivol_sBody* lb)
{
  static ivol_sBody body;

  pwr_dStatus(sts, status, GDH__SUCCESS);

  if (lb == NULL)
    lb = &body;

  lb->op = hash_Search(sts, gdbroot->oid_ht, &oid);
  if (!lb->op)
    return 0;
  lb->size = lb->op->g.size;
  lb->body = pool_Address(NULL, gdbroot->rtdb, lb->op->u.n.body);

  return lb;
}

/* Prepare all objects in a volume for a hot swap update.  */

pwr_tBoolean ivol_InitiateVolumeUpdate(pwr_tStatus* status, ivol_sVolume* lv)
{
  gdb_sVolume* vp = lv->vp;
  gdb_sObject* op;
  pool_sQlink* ol;

  pwr_dStatus(sts, status, GDH__SUCCESS);

  for (/* All the objects in the volume.  */
      ol = pool_Qsucc(sts, gdbroot->pool, &vp->l.obj_lh); ol != &vp->l.obj_lh;
      ol = pool_Qsucc(sts, gdbroot->pool, ol)) {
    if (ol == NULL)
      return NO;

    op = pool_Qitem(ol, gdb_sObject, l.obj_ll);

    op->u.n.flags.m |= gdb_mNo_swap;
  }

  return YES;
}

/* Loads an object into the object database.
   Used when initially populating the database, before
   the nethandler is actually running. No checks are done except for
   duplicate Objid. The only way to find an object is searching by
   Objid. When InitialLoadDone is called, the rest of the relations
   between the objects are put in place.  */

gdb_sObject* ivol_LoadObject(
    pwr_tStatus* status, ivol_sVolume* lv, dbs_sObject* o, pwr_tBitMask ilink)
{
  gdb_sObject* op;
  net_mGo flags;
  pwr_tObjid soid = pwr_cNObjid;

  pwr_dStatus(sts, status, GDH__SUCCESS);

  flags.m = 0;

  if (o->flags.m & dbs_mFlags_isClient) {
    soid = o->soid;
    flags.b.isAliasClient = o->flags.b.isAliasClient;
    flags.b.isMountClient = o->flags.b.isMountClient;
  }

  op = gdb_AddObject(
      sts, o->name, o->oid, o->cid, o->rbody.size, o->poid, flags.m, soid);
  if (op == NULL)
    return NULL;
  if (vol_LinkObject(sts, lv->vp, op, ilink) == NULL)
    return NULL;
  op->u.n.flags.b.isMountClean = o->flags.b.isMountClean;
  /*  op->u.n.rbTime = o->rbody.time; */
  op->u.n.time = o->time;
  op->u.n.lflags.m = o->flags.m;
  return op;
}

/* Loads an object into the object database.
   Used when initially populating the database, before
   the nethandler is actually running. No checks are done except for
   duplicate Objid. The only way to find an object is searching by
   Objid. When InitialLoadDone is called, the rest of the relations
   between the objects are put in place.  */

gdb_sScObject* ivol_LoadScObject(pwr_tStatus* status, ivol_sVolume* lv,
    dbs_sScObject* sc, pwr_tBitMask ilink)
{
  gdb_sScObject* scp;
  gdb_mSc flags;

  pwr_dStatus(sts, status, GDH__SUCCESS);

  flags.m = 0;
  flags.b.isArrayElem = sc->flags.b.isArrayElem;
  flags.b.hasSc = sc->flags.b.hasSubClass;

  scp = gdb_AddScObject(
      sts, sc->oid, sc->cid, sc->size, sc->poid, sc->aidx, sc->elem, flags);
  if (scp == NULL)
    return NULL;
  scp->lflags.m = sc->flags.m;
  if (vol_LinkScObject(sts, lv->vp, scp, ilink) == NULL)
    return NULL;
  //  scp->flags.b.isMountClean = sc->flags.b.isMountClean;
  //  scp->time = sc->time;

  return scp;
}

/* Loads  */

gdb_sVolume* ivol_LoadVolume(
    pwr_tStatus* status, dbs_sVolume* v, const co_mFormat* format)
{
  pwr_dStatus(sts, status, GDH__SUCCESS);

  return gdb_LoadVolume(sts, v->vid, v->name, v->cid, gdbroot->db->nid, v->time,
      gdb_mLoad_build, format);
}

/* Rebuild a volume after a hotswap update.  */

pwr_tBoolean ivol_RebuildVolume(
    pwr_tStatus* status, ivol_sVolume* lv, const co_mFormat* format)
{
  gdb_sVolume* vp = lv->vp;
  ivol_sObject* iop;
  lst_sEntry* iol;
  pool_sQlink* ol;
  gdb_sObject* op;

  pwr_dStatus(sts, status, GDH__SUCCESS);

  gdb_AssumeExcled;
  gdb_AssumeLocked;

  for (ol = pool_Qsucc(sts, gdbroot->pool, &vp->l.obj_lh);
       ol != &vp->l.obj_lh;) {
    op = pool_Qitem(ol, gdb_sObject, l.obj_ll);
    ol = pool_Qsucc(sts, gdbroot->pool, ol);

    if (op->u.n.flags.b.swapDelete && !op->u.n.flags.b.systemCreated) {
      vol_SetAlarmLevel(sts, op, 0); /* !!! TODO Remeber to take care of move
                                        also in dvol delete and move */
      vol_SetBlockLevel(sts, op, 0); /* !!! TODO Remeber to take care of move
                                        also in dvol delete and move */
    }
  }

  /* Unlink all modified objects.  */
  for (iop = lst_Succ(NULL, &lv->upd_lh, &iol); iop != NULL;
       iop = lst_Succ(NULL, iol, &iol)) {
    if (!(iop->flags.m & gdb_mChange_head))
      continue;
    vol_UnlinkObject(sts, vp, iop->op, iop->unlink.m);
    updateObject(lv, iop);
  }
  for (iop = lst_Succ(NULL, &lv->upd_io_lh, &iol); iop != NULL;
       iop = lst_Succ(NULL, iol, &iol)) {
    if (!(iop->flags.m & gdb_mChange_head))
      continue;
    vol_UnlinkObject(sts, vp, iop->op, iop->unlink.m);
    updateObject(lv, iop);
  }

  for (iop = lst_Succ(NULL, &lv->cre_lh, &iol); iop != NULL;
       iop = lst_Succ(NULL, iol, &iol)) {
    vol_LinkObject(sts, vp, iop->op, vol_mLink_loOidTab);
  }

  for (ol = pool_Qsucc(sts, gdbroot->pool, &vp->l.obj_lh);
       ol != &vp->l.obj_lh;) {
    op = pool_Qitem(ol, gdb_sObject, l.obj_ll);
    ol = pool_Qsucc(sts, gdbroot->pool, ol);

    if (op->u.n.flags.b.swapDelete && !op->u.n.flags.b.systemCreated) {
      vol_UnlinkObject(sts, vp, op, vol_mLink_swapDelete);
    }
  }

  /* Relink all new and modified objects.  */

  for (iop = lst_Succ(NULL, &lv->upd_lh, &iol); iop != NULL;
       iop = lst_Succ(NULL, iol, &iol)) {
    if (!(iop->flags.m & gdb_mChange_head))
      continue;
    vol_LinkObject(sts, vp, iop->op, iop->link.m);
  }
  for (iop = lst_Succ(NULL, &lv->upd_io_lh, &iol); iop != NULL;
       iop = lst_Succ(NULL, iol, &iol)) {
    if (!(iop->flags.m & gdb_mChange_head))
      continue;
    vol_LinkObject(sts, vp, iop->op, iop->link.m);
  }

  for (iop = lst_Succ(NULL, &lv->cre_lh, &iol); iop != NULL;
       iop = lst_Succ(NULL, iol, &iol)) {
    vol_LinkObject(sts, vp, iop->op, vol_mLink_swapBuild);
  }

  return YES;
}

pwr_tBoolean ivol_DecodeBody(pwr_tStatus* status, void* bp, gdb_sClass* cp)
{
  return TRUE;
}

void ivol_CopyBody(pwr_tStatus* status, void* fp, void* tp, gdb_sClass* cp)
{
  int i;
  gdb_sAttribute* ap;

  pwr_dStatus(sts, status, GDH__SUCCESS);

  for (i = 0, ap = cp->attr; i < cp->acount; i++, ap++) {
    if (ap->flags.b.state)
      continue;
    memcpy((char*)tp + ap->offs, (char*)fp + ap->offs, ap->size);
  }
}
