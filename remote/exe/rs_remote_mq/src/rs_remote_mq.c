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

/*************************************************************************
*		===============
*                P r o v i e w
*               ===============
**************************************************************************
*
* Filename:             rs_remote_mq.c
*
* Description:		Remote transport process for BEA Message Queue as a
*client
*			For further information, please refer to BEA Message
*Queue
*			documentation.
*
* Change log:		2005-11-24, Claes Jurstrand
*			First version introduced in 4.0.1-1
*
*
**************************************************************************
**************************************************************************/

/*_Include files_________________________________________________________*/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "pwr_systemclasses.h"
#include "rt_gdh.h"
#include "co_time.h"
#include "co_cdh.h"
#include "pwr_baseclasses.h"
#include "pwr_remoteclasses.h"
#include "rt_pwr_msg.h"
#include "rt_aproc.h"
#include "remote.h"
#include "remote_remtrans_utils.h"

// Message Q include files

#include "p_entry.h" /* PAMS function declarations     */
#include "p_proces.h" /* Known Queue number definitions */
#include "p_group.h" /* Known Group ID definitions     */
#include "p_typecl.h" /* Generic Type/Class definitions */
#include "p_return.h" /* PAMS return status definitions */
#include "p_symbol.h" /* Generic PSEL/PSYM definitions  */

#define TIME_INCR 0.02
#define debug 0

remnode_item rn;
pwr_sClass_RemnodeMQ* rn_mq;

/*************************************************************************
**************************************************************************
*
* RemoteSleep
*
**************************************************************************
**************************************************************************/

void RemoteSleep(float time)
{
  struct timespec rqtp, rmtp;

  rqtp.tv_sec = 0;
  rqtp.tv_nsec = (long int)(time * 1000000000);
  nanosleep(&rqtp, &rmtp);
  return;
}

/*************************************************************************
**************************************************************************
*
* Namn : bmq_receive
*
* Typ  : unsigned int
*
* Typ		Parameter	       IOGF	Beskrivning
*
* Beskrivning : Invoked when a MQ message is received.
*
**************************************************************************
**************************************************************************/

unsigned int bmq_receive()
{
  char buf[32767];
  char prio = 0;
  q_address source;
  int16 class, type, len, len_data;
  struct PSB psb;

  unsigned int sts;
  int32 mq_sts;
  char receive_force_j = PDEL_DEFAULT_JRN;

  char search_remtrans;
  remtrans_item* remtrans;

  len = sizeof(buf);

  sts = pams_get_msg((char*)&buf, &prio, &source, &class, &type, &len,
      &len_data, NULL, &psb, NULL, NULL, NULL, NULL, NULL);
  if (sts == PAMS__SUCCESS) {
    if (debug)
      printf("Received message %d\n", len_data);

    search_remtrans = true;

    remtrans = rn.remtrans;
    while (remtrans && search_remtrans) {
      if (remtrans->objp->Address[0] == class
          && remtrans->objp->Address[1] == type
          && remtrans->objp->Direction == REMTRANS_IN) {
        search_remtrans = false;
        sts = RemTrans_Receive(remtrans, (char*)&buf, len_data);
        if (sts != STATUS_OK && sts != STATUS_BUFF)
          errh_Error("Error from RemTrans_Receive, queue %d, status %d",
              rn_mq->MyQueue, sts, 0);
        break;
      }
      remtrans = (remtrans_item*)remtrans->next;
    }
    if (search_remtrans) {
      rn_mq->ErrCount++;
      errh_Info("No remtrans for received message, queue %d, class %d, type %d",
          rn_mq->MyQueue, class, type, 0);
    }

    if (psb.del_psb_status == PAMS__CONFIRMREQ
        || psb.del_psb_status == PAMS__POSSDUPL)
      pams_confirm_msg(psb.seq_number, &mq_sts, &receive_force_j);

  } else {
    if (sts != PAMS__NOMOREMSG) {
      rn_mq->ErrCount++;
      errh_Error(
          "Receive failed, queue %d, MQ status %d", rn_mq->MyQueue, sts, 0);
    }
  }

  return (sts);
}

/*************************************************************************
**************************************************************************
*
* Namn : bmq_send
*
* Typ  : unsigned int
*
* Typ		Parameter	       IOGF	Beskrivning
*
* Beskrivning : Sends a MQ message to Remote node
*
**************************************************************************
**************************************************************************/

unsigned int bmq_send(remnode_item* remnode, pwr_sClass_RemTrans* remtrans,
    char* buf, int buf_size)

{
  int32 mq_sts;

  char put_prio = 0;
  q_address dest;
  short class;
  short type;
  short msg_size;
  int32 timeout = 10;
  struct PSB put_psb;
  // Default delivery mode is WF_MEM & DISC, no recovery
  char delivery = PDEL_MODE_WF_MEM;
  char put_uma = PDEL_UMA_DISC;

  // Set destination group and queue
  dest.au.group = rn_mq->TargetGroup;
  dest.au.queue = rn_mq->TargetQueue;

  class = remtrans->Address[0];
  type = remtrans->Address[1];
  if (remtrans->Address[2] != 0)
    delivery = remtrans->Address[2];
  if (remtrans->Address[3] != 0)
    put_uma = remtrans->Address[3];

  msg_size = buf_size;

  mq_sts = pams_put_msg(buf, &put_prio, &dest, &class, &type, &delivery,
      &msg_size, &timeout, &put_psb, &put_uma, NULL, NULL, NULL, NULL);
  if (mq_sts == PAMS__SUCCESS) {
    if (put_psb.del_psb_status < 0) {
      if (put_psb.uma_psb_status < 0) {
        remtrans->ErrCount++;
        errh_Error(
            "Send failed, message not recoverable. Queue %d, UMA status %d",
            rn_mq->MyQueue, put_psb.uma_psb_status, 0);
      } else {
        errh_Error("Message not deliverable, UMA action taken. Queue %d, PSB "
                   "status %d",
            rn_mq->MyQueue, put_psb.del_psb_status, 0);
      }
    }
    if (debug)
      printf("Sent message sts:%d psb:%d uma:%d\n", (int)mq_sts,
          (int)put_psb.del_psb_status, (int)put_psb.uma_psb_status);
  } else {
    remtrans->ErrCount++;
    errh_Error(
        "Send failed, queue %d, MQ status %d", rn_mq->MyQueue, mq_sts, 0);
    if (debug)
      printf("Send failed sts:%d\n", (int)mq_sts);
  }

  return (STATUS_OK);
}

/*************************************************************************
**************************************************************************
*
* Main
*
**************************************************************************
**************************************************************************/

int main(int argc, char* argv[])
{
  remtrans_item* remtrans;
  char id[32];
  char pname[45];

  pwr_tStatus sts;
  int i;
  float time_since_scan = 0.0;

  q_address mq_queue_no;
  int32 mq_attach_mode;
  int32 mq_queue_type = PSYM_ATTACH_PQ;
  char queue_name[16];
  int32 queue_name_len;

  /* Read arg number 2, should be id for this instance and id is our queue
   * number */

  if (argc >= 2)
    strncpy(id, argv[1], sizeof(id));
  else
    strcpy(id, "0");

  /* Build process name with id */

  sprintf(pname, "rs_remmq_%s", id);

  /* Init of errh */

  errh_Init(pname, errh_eAnix_remote);
  errh_SetStatus(PWR__SRVSTARTUP);

  /* Init of gdh */
  if (debug)
    printf("Before gdh_init\n");
  sts = gdh_Init(pname);
  if (EVEN(sts)) {
    errh_Fatal("gdh_Init, %m", sts);
    errh_SetStatus(PWR__SRVTERM);
    exit(sts);
  }

  /* Arg number 3 should be my remnodes objid in string representation,
     read it, convert to real objid and store in remnode_item */

  sts = 0;
  if (argc >= 3)
    sts = cdh_StringToObjid(argv[2], &rn.objid);
  if (EVEN(sts)) {
    errh_Fatal("cdh_StringToObjid, %m", sts);
    errh_SetStatus(PWR__SRVTERM);
    exit(sts);
  }

  /* Get pointer to RemnodeMQ object and store locally */

  sts = gdh_ObjidToPointer(rn.objid, (pwr_tAddress*)&rn_mq);
  if (EVEN(sts)) {
    errh_Fatal("cdh_ObjidToPointer, %m", sts);
    errh_SetStatus(PWR__SRVTERM);
    exit(sts);
  }

  /* Initialize some internal data and make standard remtrans init */

  rn.next = NULL;
  rn.local = NULL; // We dont use local structure since we only have one remnode
  rn_mq->ErrCount = 0;
  if (debug)
    printf("Before remtrans_init\n");

  sts = RemTrans_Init(&rn);

  if (EVEN(sts)) {
    errh_Fatal("RemTrans_Init, %m", sts);
    errh_SetStatus(PWR__SRVTERM);
    exit(sts);
  }

  /* Store remtrans objects objid in remnode_mq object */
  remtrans = rn.remtrans;
  i = 0;
  while (remtrans) {
    rn_mq->RemTransObjects[i++] = remtrans->objid;
    if (i >= (int)(sizeof(rn_mq->RemTransObjects)
                 / sizeof(rn_mq->RemTransObjects[0])))
      break;
    remtrans = (remtrans_item*)remtrans->next;
  }

  /* Attach to Queue */

  mq_attach_mode = PSYM_ATTACH_TEMPORARY;
  queue_name[0] = 0;
  queue_name_len = 0;

  if (rn_mq->MyQueue > 0) {
    mq_attach_mode = PSYM_ATTACH_BY_NUMBER;
    sprintf(queue_name, "%d", rn_mq->MyQueue);
    queue_name_len = strlen(queue_name);
  }

  sts = pams_attach_q(&mq_attach_mode, &mq_queue_no, &mq_queue_type,
      (char*)&queue_name, &queue_name_len, (int32*)0, (int32*)0, NULL, NULL,
      NULL);

  if (sts != PAMS__SUCCESS && sts != PAMS__JOURNAL_ON) {
    errh_Fatal("pams_attach_q, %d", sts);
    errh_SetStatus(PWR__SRVTERM);
    exit(sts);
  }

  rn_mq->MyQueue = (pwr_tInt32)mq_queue_no.au.queue;

  /* Set running status */

  errh_SetStatus(PWR__SRUN);

  /* Set (re)start time in remnode object */

  time_GetTime(&rn_mq->RestartTime);

  /* Loop forever */

  while (!doomsday) {
    if (rn_mq->Disable == 1) {
      errh_Fatal("Disabled, exiting");
      errh_SetStatus(PWR__SRVTERM);
      exit(0);
    }
    aproc_TimeStamp(TIME_INCR, 5);
    RemoteSleep(TIME_INCR);

    time_since_scan += TIME_INCR;

    sts = bmq_receive();

    if (time_since_scan >= rn_mq->ScanTime) {
      sts = RemTrans_Cyclic(&rn, &bmq_send);
      time_since_scan = 0.0;
    }
  }
}
