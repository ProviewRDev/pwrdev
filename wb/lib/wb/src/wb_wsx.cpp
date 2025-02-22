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

/* wb_wsx.c -- wb syntax control
   Wb syntax control.
   This module control the syntax for objects.  */

#include <stdlib.h>
#include <string.h>

#include "pwr_baseclasses.h"
#include "pwr_systemclasses.h"
#include "co_msg.h"

#include "cow_msgwindow.h"

#include "wb_session.h"
#include "wb_trv.h"
#include "wb_wsx.h"
#include "wb_wsx_msg.h"

static int wsx_object_count(pwr_tAttrRef* aref, void* count, void* dum1,
    void* dum2, void* dum3, void* dum4);

/*_define _______________________________________________________*/

/*_Local procedues_______________________________________________________*/

static int wsx_object_count(pwr_tAttrRef* aref, void* count, void* dum1,
    void* dum2, void* dum3, void* dum4)
{
  (*(int*)count)++;
  return WSX__SUCCESS;
}

/*_Backcalls for the controlled gredit module______________________________*/

/*_Methods defined for this module_______________________________________*/

/*************************************************************************
*
* Name:		wsx_error_msg()
*
* Type		void
*
* Type		Parameter	IOGF	Description
*
* Description:
*	Prints a error or warning message for an object and increments the
*	errorcount or warningcount.
*
**************************************************************************/

pwr_tStatus wsx_error_msg(ldh_tSesContext sesctx, pwr_tStatus sts,
    pwr_tAttrRef aref, int* errorcount, int* warningcount)
{
  static char msg[256];
  int status, size;
  pwr_tAName name;
  char* namep;
  FILE* logfile;

  logfile = NULL;

  if (EVEN(sts)) {
    msg_GetMsg(sts, msg, sizeof(msg));

    if (logfile != NULL)
      fprintf(logfile, "%s\n", msg);
    else
      printf("%s\n", msg);
    if (cdh_ObjidIsNull(aref.Objid))
      MsgWindow::message(co_error(sts), 0, 0);
    else {
      /* Get the full hierarchy name for the node */
      status = ldh_AttrRefToName(sesctx, &aref, cdh_mNName, &namep, &size);
      if (EVEN(status))
        return status;

      strncpy(name, namep, sizeof(name));
      if (logfile != NULL)
        fprintf(logfile, "        in object  %s\n", name);
      else
        printf("        in object  %s\n", name);
      MsgWindow::message(co_error(sts), "   in object", name, aref.Objid);
    }
    if ((sts & 2) && !(sts & 1))
      (*errorcount)++;
    else if (!(sts & 2) && !(sts & 1))
      (*warningcount)++;
  }
  return WSX__SUCCESS;
}

pwr_tStatus wsx_error_msg_object(ldh_tSesContext sesctx, pwr_tStatus sts,
    pwr_tOid oid, int* errorcount, int* warningcount)
{
  static char msg[256];
  int status, size;
  pwr_tOName name;
  FILE* logfile;

  logfile = NULL;

  if (EVEN(sts)) {
    msg_GetMsg(sts, msg, sizeof(msg));

    if (logfile != NULL)
      fprintf(logfile, "%s\n", msg);
    else
      printf("%s\n", msg);
    if (cdh_ObjidIsNull(oid))
      MsgWindow::message(co_error(sts), 0, 0);
    else {
      /* Get the full hierarchy name for the node */
      status = ldh_ObjidToName(sesctx, oid, cdh_mName_pathStrict, name, 
          sizeof(name), &size);
      if (EVEN(status))
        return status;

      if (logfile != NULL)
        fprintf(logfile, "        in object  %s\n", name);
      else
        printf("        in object  %s\n", name);
      MsgWindow::message(co_error(sts), "   in object", name, oid);
    }
    if ((sts & 2) && !(sts & 1))
      (*errorcount)++;
    else if (!(sts & 2) && !(sts & 1))
      (*warningcount)++;
  }
  return WSX__SUCCESS;
}

pwr_tStatus wsx_error_msg_str(ldh_tSesContext sesctx, const char* str,
    pwr_tAttrRef aref, int severity, int* errorcount, int* warningcount)
{
  int status, size;
  pwr_tAName name;
  char* namep;
  FILE* logfile;
  char msg[200];

  logfile = NULL;

  snprintf(msg, sizeof(msg), "%%WSX-%c-MSG, %s", severity, str);
  if (logfile != NULL)
    fprintf(logfile, "%s\n", msg);
  else
    printf("%s\n", msg);

  if (cdh_ObjidIsNull(aref.Objid))
    MsgWindow::message(severity, msg, "", 0);
  else {
    /* Get the full hierarchy name for the node */
    status = ldh_AttrRefToName(sesctx, &aref, cdh_mNName, &namep, &size);
    if (EVEN(status))
      return status;

    strncpy(name, namep, sizeof(name));
    if (logfile != NULL)
      fprintf(logfile, "        in object  %s\n", name);
    else
      printf("        in object  %s\n", name);
    MsgWindow::message(severity, msg, "   in object", name, aref.Objid);
  }
  if (severity == 'E' || severity == 'F')
    (*errorcount)++;
  else if (severity == 'W')
    (*warningcount)++;

  return WSX__SUCCESS;
}

/*************************************************************************
*
* Name:		wsx_CheckCard()
*
* Type		int
*
* Type		Parameter	IOGF	Description
* ldh_tSesContext sesctx	I	ldh session context.
* pwr_tObjid	objid		I	card objid.
*
* Description:
*	Check the syntax of a card.
**************************************************************************/

pwr_tStatus wsx_CheckCard(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    int* errorcount, int* warningcount, wsx_mCardOption options)
{
  int sts;
  int size;
  char* buf_ptr;
  pwr_tUInt16 chan_max;
  int chan_number_array[256];
  int chan_count;
  pwr_tUInt16 number;
  pwr_tObjid chan_objid;
  pwr_tClassId cid;

  /* Check ErrorSoftLimit */
  sts = ldh_GetAttrObjectPar(
      sesctx, &aref, "RtBody", "ErrorSoftLimit", (char**)&buf_ptr, &size);
  if (EVEN(sts))
    return sts;
  if (*(int*)buf_ptr == 0)
    wsx_error_msg(sesctx, WSX__CARDERRSOFTLIM, aref, errorcount, warningcount);
  free((char*)buf_ptr);

  /* Check ErrorHardLimit */
  sts = ldh_GetAttrObjectPar(
      sesctx, &aref, "RtBody", "ErrorHardLimit", (char**)&buf_ptr, &size);
  if (EVEN(sts))
    return sts;
  if (*(int*)buf_ptr == 0)
    wsx_error_msg(sesctx, WSX__CARDERRHARDLIM, aref, errorcount, warningcount);
  free((char*)buf_ptr);

  /* Get MaxNoOfChannels */
  sts = ldh_GetAttrObjectPar(
      sesctx, &aref, "RtBody", "MaxNoOfChannels", (char**)&buf_ptr, &size);
  if (EVEN(sts))
    return sts;

  chan_max = *(pwr_tUInt16*)buf_ptr;
  free((char*)buf_ptr);

  if (chan_max > 256) {
    wsx_error_msg(sesctx, WSX__MAXCHAN, aref, errorcount, warningcount);
    return WSX__SUCCESS;
  }
  /* Check that Number in channel-objects are unique within the card */
  memset(chan_number_array, 0, sizeof(chan_number_array));
  chan_count = 0;
  sts = ldh_GetChild(sesctx, aref.Objid, &chan_objid);
  while (ODD(sts)) {
    sts = ldh_GetObjectClass(sesctx, chan_objid, &cid);
    if (EVEN(sts))
      return sts;
    switch (cid) {
    case pwr_cClass_ChanDi:
    case pwr_cClass_ChanDo:
    case pwr_cClass_ChanAi:
    case pwr_cClass_ChanAit:
    case pwr_cClass_ChanAo:
      chan_count++;
      sts = ldh_GetObjectPar(
          sesctx, chan_objid, "RtBody", "Number", (char**)&buf_ptr, &size);
      if (EVEN(sts))
        return sts;
      number = *(pwr_tUInt16*)buf_ptr;
      free((char*)buf_ptr);
      /* Check than number is within limits */
      if (number >= chan_max) {
        wsx_error_msg(sesctx, WSX__NUMRANGE, cdh_ObjidToAref(chan_objid),
            errorcount, warningcount);
        break;
      }
      if (chan_number_array[number])
        /* Number is occupied */
        wsx_error_msg(sesctx, WSX__NUMNOTUNIQUE, cdh_ObjidToAref(chan_objid),
            errorcount, warningcount);
      else
        chan_number_array[number] = 1;
      break;
    default:
      wsx_error_msg(sesctx, WSX__MISPLACED, aref, errorcount, warningcount);
    }
    sts = ldh_GetNextSibling(sesctx, chan_objid, &chan_objid);
  }
  if (chan_count > chan_max)
    wsx_error_msg(sesctx, WSX__CHANCOUNT, aref, errorcount, warningcount);

  return WSX__SUCCESS;
}

/*************************************************************************
*
* Name:		wsx_CheckCoCard()
*
* Type		int
*
* Type		Parameter	IOGF	Description
* ldh_tSesContext sesctx	I	ldh session context.
* pwr_tObjid	objid		I	card objid.
*
* Description:
*	Check the syntax of a co card.
**************************************************************************/

pwr_tStatus wsx_CheckCoCard(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    int* errorcount, int* warningcount)
{
  int sts;
  int size;
  char* buf_ptr;
  pwr_tUInt16 chan_max;
  int chan_count;
  pwr_tObjid chan_objid;
  pwr_tClassId cid;

  /* Check ErrorSoftLimit */
  sts = ldh_GetAttrObjectPar(
      sesctx, &aref, "RtBody", "ErrorSoftLimit", (char**)&buf_ptr, &size);
  if (EVEN(sts))
    return sts;
  if (*(int*)buf_ptr == 0)
    wsx_error_msg(sesctx, WSX__CARDERRSOFTLIM, aref, errorcount, warningcount);
  free((char*)buf_ptr);

  /* Check ErrorHardLimit */
  sts = ldh_GetAttrObjectPar(
      sesctx, &aref, "RtBody", "ErrorHardLimit", (char**)&buf_ptr, &size);
  if (EVEN(sts))
    return sts;
  if (*(int*)buf_ptr == 0)
    wsx_error_msg(sesctx, WSX__CARDERRHARDLIM, aref, errorcount, warningcount);
  free((char*)buf_ptr);

  /* Get MaxNoOfChannels */
  sts = ldh_GetAttrObjectPar(
      sesctx, &aref, "RtBody", "MaxNoOfCounters", (char**)&buf_ptr, &size);
  if (EVEN(sts))
    return sts;

  chan_max = *(pwr_tUInt16*)buf_ptr;
  free((char*)buf_ptr);

  chan_count = 0;
  sts = ldh_GetChild(sesctx, aref.Objid, &chan_objid);
  while (ODD(sts)) {
    sts = ldh_GetObjectClass(sesctx, chan_objid, &cid);
    if (EVEN(sts))
      return sts;
    switch (cid) {
    case pwr_cClass_ChanCo:
      chan_count++;
      break;
    default:
      wsx_error_msg(sesctx, WSX__MISPLACED, aref, errorcount, warningcount);
    }
    sts = ldh_GetNextSibling(sesctx, chan_objid, &chan_objid);
  }
  if (chan_count > chan_max)
    wsx_error_msg(sesctx, WSX__CHANCOUNT, aref, errorcount, warningcount);

  return WSX__SUCCESS;
}

pwr_tStatus wsx_CheckIoDevice(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    int* errorcount, int* warningcount, wsx_mCardOption options)
{
  wb_session* sp = (wb_session*)sesctx;
  pwr_tMask process;
  pwr_tOid thread;

  wb_attribute a = sp->attribute(&aref);
  if (!a)
    return a.sts();

  // Check Process
  wb_attribute process_a(a, 0, "Process");
  if (!process_a)
    return process_a.sts();

  process_a.value(&process);
  if (!process_a)
    return process_a.sts();
  if (process == 0)
    wsx_error_msg_str(sesctx, "Process is not specified", aref, 'W', errorcount,
        warningcount);
  else if (process == 1) {
    // Check thread object
    wb_attribute thread_a(a, 0, "ThreadObject");
    if (!thread_a)
      return thread_a.sts();

    thread_a.value(&thread);
    if (!thread_a)
      return thread_a.sts();
    if (cdh_ObjidIsNull(thread))
      wsx_error_msg_str(sesctx, "ThreadObject is not specified", aref, 'E',
          errorcount, warningcount);
    else {
      wb_object thread_o = sp->object(thread);
      if (!thread_o)
        wsx_error_msg_str(sesctx, "Undefined ThreadObject", aref, 'E',
            errorcount, warningcount);
      else if (thread_o.cid() != pwr_cClass_PlcThread)
        wsx_error_msg_str(sesctx, "Error in ThreadObject class", aref, 'E',
            errorcount, warningcount);
    }
  }

  if (options & wsx_mCardOption_ErrorLimits) {
    pwr_tUInt32 limit;

    // Check SoftLimit
    wb_attribute softlimit_a(a, 0, "ErrorSoftLimit");
    if (!softlimit_a)
      return softlimit_a.sts();

    softlimit_a.value(&limit);
    if (!softlimit_a)
      return softlimit_a.sts();
    if (limit == 0)
      wsx_error_msg_str(sesctx, "ErrorSoftLimit is not specified", aref, 'W',
          errorcount, warningcount);

    // Check HardLimit
    wb_attribute hardlimit_a(a, 0, "ErrorHardLimit");
    if (!hardlimit_a)
      return hardlimit_a.sts();

    hardlimit_a.value(&limit);
    if (!hardlimit_a)
      return hardlimit_a.sts();
    if (limit == 0)
      wsx_error_msg_str(sesctx, "ErrorHardLimit is not specified", aref, 'E',
          errorcount, warningcount);
  }
  return WSX__SUCCESS;
}

/*************************************************************************
*
* Name:		wsx_CheckSigChanCon()
*
* Type		int
*
* Type		Parameter	IOGF	Description
* ldh_tSesContext sesctx	I	ldh session context.
* pwr_tObjid	objid		I	card objid.
*
* Description:
*	Check SigChanCon in a signal or a channel.
**************************************************************************/

pwr_tStatus wsx_CheckSigChanCon(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    int* errorcount, int* warningcount)
{
  int sts;
  int size;
  pwr_tAttrRef* con_ptr;
  int class_error;
  pwr_tClassId cid;
  pwr_tClassId con_class;

  sts = ldh_GetAttrRefTid(sesctx, &aref, &cid);
  if (EVEN(sts))
    return sts;

  /* Check SigChanCon */
  sts = ldh_GetAttrObjectPar(
      sesctx, &aref, "RtBody", "SigChanCon", (char**)&con_ptr, &size);
  if (EVEN(sts))
    return sts;

  if (cdh_ObjidIsNull(con_ptr->Objid)) {
    wsx_error_msg(sesctx, WSX__SIGCHANCON, aref, errorcount, warningcount);
    free((char*)con_ptr);
    return WSX__SUCCESS;
  }

  /* Check object class of connected object */
  sts = ldh_GetAttrRefTid(sesctx, con_ptr, &con_class);
  if (EVEN(sts)) {
    wsx_error_msg(sesctx, WSX__SIGCHANCON, aref, errorcount, warningcount);
    free((char*)con_ptr);
    return WSX__SUCCESS;
  }
  class_error = 0;
  switch (cid) {
  case pwr_cClass_Di:
    if (con_class != pwr_cClass_ChanDi)
      class_error = 1;
    break;
  case pwr_cClass_Do:
    if (con_class != pwr_cClass_ChanDo)
      class_error = 1;
    break;
  case pwr_cClass_Po:
    if (con_class != pwr_cClass_ChanDo)
      class_error = 1;
    break;
  case pwr_cClass_Ai:
    if (!(con_class == pwr_cClass_ChanAi || con_class == pwr_cClass_ChanAit))
      class_error = 1;
    break;
  case pwr_cClass_Ao:
    if (con_class != pwr_cClass_ChanAo)
      class_error = 1;
    break;
  case pwr_cClass_Co:
    if (con_class != pwr_cClass_ChanCo)
      class_error = 1;
    break;
  case pwr_cClass_Io:
    if (con_class != pwr_cClass_ChanIo)
      class_error = 1;
    break;
  case pwr_cClass_Ii:
    if (con_class != pwr_cClass_ChanIi)
      class_error = 1;
    break;
  case pwr_cClass_Eo:
    if (con_class != pwr_cClass_ChanEo)
      class_error = 1;
    break;
  case pwr_cClass_Ei:
    if (con_class != pwr_cClass_ChanEi)
      class_error = 1;
    break;
  case pwr_cClass_So:
    if (con_class != pwr_cClass_ChanSo)
      class_error = 1;
    break;
  case pwr_cClass_Si:
    if (con_class != pwr_cClass_ChanSi)
      class_error = 1;
    break;
  case pwr_cClass_Bi:
    if (con_class != pwr_cClass_ChanBi)
      class_error = 1;
    break;
  case pwr_cClass_Bo:
    if (con_class != pwr_cClass_ChanBo)
      class_error = 1;
    break;
  }

  if (class_error) {
    wsx_error_msg(sesctx, WSX__SIGCHANCONCLASS, aref, errorcount, warningcount);
    free((char*)con_ptr);
    return WSX__SUCCESS;
  }

  free((char*)con_ptr);
  return WSX__SUCCESS;
}

//
// Check if an attrref attribute contains an invalid attrref
//

pwr_tStatus wsx_CheckAttrRef(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    const pwr_tObjName attribute, pwr_tCid* cid_vect, int null_is_ok,
    int* errorcount, int* warningcount)
{
  pwr_tAttrRef value;
  wb_session* sp = (wb_session*)sesctx;

  wb_attribute a = sp->attribute(&aref);
  if (!a)
    return a.sts();

  wb_attribute a_attr(a, 0, attribute);
  if (!a_attr)
    return a_attr.sts();

  a_attr.value(&value);
  if (!a_attr)
    return a_attr.sts();

  if (!null_is_ok && cdh_ObjidIsNull(value.Objid)) {
    char msg[80];
    sprintf(msg, "Attribute reference is null in \"%s\"", attribute);
    wsx_error_msg_str(sesctx, msg, aref, 'E', errorcount, warningcount);
  }
  if (cdh_ObjidIsNotNull(value.Objid)) {
    wb_attribute a_value = sp->attribute(&value);
    if (!a_value) {
      char msg[80];
      sprintf(msg, "Undefined attribute reference in \"%s\"", attribute);
      wsx_error_msg_str(sesctx, msg, aref, 'E', errorcount, warningcount);
    } else if (cid_vect) {
      // Check attribute reference class
      bool found = false;
      for (int i = 0; cid_vect[i]; i++) {
        if (cid_vect[i] == a_value.tid()) {
          found = true;
          break;
        }
      }
      if (!found) {
        char msg[80];
        sprintf(
            msg, "Invalid class of attribute reference in \"%s\"", attribute);
        wsx_error_msg_str(sesctx, msg, aref, 'E', errorcount, warningcount);
      }
    }
  }
  return WSX__SUCCESS;
}

//
// Check if an Z attrref attribute contains an invalid attrref and points back
//

pwr_tStatus wsx_CheckXAttrRef(ldh_tSesContext sesctx, pwr_tAttrRef aref,
    const pwr_tObjName attribute, const pwr_tObjName back_attribute,
    pwr_tCid* cid_vect, int null_is_ok, int* errorcount, int* warningcount)
{
  pwr_tAttrRef value;
  pwr_tAttrRef back_aref;
  wb_session* sp = (wb_session*)sesctx;

  wb_attribute a = sp->attribute(&aref);
  if (!a)
    return a.sts();

  wb_attribute a_attr(a, 0, attribute);
  if (!a_attr)
    return a_attr.sts();

  a_attr.value(&value);
  if (!a_attr)
    return a_attr.sts();

  if (!null_is_ok && cdh_ObjidIsNull(value.Objid)) {
    char msg[80];
    sprintf(msg, "Attribute reference is null in \"%s\"", attribute);
    wsx_error_msg_str(sesctx, msg, aref, 'E', errorcount, warningcount);
  }
  if (cdh_ObjidIsNotNull(value.Objid)) {
    wb_attribute a_value = sp->attribute(&value);
    if (!a_value) {
      char msg[80];
      sprintf(msg, "Undefined attribute reference in \"%s\"", attribute);
      wsx_error_msg_str(sesctx, msg, aref, 'E', errorcount, warningcount);
      return WSX__SUCCESS;
    }
    if (cid_vect) {
      // Check attribute reference class
      bool found = false;
      for (int i = 0; cid_vect[i]; i++) {
        if (cid_vect[i] == a_value.tid()) {
          found = true;
          break;
        }
      }
      if (!found) {
        char msg[80];
        sprintf(
            msg, "Invalid class of attribute reference in \"%s\"", attribute);
        wsx_error_msg_str(sesctx, msg, aref, 'E', errorcount, warningcount);
        return WSX__SUCCESS;
      }
    }

    // Check back attrref
    wb_attribute a_back(a_value, 0, back_attribute);
    if (!a_back)
      return a_back.sts();

    a_back.value(&back_aref);
    if (!a_back)
      return a_back.sts();

    if (!(cdh_ObjidIsEqual(back_aref.Objid, aref.Objid)
            && back_aref.Offset == aref.Offset)) {
      char msg[80];
      sprintf(msg, "Reference is not mutual \"%s\"", attribute);
      wsx_error_msg_str(sesctx, msg, aref, 'E', errorcount, warningcount);
      return WSX__SUCCESS;
    }
  }
  return WSX__SUCCESS;
}

/*************************************************************************
*
* Name:		wsx_CheckVolume()
*
* Type		int
*
* Type		Parameter	IOGF	Description
* ldh_tSesContext sesctx	I	ldh session context.
* pwr_tObjid	objid		I	card objid.
*
* Description:
*	Check the syntax of a volume.
**************************************************************************/

pwr_tStatus wsx_CheckVolume(ldh_tSesContext sesctx, pwr_tObjid objid,
    int* errorcount, int* warningcount)
{
  pwr_tStatus sts;
  pwr_tUInt32* opsys_ptr;
  pwr_tUInt32 opsys;
  pwr_tUInt32 opsys_sum;
  int size;
  int i;
  pwr_tClassId cid;
  pwr_tClassId class_vect[2];
  int node_object_count;

  sts = ldh_GetObjectClass(sesctx, objid, &cid);
  if (EVEN(sts))
    return sts;

  /* Check number of $Node objects in the volume */
  class_vect[0] = pwr_cClass_Node;
  class_vect[1] = 0;
  node_object_count = 0;
  sts = trv_get_objects_hier_class_name(sesctx, pwr_cNObjid, class_vect, NULL,
      &wsx_object_count, &node_object_count, 0, 0, 0, 0);
  if (EVEN(sts))
    return sts;

  switch (cid) {
  case pwr_cClass_RootVolume:
  case pwr_cClass_SubVolume:

    if (cid == pwr_cClass_RootVolume) {
      if (node_object_count != 1)
        wsx_error_msg(sesctx, WSX__NODECOUNT, cdh_ObjidToAref(objid),
            errorcount, warningcount);
    } else {
      if (node_object_count != 0)
        wsx_error_msg(sesctx, WSX__NODECOUNT, cdh_ObjidToAref(objid),
            errorcount, warningcount);
    }

    /* Check OperatingSystem */
    sts = ldh_GetObjectPar(
        sesctx, objid, "SysBody", "OperatingSystem", (char**)&opsys_ptr, &size);
    if (EVEN(sts))
      return sts;

    opsys_sum = 0;
    for (i = 0;; i++) {
      opsys = 1 << i;
      opsys_sum |= opsys;
      if (opsys >= pwr_mOpSys_)
        break;
    }

    if (*opsys_ptr & ~opsys_sum)
      wsx_error_msg(sesctx, WSX__OSINVALID, cdh_ObjidToAref(objid), errorcount,
          warningcount);

    free((char*)opsys_ptr);
    break;

  default:
    if (node_object_count != 0)
      wsx_error_msg(sesctx, WSX__NODECOUNT, cdh_ObjidToAref(objid), errorcount,
          warningcount);
  }
  return WSX__SUCCESS;
}
