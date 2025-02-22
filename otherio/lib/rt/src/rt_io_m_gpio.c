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

/* rt_io_m_gpio.c -- I/O methods for class GPIO. */

#include <stdio.h>
#include <stdlib.h>

#include "pwr_basecomponentclasses.h"
#include "pwr_otherioclasses.h"
#include "rt_io_base.h"
#include "rt_io_rack_init.h"
#include "rt_io_rack_close.h"
#include "rt_io_msg.h"
#include "rt_io_m_gpio.h"

static pwr_tStatus IoRackInit(io_tCtx ctx, io_sAgent* ap, io_sRack* rp)
{
  io_sCard* cp;
  pwr_sClass_GPIO* op = (pwr_sClass_GPIO*)rp->op;
  FILE* fp;
  pwr_tStatus sts;
  int i;

  fp = fopen("/sys/class/gpio/export", "w");
  if (!fp) {
    errh_Error("GPIO unable to open \"/sys/class/gpio/export\", %s", rp->Name);
    sts = IO__INITFAIL;
    op->Status = sts;
    return sts;
  }

  for (cp = rp->cardlist; cp; cp = cp->next) {
    for (i = 0; i < GPIO_MAX_CHANNELS; i++) {
      if (cp->chanlist[i].cop) {
        switch (cp->chanlist[i].ChanClass) {
        case pwr_cClass_ChanDi:
          fprintf(fp, "%u", ((pwr_sClass_ChanDi*)cp->chanlist[i].cop)->Number);
          fflush(fp);
          break;
        case pwr_cClass_ChanDo:
          fprintf(fp, "%u", ((pwr_sClass_ChanDo*)cp->chanlist[i].cop)->Number);
          fflush(fp);
          break;
        default:;
        }
      }
    }
  }
  fclose(fp);

  errh_Info("Init of GPIO '%s'", rp->Name);
  op->Status = IO__SUCCESS;

  return IO__SUCCESS;
}

static pwr_tStatus IoRackClose(io_tCtx ctx, io_sAgent* ap, io_sRack* rp)
{
  pwr_sClass_GPIO* op = (pwr_sClass_GPIO*)rp->op;

  op->Status = 0;
  return IO__SUCCESS;
}

/*  Every method should be registred here. */

pwr_dExport pwr_BindIoMethods(GPIO) = { pwr_BindIoMethod(IoRackInit),
  pwr_BindIoMethod(IoRackClose), pwr_NullMethod };
