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

/************************************************************************
* Description:
*	Application functions in rtt.
**************************************************************************/

/*_Include files_________________________________________________________*/
#include "pwr_inc:rt_rtt_menu.h"
#include "pwr_inc:rt_rtt_msg.h"

/*_Local rtt database____________________________________________________*/
RTT_DB_START
#include "pwrp_rtt:dtt_appl_RTTSYS_m.rdb1"
RTT_DB_CONTINUE
#include "pwrp_rtt:dtt_appl_RTTSYS_m.rdb2"
RTT_DB_END

/*************************************************************************
*
* Name:		RTTSYS_SHOW_NODES
*
* Type		int
*
* Type		Parameter	IOGF	Description
* menu_ctx	ctx		I	context of the picture.
* int		event		I 	type of event.
* char		*parameter_ptr	I	pointer to the parameter which value
*					has been changed.
*
* Description:
*	Application function of a picture.
*
**************************************************************************/

int RTTSYS_SHOW_NODES(ctx, event, parameter_ptr) menu_ctx ctx;
int event;
char* parameter_ptr;
{
  switch (event) {
  /**********************************************************
  *	Initialization of the picture
  ***********************************************************/
  case RTT_APPL_INIT:
    break;
  /**********************************************************
  *	Udate of the picture
  ***********************************************************/
  case RTT_APPL_UPDATE:
    break;
  /**********************************************************
  *	Exit of the picture
  ***********************************************************/
  case RTT_APPL_EXIT:
    break;
  /**********************************************************
  *	The value of a parameter is changed.
  ***********************************************************/
  case RTT_APPL_VALUECHANGED:
    break;
  /**********************************************************
  *	Next page key activated.
  ***********************************************************/
  case RTT_APPL_NEXTPAGE:
    break;
  /**********************************************************
  *	Pevious page key activated.
  ***********************************************************/
  case RTT_APPL_PREVPAGE:
    break;
  }
  return RTT__SUCCESS;
}
