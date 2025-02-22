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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <sys/file.h>
#include <sys/ioctl.h>

#include "pwr_profibusclasses.h"
#include "rt_io_base.h"
#include "rt_io_bus.h"
#include "rt_io_msg.h"
#include "rt_pb_msg.h"

#include "rt_io_pb_locals.h"

#include "keywords.h"

#pragma pack(1)
#include "pb_type.h"
#include "pb_conf.h"
#include "pb_if.h"
#include "pb_err.h"
#include "pb_fmb.h"
#include "pb_dp.h"
#include "pb_fdl.h"
#pragma pack(0)

static unsigned char req_res_buffer[512];

static short fdlif_sda_sdn_sdr_req(io_sAgentLocal* local_agent,
                                   pwr_sClass_Pb_FDL_SAP* sap,
                                   pwr_sClass_Pb_FDL_DataTransfer* op,
                                   io_sFDLCardLocal* local)

/*-----------------------------------------------------------------------------
    FUNCTIONAL_DESCRIPTION

    this function is used to activate a SAP

possible return values:

    - == E_OK    -> no error
    - != E_OK    -> error

    -----------------------------------------------------------------------------*/
{
  // LOCAL_VARIABLES

  short result; /* !!! local result variable !!! */

  // FUNCTION_BODY

  T_FDLIF_SDN_SDA_SRD_REQ* req = (T_FDLIF_SDN_SDA_SRD_REQ*)req_res_buffer;
  unsigned char* data_ptr;
  T_PROFI_SERVICE_DESCR sdb;

  /* --- set data block parameters -------------------------------------------
   */
  data_ptr = (USIGN8*)(req + 1);

  req->ssap = sap->SAP_Nr;
  req->dsap = op->DestSAP;
  req->rem_add = sap->SlaveAddress;
  req->priority = HIGH;

  memcpy(data_ptr, local->output_area, local->output_area_size);
  req->length = local->output_area_size;

  /* --- set parameter block parameters --------------------------------------
   */
  sdb.comm_ref = 0;
  sdb.layer = FDLIF;
  switch (op->Type)
  {
  case pwr_ePbFDLDataTransferTypeEnum_FDLIF_SDA:
  {
    sdb.service = FDLIF_SDA;
    break;
  }
  case pwr_ePbFDLDataTransferTypeEnum_FDLIF_SDN:
  {
    sdb.service = FDLIF_SDN;
    break;
  }
  case pwr_ePbFDLDataTransferTypeEnum_FDLIF_SRD:
  {
    sdb.service = FDLIF_SRD;
    break;
  }
  default:
    sdb.service = FDLIF_SDN;
    break;
  }

  sdb.primitive = REQ;
  sdb.invoke_id = local->invoke_id = local_agent->invoke_id;
  local_agent->invoke_id = (local_agent->invoke_id + 1) % 128;

  // result = profi_snd_req_res(
  //     (T_PROFI_DEVICE_HANDLE*)local_agent, &sdb, (void*)req, PB_FALSE);
  result = profi_snd_req_res(&sdb, (void*)req, PB_FALSE);

  return ((pwr_tBoolean)(result == E_OK));
}

static short fdlif_reply_update_mult_req(io_sAgentLocal* local_agent,
                                         pwr_sClass_Pb_FDL_SAP* sap,
                                         pwr_sClass_Pb_FDL_DataTransfer* op,
                                         io_sFDLCardLocal* local)

/*-----------------------------------------------------------------------------
    FUNCTIONAL_DESCRIPTION

    this function is used to activate a SAP

possible return values:

    - == E_OK    -> no error
    - != E_OK    -> error

    -----------------------------------------------------------------------------*/
{
  // LOCAL_VARIABLES

  INT16 result; /* !!! local result variable !!! */

  // FUNCTION_BODY

  T_FDLIF_RUP_REQ* req = (T_FDLIF_RUP_REQ*)req_res_buffer;
  USIGN8* data_ptr;
  T_PROFI_SERVICE_DESCR sdb;

  /* --- set data block parameters -------------------------------------------
   */
  data_ptr = (USIGN8*)(req + 1);

  req->sap_nr = sap->SAP_Nr;
  req->priority = HIGH;

  memcpy(data_ptr, local->output_area, local->output_area_size);
  req->length = local->output_area_size;

  /* --- set parameter block parameters --------------------------------------
   */
  sdb.comm_ref = 0;
  sdb.layer = FDLIF;
  sdb.service = FDLIF_REPLY_UPDATE_MULTIPLE;
  sdb.primitive = REQ;
  sdb.invoke_id = local->invoke_id = local_agent->invoke_id;
  local_agent->invoke_id = (local_agent->invoke_id + 1) % 128;

  // result = profi_snd_req_res((T_PROFI_DEVICE_HANDLE*)local_agent, &sdb,
  // (void*)req, PB_FALSE);
  result = profi_snd_req_res(&sdb, (void*)req, PB_FALSE);

  return ((pwr_tBoolean)(result == E_OK));
}

/*----------------------------------------------------------------------------*\
   Init method for the Pb module
\*----------------------------------------------------------------------------*/
static pwr_tStatus IoCardInit(io_tCtx ctx, io_sAgent* ap, io_sRack* rp,
                              io_sCard* cp)
{
  io_sFDLCardLocal* local;
  pwr_sClass_Pb_FDL_DataTransfer* op = (pwr_sClass_Pb_FDL_DataTransfer*)cp->op;
  unsigned int input_area_offset = 0;
  unsigned int input_area_chansize = 0;
  unsigned int output_area_offset = 0;
  unsigned int output_area_chansize = 0;

  local = (io_sFDLCardLocal*)calloc(1, sizeof(io_sFDLCardLocal));
  cp->Local = local;

  /* Initialize remote address structure */

  local->byte_ordering = ((pwr_sClass_Pb_FDL_SAP*)rp->op)->ByteOrdering;

  io_bus_card_init(ctx, cp, &input_area_offset, &input_area_chansize,
                   &output_area_offset, &output_area_chansize,
                   local->byte_ordering, io_eAlignment_Packed);

  local->input_area_size = input_area_offset + input_area_chansize;
  local->output_area_size = output_area_offset + output_area_chansize;

  if (local->input_area_size > 0)
    local->input_area = calloc(1, local->input_area_size);
  if (local->output_area_size > 0)
    local->output_area = calloc(1, local->output_area_size);

  errh_Info("PROFIBUS: Init of FDL Data transfer '%s'", cp->Name);

  op->Status = PB__NORMAL;

  return IO__SUCCESS;
}

/*----------------------------------------------------------------------------*\
   Read method for the Pb FDL Data transfer module
\*----------------------------------------------------------------------------*/
static pwr_tStatus IoCardRead(io_tCtx ctx, io_sAgent* ap, io_sRack* rp,
                              io_sCard* cp)
{
  io_sFDLCardLocal* local = (io_sFDLCardLocal*)cp->Local;

  io_bus_card_read(ctx, rp, cp, local->input_area, 0, local->byte_ordering,
                   local->float_representation);

  return IO__SUCCESS;
}

/*----------------------------------------------------------------------------*\
   Write method for the Pb  FDL Data transfer module
\*----------------------------------------------------------------------------*/
static pwr_tStatus IoCardWrite(io_tCtx ctx, io_sAgent* ap, io_sRack* rp,
                               io_sCard* cp)
{
  io_sAgentLocal* local_agent = (io_sAgentLocal*)ap->Local;
  pwr_sClass_Pb_FDL_DataTransfer* op = (pwr_sClass_Pb_FDL_DataTransfer*)cp->op;
  pwr_sClass_Pb_FDL_SAP* sap = (pwr_sClass_Pb_FDL_SAP*)rp->op;
  io_sFDLCardLocal* local = (io_sFDLCardLocal*)cp->Local;

  op->Status = sap->Status;

  if (op->Status == PB__NORMAL)
  {
    io_bus_card_write(ctx, cp, local->output_area, local->byte_ordering,
                      local->float_representation);

    if (op->SendReq)
    {
      pthread_mutex_lock(&local_agent->mutex);

      if (sap->Responder)
      {
        fdlif_reply_update_mult_req(local_agent, sap, op, local);
      }
      else
      {
        fdlif_sda_sdn_sdr_req(local_agent, sap, op, local);
      }
      op->SendReq = 0;

      pthread_mutex_unlock(&local_agent->mutex);
    }
  }
  return IO__SUCCESS;
}

/*----------------------------------------------------------------------------*\
   Close method for the Pb module
\*----------------------------------------------------------------------------*/
static pwr_tStatus IoCardClose(io_tCtx ctx, io_sAgent* ap, io_sRack* rp,
                               io_sCard* cp)
{
  io_sCardLocal* local;
  local = cp->Local;

  free((char*)local);

  printf("Method Pb_Module-IoCardClose\n");
  return IO__SUCCESS;
}

/*----------------------------------------------------------------------------*\
  Every method to be exported to the workbench should be registred here.
\*----------------------------------------------------------------------------*/

pwr_dExport pwr_BindIoMethods(Pb_FDL_DataTransfer) = {
    pwr_BindIoMethod(IoCardInit), pwr_BindIoMethod(IoCardRead),
    pwr_BindIoMethod(IoCardWrite), pwr_BindIoMethod(IoCardClose),
    pwr_NullMethod};
