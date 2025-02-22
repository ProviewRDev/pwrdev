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

/* rt_io_m_mb_rtu_master.c -- io methods for the Modbus RTU Master object
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#if defined OS_LINUX
#include <termio.h>
#endif
#if defined OS_LINUX || defined OS_MACOS
#include <sgtty.h>
#endif
#include <sys/ioctl.h>

#include "co_cdh.h"
#include "pwr_basecomponentclasses.h"
#include "pwr_otherioclasses.h"
#include "rt_io_base.h"
#include "rt_io_bus.h"
#include "rt_io_msg.h"
#include "co_cdh.h"
#include "co_time.h"
#include "rt_mb_msg.h"
#include "rt_io_mb_rtu.h"
#include "rt_io_agent_init.h"

static pwr_tStatus IoAgentInit(io_tCtx ctx, io_sAgent* ap);
static pwr_tStatus IoAgentRead(io_tCtx ctx, io_sAgent* ap);
static pwr_tStatus IoAgentWrite(io_tCtx ctx, io_sAgent* ap);
static pwr_tStatus IoAgentClose(io_tCtx ctx, io_sAgent* ap);

static void float_to_timeval(struct timeval* tv, float t)
{
  tv->tv_sec = t;
  tv->tv_usec = (t - (float)tv->tv_sec) * 1000000;
}

static void float_to_timespec(struct timespec* tv, float t)
{
  tv->tv_sec = t;
  tv->tv_nsec = (t - (float)tv->tv_sec) * 1000000000;
}

/*----------------------------------------------------------------------------*\
   Init method for the Modbus RTU Master agent
\*----------------------------------------------------------------------------*/
static pwr_tStatus IoAgentInit(io_tCtx ctx, io_sAgent* ap)
{
  struct termios tty_attributes;
  int sts;
  io_sAgentLocal* local;
  pwr_sClass_Modbus_RTU_Master* op = (pwr_sClass_Modbus_RTU_Master*)ap->op;

  /* Allocate area for local data structure */
  ap->Local = calloc(1, sizeof(io_sAgentLocal));
  local = ap->Local;

  if (op->Disable)
    return IO__SUCCESS;

  local->fd = open(op->Device, O_RDWR | O_NDELAY | O_NOCTTY);
  if (local->fd == -1) {
    errh_Error("Modbus RTU Master, open device error, %s, errno %d", ap->Name, errno);
    return IO__ERRINIDEVICE;
  }

  tcgetattr(local->fd, &tty_attributes);
  tty_attributes.c_cc[VMIN] = 1;
  tty_attributes.c_cc[VTIME] = 0;
  tty_attributes.c_lflag &= ~(ICANON | ISIG | ECHO | IEXTEN);
  tty_attributes.c_cflag |= (CLOCAL | CREAD);
  tty_attributes.c_oflag &= ~(OPOST);
  tty_attributes.c_oflag &= ~(ONLCR);
  tty_attributes.c_iflag &= ~(INLCR | ICRNL);

/* Speed */

#if defined OS_LINUX
  tty_attributes.c_cflag &= ~CBAUD;
#endif
  switch (op->Speed) {
  case 300:
    tty_attributes.c_cflag |= B300;
    break;
  case 1200:
    tty_attributes.c_cflag |= B1200;
    break;
  case 2400:
    tty_attributes.c_cflag |= B2400;
    break;
  case 4800:
    tty_attributes.c_cflag |= B4800;
    break;
  case 9600:
    tty_attributes.c_cflag |= B9600;
    break;
  case 19200:
    tty_attributes.c_cflag |= B19200;
    break;
  case 38400:
    tty_attributes.c_cflag |= B38400;
    break;
  case 57600:
    tty_attributes.c_cflag |= B57600;
    break;
  case 115200:
    tty_attributes.c_cflag |= B115200;
    break;
  default:
    errh_Error("Modbus RTU Master, unsupported speed, %s", ap->Name);
    tty_attributes.c_cflag |= B9600;
    break;
  }

  /* DataBits 5, 6, 7 or 8 */
  tty_attributes.c_cflag &= ~CSIZE;
  switch (op->DataBits) {
  case pwr_eDataBitsEnum_5:
    tty_attributes.c_cflag |= CS5;
    break;
  case pwr_eDataBitsEnum_6:
    tty_attributes.c_cflag |= CS6;
    break;
  case pwr_eDataBitsEnum_7:
    tty_attributes.c_cflag |= CS7;
    break;
  case pwr_eDataBitsEnum_8:
    tty_attributes.c_cflag |= CS8;
    break;
  default:
    errh_Error("Modbus RTU Master, unsupported DataBits, %s", ap->Name);
    tty_attributes.c_cflag |= CS8;
  }
  // tty_attributes.c_iflag |=ISTRIP;

  /* Parity */
  switch (op->Parity) {
  case pwr_eParityEnum_Odd:
  case pwr_eParityEnum_Even:
    tty_attributes.c_cflag |= PARENB;
    tty_attributes.c_iflag |= IGNPAR;
    if (op->Parity == pwr_eParityEnum_Even)
      tty_attributes.c_cflag &= ~PARODD;
    else
      tty_attributes.c_cflag |= PARODD;
    break;
  default:
    tty_attributes.c_cflag &= ~PARENB;
  }

  /* stopbitsval */

  switch (op->StopBits) {
  case pwr_eStopBitsEnum_2:
    tty_attributes.c_cflag |= CSTOPB;
    break;
  case pwr_eStopBitsEnum_1:
    tty_attributes.c_cflag &= ~CSTOPB;
    break;
  case pwr_eStopBitsEnum_0:
    errh_Error("Modbus RTU Master, unsupported StopBits, %s", ap->Name);
    tty_attributes.c_cflag &= ~CSTOPB;
    break;
  }

  tty_attributes.c_iflag &= ~IXON; // ingen XON/XOFF in

  // tty_attributes.c_iflag &= (V_IGNCR);
  // tty_attributes.c_iflag &= (IGNPAR | V_IGNCR);
  // tty_attributes.c_iflag &= ~(BRKINT | IXON | V_INLCR | V_ICRNL);
  // tty_attributes.c_cflag &= ~(CSIZE | CSTOPB | PARENB);  //fippla om v�rden
  // lite granna

  sts = tcsetattr(local->fd, TCSANOW, &tty_attributes);

  if (sts < 0) {
    errh_Error("Modbus RTU Master, set device attributes error, %s", ap->Name);
    return IO__ERRINIDEVICE;
  }

  tcflush(local->fd, TCIOFLUSH);
  // Test
  sleep(2);

  local->initialized = TRUE;

  return IO__SUCCESS;
}

static void generate_crc(unsigned char* buf, int size, unsigned char* result)
{
  unsigned short int crc;
  unsigned short int gen_polynomial = 0xA001;
  unsigned short int flag_mask = 0x0001;
  unsigned short int flag;
  int i, j;

  crc = 0xFFFF;

  for (i = 0; i < size; i++) {
    crc = crc ^ buf[i];

    for (j = 0; j < 8; j++) {
      flag = crc & flag_mask;
      crc = crc >> 1;
      if (flag)
        crc = crc ^ gen_polynomial;
    }
  }

  result[0] = (unsigned char)(crc & 0x00FF);
  result[1] = (unsigned char)((crc >> 8) & 0x00FF);
}

static pwr_tStatus rtu_send(io_sRack* rp, io_sAgentLocal* local_master,
    io_sRackLocal* local_slave, io_sCardLocalMsg* local_card,
    pwr_sClass_Modbus_RTU_Master* masterp, pwr_sClass_Modbus_RTU_Slave* slavep,
    pwr_sClass_Modbus_RTU_Module* modulep, unsigned char* buf, int buffer_size)
{
  int sts;
  rec_buf* rb;
  unsigned char fc;
  int data_size = 0;
  unsigned char telegram[512];
  unsigned char crc[2];
  fd_set read_fd;
  struct timeval tv;

  generate_crc(buf, buffer_size, &buf[buffer_size]);

  if (masterp->Debug) {
    int i;
    pwr_tTime current;
    char timstr[40];
    time_GetTime(&current);
    time_AtoAscii(&current, time_eFormat_Time, timstr, sizeof(timstr));
    printf("Snd: %s %2d  ", timstr, buffer_size + 2);
    for (i = 0; i < buffer_size + 2; i++)
      printf("%02d ", buf[i]);
    printf("\n");
  }

  sts = write(local_master->fd, buf, buffer_size + 2);
  if (sts <= 0) {
    slavep->ErrorCount++;
    return 0;
  }

  slavep->TX_packets++;

  /* Receive answer */

  sts = 1;

  float_to_timeval(&tv, masterp->ReceiveTimeout);

  FD_ZERO(&read_fd);
  FD_SET(local_master->fd, &read_fd);
  sts = select(local_master->fd + 1, &read_fd, NULL, NULL, &tv);
  if (sts == 0)
    return 0;

  sts = read(local_master->fd, telegram, 1);

  if (sts <= 0) {
    if (masterp->Debug)
      printf("Rcv: Nothing to read\n");
    return 0;
  }

  while (sts > 0) {
    data_size++;

    float_to_timeval(&tv, masterp->CharTimeout);

    FD_ZERO(&read_fd);
    FD_SET(local_master->fd, &read_fd);
    sts = select(local_master->fd + 1, &read_fd, NULL, NULL, &tv);
    if (sts == 0) {
      break;
    }
    sts = read(local_master->fd, telegram + data_size, 1);
  }

  if (data_size < 2) {
    if (masterp->Debug)
      printf("Rcv: Data size < 2\n");
    return 0;
  }

  if (masterp->Debug) {
    int i;
    pwr_tTime current;
    char timstr[40];
    time_GetTime(&current);
    time_AtoAscii(&current, time_eFormat_Time, timstr, sizeof(timstr));
    printf("Rcv: %s %2d  ", timstr, data_size);
    for (i = 0; i < data_size; i++)
      printf("%02d ", telegram[i]);
    printf("\n");
  }

  generate_crc(telegram, data_size - 2, crc);
  if (crc[0] != telegram[data_size - 2] || crc[1] != telegram[data_size - 1]) {
    slavep->ErrorCount++;
    return 0;
  }

  slavep->RX_packets++;

  rb = (rec_buf*)telegram;

  fc = rb->fc;

  if (fc != modulep->FunctionCode) {
    return 0;
  }

  slavep->Status = MB__NORMAL;
  modulep->Status = pwr_eModbusModule_StatusEnum_OK;

  switch (fc) {
  case pwr_eModbus_FCEnum_ReadCoils: {
    res_read* res_r;
    res_r = (res_read*)rb;
    memcpy(local_card->input_area, res_r->buf,
        MIN(res_r->bc, local_card->input_size));
    break;
  }

  case pwr_eModbus_FCEnum_ReadDiscreteInputs: {
    res_read* res_r;
    res_r = (res_read*)rb;
    memcpy(local_card->input_area, res_r->buf,
        MIN(res_r->bc, local_card->input_size));
    break;
  }

  case pwr_eModbus_FCEnum_ReadHoldingRegisters: {
    res_read* res_r;
    res_r = (res_read*)rb;
    memcpy(local_card->input_area, res_r->buf,
        MIN(res_r->bc, local_card->input_size));
    break;
  }

  case pwr_eModbus_FCEnum_ReadInputRegisters: {
    res_read* res_r;
    res_r = (res_read*)rb;
    memcpy(local_card->input_area, res_r->buf,
        MIN(res_r->bc, local_card->input_size));
    break;
  }

  case pwr_eModbus_FCEnum_WriteMultipleCoils:
  case pwr_eModbus_FCEnum_WriteMultipleRegisters:
  case pwr_eModbus_FCEnum_WriteSingleRegister:
    // Nothing good to do here
    break;
  }

  return IO__SUCCESS;
}

static pwr_tStatus mb_rtu_send_data(io_sRack* rp, io_sAgentLocal* local_master,
    io_sRackLocal* local_slave, pwr_sClass_Modbus_RTU_Master* masterp,
    pwr_sClass_Modbus_RTU_Slave* slavep, mb_tSendMask mask)
{
  io_sCardLocalMsg* local_card;
  io_sCard* cardp;
  pwr_sClass_Modbus_RTU_Module* modulep = NULL;
  pwr_tStatus sts;
  pwr_tCid cid;
  int modules;
  int i;
  int send_error;
  struct timespec tf;

  /* Send messages to slave */

  cardp = rp->cardlist;

  while (cardp) {
    cid = cardp->Class;
    while (ODD(gdh_GetSuperClass(cid, &cid, cardp->Objid)))
      ;

    switch (cid) {
    case pwr_cClass_Modbus_RTU_Module:
      modulep = (pwr_sClass_Modbus_RTU_Module*)cardp->op;
      modules = 1;
      break;
    default:
      modules = 0;
    }

    if (!modules) {
      cardp = cardp->next;
      continue;
    }

    send_error = 0;

    for (i = 0; i < modules; i++) {
      sts = 1;

      if (!modulep->Continuous && !modulep->SendOp) {
        break;
      }

      local_card = &((io_sCardLocal*)cardp->Local)->msg[i];
      if (modulep->ScanInterval > 1 && local_card->interval_cnt != 0) {
        modulep++;
        continue;
      }

      if (mask & mb_mSendMask_ReadReq) {
        switch (modulep->FunctionCode) {
        case pwr_eModbus_FCEnum_ReadCoils:
        case pwr_eModbus_FCEnum_ReadDiscreteInputs: {
          read_req rr;

          modulep->SendOp = FALSE;

          rr.unit_id = modulep->UnitId;
          rr.fc = modulep->FunctionCode;
          rr.addr = htons(modulep->Address);
          rr.quant = htons(local_card->no_di);
          //              rr.quant = ntohs(local_card->input_size * 8);

          sts = rtu_send(rp, local_master, local_slave, local_card, masterp,
              slavep, modulep, (unsigned char*)&rr, sizeof(read_req) - 2);
          if (EVEN(sts)) {
            slavep->Status = MB__CONNDOWN;
            slavep->ErrorCount++;
            send_error = 1;
            break;
          }
          slavep->Status = MB__NORMAL;

          break;
        }

        case pwr_eModbus_FCEnum_ReadHoldingRegisters:
        case pwr_eModbus_FCEnum_ReadInputRegisters: {
          read_req rr;

          modulep->SendOp = FALSE;

          rr.unit_id = modulep->UnitId;
          rr.fc = modulep->FunctionCode;
          rr.addr = htons(modulep->Address);
          rr.quant = ntohs((local_card->input_size + 1) / 2);

          sts = rtu_send(rp, local_master, local_slave, local_card, masterp,
              slavep, modulep, (unsigned char*)&rr, sizeof(read_req) - 2);
          if (EVEN(sts)) {
            slavep->Status = MB__CONNDOWN;
            slavep->ErrorCount++;
            send_error = 1;
            break;
          }
          slavep->Status = MB__NORMAL;
          break;
        }
        } /* End - switch FC ... */
      }

      if (mask & mb_mSendMask_WriteReq) {
        switch (modulep->FunctionCode) {
        case pwr_eModbus_FCEnum_WriteSingleCoil: {
          write_single_req wsr;

          modulep->SendOp = FALSE;

          wsr.unit_id = modulep->UnitId;
          wsr.fc = modulep->FunctionCode;
          wsr.addr = htons(modulep->Address);
          if (local_card->output_size == 4) {
            if (*(int*)local_card->output_area)
              wsr.value = ntohs(0xFF00);
            else
              wsr.value = 0;
          } else if (local_card->output_size == 2) {
            if (*(short int*)local_card->output_area)
              wsr.value = ntohs(0xFF00);
            else
              wsr.value = 0;
          } else if (local_card->output_size == 1) {
            if (*(char*)local_card->output_area)
              wsr.value = ntohs(0xFF00);
            else
              wsr.value = 0;
          } else
            wsr.value = 0;

          sts = rtu_send(rp, local_master, local_slave, local_card, masterp,
              slavep, modulep, (unsigned char*)&wsr, sizeof(wsr) - 2);
          if (EVEN(sts)) {
            slavep->Status = MB__CONNDOWN;
            slavep->ErrorCount++;
            send_error = 1;
            break;
          }
          slavep->Status = MB__NORMAL;
          break;
        }

        case pwr_eModbus_FCEnum_WriteMultipleCoils: {
          write_coils_req wcr;

          modulep->SendOp = FALSE;

          wcr.unit_id = modulep->UnitId;
          wcr.fc = modulep->FunctionCode;
          wcr.addr = htons(modulep->Address);
          wcr.quant = htons(local_card->no_do);
          //              wcr.quant = ntohs((local_card->output_size) * 8);
          wcr.bc = local_card->output_size;
          memcpy(wcr.reg, local_card->output_area, local_card->output_size);

          sts = rtu_send(rp, local_master, local_slave, local_card, masterp,
              slavep, modulep, (unsigned char*)&wcr,
              sizeof(wcr) - 2 - sizeof(wcr.reg) + local_card->output_size);
          if (EVEN(sts)) {
            slavep->Status = MB__CONNDOWN;
            slavep->ErrorCount++;
            send_error = 1;
            break;
          }
          slavep->Status = MB__NORMAL;
          break;
        }

        case pwr_eModbus_FCEnum_WriteMultipleRegisters: {
          write_reg_req wrr;

          modulep->SendOp = FALSE;

          wrr.unit_id = modulep->UnitId;
          wrr.fc = modulep->FunctionCode;
          wrr.addr = htons(modulep->Address);
          wrr.quant = ntohs((local_card->output_size) / 2);
          wrr.bc = local_card->output_size;
          memcpy(wrr.reg, local_card->output_area, local_card->output_size);

          sts = rtu_send(rp, local_master, local_slave, local_card, masterp,
              slavep, modulep, (unsigned char*)&wrr,
              sizeof(wrr) - 2 - sizeof(wrr.reg) + local_card->output_size);
          if (EVEN(sts)) {
            slavep->Status = MB__CONNDOWN;
            slavep->ErrorCount++;
            send_error = 1;
            break;
          }
          slavep->Status = MB__NORMAL;
          break;
        }
        case pwr_eModbus_FCEnum_WriteSingleRegister: {
          write_single_req wrr;

          modulep->SendOp = FALSE;

          wrr.unit_id = modulep->UnitId;
          wrr.fc = modulep->FunctionCode;
          wrr.addr = htons(modulep->Address);
          memcpy(&wrr.value, local_card->output_area, sizeof(wrr.value));

          sts = rtu_send(rp, local_master, local_slave, local_card, masterp,
              slavep, modulep, (unsigned char*)&wrr, sizeof(wrr) - 2);
          if (EVEN(sts)) {
            slavep->Status = MB__CONNDOWN;
            slavep->ErrorCount++;
            send_error = 1;
            break;
          }
          slavep->Status = MB__NORMAL;
          break;
        }
        } /* End - switch FC ... */
      }
      if (send_error && cardp->next) {
        /* Time to next frame */
        float_to_timespec(&tf, masterp->FrameTimeout - masterp->CharTimeout);
        nanosleep(&tf, NULL);
        break;
      }
      if (!(i == modules - 1 && !cardp->next)) {
        float_to_timespec(&tf, masterp->FrameTimeout - masterp->CharTimeout);
        nanosleep(&tf, NULL);
      }

      modulep++;
    }

    cardp = cardp->next;

  } /* End - while cardp ... */

  return IO__SUCCESS;
}

/*----------------------------------------------------------------------------*\
   Read method for the Modbus RTU Master agent
\*----------------------------------------------------------------------------*/
static pwr_tStatus IoAgentRead(io_tCtx ctx, io_sAgent* ap)
{
  io_sAgentLocal* local;
  io_sRackLocal* local_rack;
  pwr_tUInt16 sts;
  io_sRack* rp;
  pwr_tCid cid;
  pwr_sClass_Modbus_RTU_Slave* sp;
  pwr_sClass_Modbus_RTU_Master* op = (pwr_sClass_Modbus_RTU_Master*)ap->op;

  local = (io_sAgentLocal*)ap->Local;

  if (!local->initialized)
    return IO__SUCCESS;

  rp = ap->racklist;

  while (rp) {
    cid = rp->Class;
    while (ODD(gdh_GetSuperClass(cid, &cid, rp->Objid)))
      ;

    switch (cid) {
    case pwr_cClass_Modbus_RTU_Slave:

      sp = (pwr_sClass_Modbus_RTU_Slave*)rp->op;
      local_rack = rp->Local;

      /* Request new data */
      if (/* sp->Status == MB__NORMAL && */ sp->DisableSlave != 1) {
        sts = mb_rtu_send_data(
            rp, local, local_rack, op, sp, mb_mSendMask_ReadReq);

        if (sp->ErrorCount >= sp->ErrorLimit) {
          switch (sp->StallAction) {
          case pwr_eStallActionEnum_EmergencyBreak:
            ctx->Node->EmergBreakTrue = 1;
            break;
          case pwr_eStallActionEnum_ResetInputs:
            memset(sp->Inputs, 0, local_rack->input_size);
            local_rack->reset_inputs = 1;
            break;
          default:;
          }
        } else
          local_rack->reset_inputs = 0;
      }
      break;
    }

    rp = rp->next;
  }

  return IO__SUCCESS;
}

/*----------------------------------------------------------------------------*\
   Write method for the Modbus RTU Master agent
\*----------------------------------------------------------------------------*/
static pwr_tStatus IoAgentWrite(io_tCtx ctx, io_sAgent* ap)
{
  io_sAgentLocal* local;
  io_sRackLocal* local_rack;
  pwr_tUInt16 sts;
  io_sRack* rp;
  pwr_tCid cid;
  pwr_sClass_Modbus_RTU_Slave* sp;
  pwr_sClass_Modbus_RTU_Master* op = (pwr_sClass_Modbus_RTU_Master*)ap->op;

  local = (io_sAgentLocal*)ap->Local;

  if (!local->initialized)
    return IO__SUCCESS;

  rp = ap->racklist;

  while (rp) {
    cid = rp->Class;
    while (ODD(gdh_GetSuperClass(cid, &cid, rp->Objid)))
      ;

    switch (cid) {
    case pwr_cClass_Modbus_RTU_Slave:

      sp = (pwr_sClass_Modbus_RTU_Slave*)rp->op;
      local_rack = rp->Local;

      /* Request new data */
      if (/* sp->Status == MB__NORMAL && */ sp->DisableSlave != 1) {
        sts = mb_rtu_send_data(
            rp, local, local_rack, op, sp, mb_mSendMask_WriteReq);
      }
      break;
    }

    rp = rp->next;
  }

  return IO__SUCCESS;
}

/*----------------------------------------------------------------------------*\

\*----------------------------------------------------------------------------*/
static pwr_tStatus IoAgentClose(io_tCtx ctx, io_sAgent* ap)
{
  io_sAgentLocal* local = (io_sAgentLocal*)ap->Local;

  close(local->fd);
  return IO__SUCCESS;
}

/*----------------------------------------------------------------------------*\
  Every method to be exported to the workbench should be registred here.
\*----------------------------------------------------------------------------*/

pwr_dExport pwr_BindIoMethods(Modbus_RTU_Master)
    = { pwr_BindIoMethod(IoAgentInit), pwr_BindIoMethod(IoAgentRead),
        pwr_BindIoMethod(IoAgentWrite), pwr_BindIoMethod(IoAgentClose),
        pwr_NullMethod };
