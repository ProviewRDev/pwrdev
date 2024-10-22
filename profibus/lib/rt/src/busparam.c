/******************************************************************************
*                                                                             *
*                      SOFTING Industrial Automation GmbH                     *
*                          Richard-Reitzner-Allee 6                           *
*                                D-85540 Haar                                 *
*                        Phone: (++49)-(0)89-45656-0                          *
*                        Fax:   (++49)-(0)89-45656-399                        *
*                                                                             *
*            Copyright (C) SOFTING Industrial Automation GmbH 1995-2012       *
*                              All Rights Reserved                            *
*                                                                             *
*******************************************************************************

FILE_NAME               BUSPARAM.C

PROJECT_NAME            PROFIBUS

MODULE                  BUSPARAM

COMPONENT_LIBRARY       PBT LIBs and DLL  (16-Bit)
                        PBTOOLS DLL       (32-Bit)

AUTHOR                  SOFTING

VERSION                 5.21.0.00.release

DATE                    February-1998

STATUS                  finished

FUNCTIONAL_MODULE_DESCRIPTION




RELATED_DOCUMENTS
=============================================================================*/
#include "keywords.h"

INCLUDES

#include <stdlib.h>
#include <string.h>

#if defined(WIN16) || defined(WIN32)
#include <windows.h>
#endif

#include "pb_type.h"
#include "pb_dp.h"
#include "pb_fm7.h"
#include "pb_fdl.h"
#include "pb_fmb.h"
#include "pb_tools.h"

LOCAL_DEFINES

#define HSA 126 /* highest station address */

LOCAL_TYPEDEFS

FUNCTION_DECLARATIONS

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA

#if defined(WIN32) || defined(_WIN32) || defined(WIN16) || defined(_WIN16)
#pragma check_stack(off)
#endif

FUNCTION LOCAL PB_BOOL pbt_get_bus_param_for_dp(
    IN USIGN8 baud_rate, OUT T_FMB_SET_BUSPARAMETER_REQ* bus_param_ptr)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to get the default busparameter for DP operation
using ASPC2

possible return values:
- PB_TRUE   parameters correctly set
- PB_FALSE  no parameter set available, no parameters set
----------------------------------------------------------------------------*/
{
  LOCAL_VARIABLES

  FUNCTION_BODY

  switch (baud_rate)
  {
  case 0: /* 9,6 KBaud */
  case 1: /* 19,2 KBaud */
  case 2: /* 93,75 KBaud */
  case 3: /* 187,5 KBaud */
    bus_param_ptr->tsl = 100;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 60;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 1;
    bus_param_ptr->g = 1;
    bus_param_ptr->ttr = 50000;
    bus_param_ptr->max_retry_limit = 1;
    return (PB_TRUE);

  case 4: /* 500 KBaud */
    bus_param_ptr->tsl = 200;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 100;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 1;
    bus_param_ptr->g = 1;
    bus_param_ptr->ttr = 100000;
    bus_param_ptr->max_retry_limit = 1;
    return (PB_TRUE);

  case 6: /* 1,5 MBaud */
    bus_param_ptr->tsl = 300;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 150;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 1;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 300000;
    bus_param_ptr->max_retry_limit = 1;
    return (PB_TRUE);

  case 7: /* 3 MBaud */
    bus_param_ptr->tsl = 400;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 250;
    bus_param_ptr->tqui = 3;
    bus_param_ptr->tset = 4;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 600000;
    bus_param_ptr->max_retry_limit = 2;
    return (PB_TRUE);

  case 8: /* 6 MBaud */
    bus_param_ptr->tsl = 600;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 450;
    bus_param_ptr->tqui = 6;
    bus_param_ptr->tset = 8;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 1200000;
    bus_param_ptr->max_retry_limit = 3;
    return (PB_TRUE);

  case 9: /* 12 MBaud */
    bus_param_ptr->tsl = 1000;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 800;
    bus_param_ptr->tqui = 9;
    bus_param_ptr->tset = 16;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 2400000;
    bus_param_ptr->max_retry_limit = 4;
    return (PB_TRUE);

  case 11: /* 45,45 Kbaud */
    bus_param_ptr->tsl = 640;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 400;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 95;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 50000;
    bus_param_ptr->max_retry_limit = 1;
    return (PB_TRUE);

  default:
    return (PB_FALSE);
  }
}

FUNCTION LOCAL PB_BOOL pbt_get_bus_param_for_fms(
    IN USIGN8 baud_rate, OUT T_FMB_SET_BUSPARAMETER_REQ* bus_param_ptr)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to get the default busparameter for FMS/FM7 operation
using ASPC2

possible return values:
- PB_TRUE   parameters correctly set
- PB_FALSE  no parameter set available, no parameters set
----------------------------------------------------------------------------*/
{
  LOCAL_VARIABLES

  FUNCTION_BODY

  switch (baud_rate)
  {
  case 0: /* 9,6 KBaud */
    bus_param_ptr->tsl = 100;
    bus_param_ptr->min_tsdr = 30;
    bus_param_ptr->max_tsdr = 50;
    bus_param_ptr->tqui = 22;
    bus_param_ptr->tset = 5;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 10000;
    return (PB_TRUE);

  case 1: /* 19,2 KBaud */
    bus_param_ptr->tsl = 200;
    bus_param_ptr->min_tsdr = 60;
    bus_param_ptr->max_tsdr = 100;
    bus_param_ptr->tqui = 22;
    bus_param_ptr->tset = 10;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 15000;
    return (PB_TRUE);

  case 2: /* 93,75 KBaud */
    bus_param_ptr->tsl = 500;
    bus_param_ptr->min_tsdr = 125;
    bus_param_ptr->max_tsdr = 250;
    bus_param_ptr->tqui = 22;
    bus_param_ptr->tset = 15;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 30000;
    return (PB_TRUE);

  case 3: /* 187,5 KBaud */
    bus_param_ptr->tsl = 1000;
    bus_param_ptr->min_tsdr = 250;
    bus_param_ptr->max_tsdr = 500;
    bus_param_ptr->tqui = 22;
    bus_param_ptr->tset = 25;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 50000;
    return (PB_TRUE);

  case 4: /* 500 KBaud */
    bus_param_ptr->tsl = 2000;
    bus_param_ptr->min_tsdr = 500;
    bus_param_ptr->max_tsdr = 1000;
    bus_param_ptr->tqui = 22;
    bus_param_ptr->tset = 50;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 100000;
    return (PB_TRUE);

  case 6: /* 1,5 MBaud */
    bus_param_ptr->tsl = 3000;
    bus_param_ptr->min_tsdr = 150;
    bus_param_ptr->max_tsdr = 980;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 240;
    bus_param_ptr->g = 10;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 300000;
    return (PB_TRUE);

  case 7: /* 3 MBaud */
    bus_param_ptr->tsl = 400;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 250;
    bus_param_ptr->tqui = 3;
    bus_param_ptr->tset = 4;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 600000;
    bus_param_ptr->max_retry_limit = 2;
    return (PB_TRUE);

  case 8: /* 6 MBaud */
    bus_param_ptr->tsl = 600;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 450;
    bus_param_ptr->tqui = 6;
    bus_param_ptr->tset = 8;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 1200000;
    bus_param_ptr->max_retry_limit = 3;
    return (PB_TRUE);

  case 9: /* 12 MBaud */
    bus_param_ptr->tsl = 1000;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 800;
    bus_param_ptr->tqui = 9;
    bus_param_ptr->tset = 16;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 2400000;
    bus_param_ptr->max_retry_limit = 4;
    return (PB_TRUE);

  default:
    return (PB_FALSE);
  }
}

FUNCTION LOCAL PB_BOOL pbt_get_bus_param_for_dpfms(
    IN USIGN8 baud_rate, OUT T_FMB_SET_BUSPARAMETER_REQ* bus_param_ptr)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to get the default busparameter for mixed DP/FMS operation
using ASPC2

possible return values:
- PB_TRUE   parameters correctly set
- PB_FALSE  no parameter set available, no parameters set
----------------------------------------------------------------------------*/
{
  LOCAL_VARIABLES

  FUNCTION_BODY

  switch (baud_rate)
  {
  case 0: /* 9,6 KBaud */
    bus_param_ptr->tsl = 125;
    bus_param_ptr->min_tsdr = 30;
    bus_param_ptr->max_tsdr = 60;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 1;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 10000;
    return (PB_TRUE);

  case 1: /* 19,2 KBaud */
    bus_param_ptr->tsl = 250;
    bus_param_ptr->min_tsdr = 60;
    bus_param_ptr->max_tsdr = 120;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 1;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 15000;
    return (PB_TRUE);

  case 2: /* 93,75 KBaud */
    bus_param_ptr->tsl = 600;
    bus_param_ptr->min_tsdr = 125;
    bus_param_ptr->max_tsdr = 250;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 1;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 30000;
    return (PB_TRUE);

  case 3: /* 187,5 KBaud */
    bus_param_ptr->tsl = 1500;
    bus_param_ptr->min_tsdr = 250;
    bus_param_ptr->max_tsdr = 500;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 1;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 2;
    bus_param_ptr->ttr = 50000;
    return (PB_TRUE);

  case 4: /* 500 KBaud */
    bus_param_ptr->tsl = 3500;
    bus_param_ptr->min_tsdr = 500;
    bus_param_ptr->max_tsdr = 1000;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 1;
    bus_param_ptr->g = 1;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 100000;
    return (PB_TRUE);

  case 6: /* 1,5 MBaud */
    bus_param_ptr->tsl = 3000;
    bus_param_ptr->min_tsdr = 150;
    bus_param_ptr->max_tsdr = 980;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 240;
    bus_param_ptr->g = 10;
    bus_param_ptr->max_retry_limit = 1;
    bus_param_ptr->ttr = 300000;
    return (PB_TRUE);

  case 7: /* 3 MBaud */
    bus_param_ptr->tsl = 400;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 250;
    bus_param_ptr->tqui = 3;
    bus_param_ptr->tset = 4;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 600000;
    bus_param_ptr->max_retry_limit = 2;
    return (PB_TRUE);

  case 8: /* 6 MBaud */
    bus_param_ptr->tsl = 600;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 450;
    bus_param_ptr->tqui = 6;
    bus_param_ptr->tset = 8;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 1200000;
    bus_param_ptr->max_retry_limit = 3;
    return (PB_TRUE);

  case 9: /* 12 MBaud */
    bus_param_ptr->tsl = 1000;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 800;
    bus_param_ptr->tqui = 9;
    bus_param_ptr->tset = 16;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 2400000;
    bus_param_ptr->max_retry_limit = 4;
    return (PB_TRUE);

  case 11: /* 45,45 Kbaud */
    bus_param_ptr->tsl = 640;
    bus_param_ptr->min_tsdr = 11;
    bus_param_ptr->max_tsdr = 400;
    bus_param_ptr->tqui = 0;
    bus_param_ptr->tset = 95;
    bus_param_ptr->g = 10;
    bus_param_ptr->ttr = 50000;
    bus_param_ptr->max_retry_limit = 1;
    return (PB_TRUE);

  default:
    return (PB_FALSE);
  }
}

FUNCTION GLOBAL PB_BOOL CALL_CONV pbt_get_dp_def_bus_param(
    IN USIGN8 baud_rate, IN USIGN8 station_addr, IN USIGN16 mode,
    OUT T_DP_BUS_PARA_SET FAR* bus_param_ptr)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to get the default busparameter for DP operation
using ASPC2. (use this function only to version 5.02)

IN:  baud_rate      -> desired baudrate    (see valid baudrates)
IN:  station_addr   -> desired station address   (0..126)
IN:  mode:          -> DP_MODE         standalone DP operation
                       DP_FMS_FM7_MODE simultaneous DP/FMS/FM7 operation
OUT: bus_param_ptr  -> pointer to DP bus parameter structure

possible return values:
- PB_TRUE   parameters correctly set
- PB_FALSE  no parameter set available, no parameters set
----------------------------------------------------------------------------*/
{
  LOCAL_VARIABLES

  PB_BOOL ret_val;
  T_FMB_SET_BUSPARAMETER_REQ bus_param;

  FUNCTION_BODY

  ret_val = PB_TRUE;
  switch (mode)
  {
  case DP_MODE: /* only DP mode */
    ret_val = pbt_get_bus_param_for_dp(baud_rate, &bus_param);
    break;

  case DP_FMS_FM7_MODE: /* mixed mode FMS-FM7 and DP */
    ret_val = pbt_get_bus_param_for_dpfms(baud_rate, &bus_param);
    break;

  default:
    ret_val = PB_FALSE;
    break;
  }

  if (ret_val == PB_TRUE)
  {
    /* -- set variable busparametes ---------------------------------------- */
    bus_param_ptr->tsl = bus_param.tsl;
    bus_param_ptr->min_tsdr = bus_param.min_tsdr;
    bus_param_ptr->max_tsdr = bus_param.max_tsdr;
    bus_param_ptr->tqui = bus_param.tqui;
    bus_param_ptr->tset = bus_param.tset;
    bus_param_ptr->ttr = bus_param.ttr;
    bus_param_ptr->g = bus_param.g;
    bus_param_ptr->max_retry_limit = bus_param.max_retry_limit;

    /* -- set fix busparametes -------------------------------------------- */
    bus_param_ptr->bus_para_len = 32;
    bus_param_ptr->fdl_add = station_addr;
    bus_param_ptr->baud_rate = baud_rate;
    bus_param_ptr->hsa = HSA;
    bus_param_ptr->bp_flag = 0x00;
    bus_param_ptr->min_slave_interval = 200; /* [100us] */
    bus_param_ptr->poll_timeout = 1000;      /* [  1ms] */
    bus_param_ptr->data_control_time = 100;  /* [ 10ms] */
    bus_param_ptr->master_user_data_len = 34;
    memcpy((USIGN8 FAR*)bus_param_ptr->master_class2_name,
           (USIGN8 FAR*)"SOFTING PROFIBUS DP MASTER      ", 32);

    bus_param_ptr->bus_para_len += bus_param_ptr->master_user_data_len;
  }
  return ret_val;
}

FUNCTION GLOBAL PB_BOOL CALL_CONV pbt_get_fms_fm7_def_bus_param(
    IN USIGN8 baud_rate, IN USIGN8 station_addr, IN PB_BOOL in_ring_desired,
    IN USIGN16 mode, OUT T_SET_BUSPARAMETER_REQ FAR* bus_param_ptr)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to get the default busparameter for FMS/FM7 operation
using ASPC2. (use this function only to version 5.02)

IN:  baud_rate       -> desired baudrate    (see valid baudrates)
IN:  station_addr    -> desired station address   (0..126)
IN:  in_ring_desired -> PB_TRUE   active station (master)
                        PB_FALSE  passive station (slave)
IN:  mode            -> FMS_FM7_MODE    standalone FMS/FM7 operation
                        DP_FMS_FM7_MODE simultaneous DP/FMS/FM7 operation
OUT: bus_param_ptr   -> pointer to FM7 bus parameter structure


possible return values:
- PB_TRUE   parameters correctly set
- PB_FALSE  no parameter set available, no parameters set
----------------------------------------------------------------------------*/
{
  LOCAL_VARIABLES

  T_FMB_SET_BUSPARAMETER_REQ bus_param;

  FUNCTION_BODY

  if (pbt_get_bus_param_for_fms(baud_rate, &bus_param))
  {
    /* -- set variable busparametes -------------------------------------- */
    bus_param_ptr->tsl = bus_param.tsl;
    bus_param_ptr->min_tsdr = bus_param.min_tsdr;
    bus_param_ptr->max_tsdr = bus_param.max_tsdr;
    bus_param_ptr->tqui = bus_param.tqui;
    bus_param_ptr->tset = bus_param.tset;
    bus_param_ptr->ttr = bus_param.ttr;
    bus_param_ptr->g = bus_param.g;
    bus_param_ptr->max_retry_limit = bus_param.max_retry_limit;

    /* -- set fix busparametes -------------------------------------------- */
    bus_param_ptr->loc_add = station_addr;
    bus_param_ptr->loc_segm = NO_SEGMENT;
    bus_param_ptr->baud_rate = baud_rate;
    bus_param_ptr->medium_red = NO_REDUNDANCY;
    bus_param_ptr->in_ring_desired = in_ring_desired;
    bus_param_ptr->hsa = HSA;
    bus_param_ptr->ident[0] = 0;
    bus_param_ptr->ident[1] = 0;
    bus_param_ptr->ident[2] = 0;
    bus_param_ptr->ident[3] = 0;

    return (PB_TRUE);
  }
  else
  {
    return (PB_FALSE);
  }
}

FUNCTION GLOBAL PB_BOOL CALL_CONV pbt_get_fdlif_def_bus_param(
    IN USIGN8 baud_rate, IN USIGN8 station_addr, IN PB_BOOL in_ring_desired,
    IN USIGN16 mode, OUT T_FDLIF_SET_BUSPARAMETER_REQ FAR* bus_param_ptr)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to get the default busparameter for FDL operation
using ASPC2. (use this function only to version 5.02)

IN:  baud_rate       -> desired baudrate    (see valid baudrates)
IN:  station_addr    -> desired station address   (0..126)
IN:  in_ring_desired -> PB_TRUE   active station (master)
                        PB_FALSE  passive station (slave)
IN:  mode            -> FMS_FM7_MODE    standalone FMS/FM7 operation
                        DP_MODE         standalone DP operation
                        DP_FMS_FM7_MODE simultaneous DP/FMS/FM7 operation
OUT: bus_param_ptr   -> pointer to FDLIF bus parameter structure


possible return values:
- PB_TRUE   parameters correctly set
- PB_FALSE  no parameter set available, no parameters set
----------------------------------------------------------------------------*/
{
  LOCAL_VARIABLES

  PB_BOOL ret_val = PB_TRUE;
  T_FMB_SET_BUSPARAMETER_REQ bus_param;

  FUNCTION_BODY

  switch (mode)
  {
  case DP_MODE: /* only DP mode */
    ret_val = pbt_get_bus_param_for_dp(baud_rate, &bus_param);
    break;

  case DP_FMS_FM7_MODE: /* mixed mode FMS-FM7 and DP */
    ret_val = pbt_get_bus_param_for_dpfms(baud_rate, &bus_param);
    break;

  case FMS_FM7_MODE: /* only FMS mode */
    ret_val = pbt_get_bus_param_for_fms(baud_rate, &bus_param);
    break;

  default:
    ret_val = PB_FALSE;
    break;
  }

  if (ret_val == PB_TRUE)
  {
    /* -- set variable busparametes ---------------------------------------- */
    bus_param_ptr->tsl = bus_param.tsl;
    bus_param_ptr->min_tsdr = bus_param.min_tsdr;
    bus_param_ptr->max_tsdr = bus_param.max_tsdr;
    bus_param_ptr->tqui = bus_param.tqui;
    bus_param_ptr->tset = bus_param.tset;
    bus_param_ptr->ttr = bus_param.ttr;
    bus_param_ptr->g = bus_param.g;
    bus_param_ptr->max_retry_limit = bus_param.max_retry_limit;

    /* -- set fix busparametes -------------------------------------------- */
    bus_param_ptr->in_ring_desired = in_ring_desired;
    bus_param_ptr->loc_add = station_addr;
    bus_param_ptr->loc_segm = 0xFF;
    bus_param_ptr->baud_rate = baud_rate;
    bus_param_ptr->hsa = HSA;
    bus_param_ptr->medium_red = NO_REDUNDANCY;
    bus_param_ptr->ident[0] = 0;
    bus_param_ptr->ident[1] = 0;
    bus_param_ptr->ident[2] = 0;
    bus_param_ptr->ident[3] = 0;
  }

  return (ret_val);
}

FUNCTION GLOBAL PB_BOOL CALL_CONV pbt_get_fmb_def_bus_param(
    IN USIGN8 baud_rate, IN USIGN8 station_addr, IN PB_BOOL in_ring_desired,
    IN USIGN16 mode, OUT T_FMB_SET_BUSPARAMETER_REQ FAR* bus_param_ptr)

/*----------------------------------------------------------------------------
FUNCTIONAL_DESCRIPTION

This function is used to get the default busparameter for FMB operation
using ASPC2.

IN:  baud_rate       -> desired baudrate    (see valid baudrates)
IN:  station_addr    -> desired station address   (0..126)
IN:  in_ring_desired -> PB_TRUE   active station (master)
                        PB_FALSE  passive station (slave)
IN:  mode            -> DP_MODE         standalone DP operation
                        DP_FMS_FM7_MODE simultaneous DP/FMS/FM7 operation
                        FMS_FM7_MODE    standalone FMS/FM7 operation
OUT: bus_param_ptr   -> pointer to FMB bus parameter structure

possible return values:
- PB_TRUE   parameters correctly set
- PB_FALSE  no parameter set available, no parameters set
----------------------------------------------------------------------------*/
{
  LOCAL_VARIABLES

  PB_BOOL ret_val = PB_TRUE;
  T_FMB_SET_BUSPARAMETER_REQ bus_param;

  FUNCTION_BODY

  switch (mode)
  {
  case DP_MODE: /* only DP mode */
    ret_val = pbt_get_bus_param_for_dp(baud_rate, &bus_param);
    break;

  case DP_FMS_FM7_MODE: /* mixed mode FMS-FM7 and DP */
    ret_val = pbt_get_bus_param_for_dpfms(baud_rate, &bus_param);
    break;

  case FMS_FM7_MODE: /* only FMS mode */
    ret_val = pbt_get_bus_param_for_fms(baud_rate, &bus_param);
    break;

  default:
    ret_val = PB_FALSE;
    break;
  }

  if (ret_val == PB_TRUE)
  {
    /* -- set variable busparametes ---------------------------------------- */
    bus_param_ptr->tsl = bus_param.tsl;
    bus_param_ptr->min_tsdr = bus_param.min_tsdr;
    bus_param_ptr->max_tsdr = bus_param.max_tsdr;
    bus_param_ptr->tqui = bus_param.tqui;
    bus_param_ptr->tset = bus_param.tset;
    bus_param_ptr->ttr = bus_param.ttr;
    bus_param_ptr->g = bus_param.g;
    bus_param_ptr->max_retry_limit = bus_param.max_retry_limit;

    /* -- set fix busparametes -------------------------------------------- */
    bus_param_ptr->in_ring_desired = in_ring_desired;
    bus_param_ptr->loc_add = station_addr;
    bus_param_ptr->loc_segm = 0xFF;
    bus_param_ptr->baud_rate = baud_rate;
    bus_param_ptr->hsa = HSA;
    bus_param_ptr->medium_red = NO_REDUNDANCY;
  }

  return (ret_val);
}

#if defined(WIN32) || defined(_WIN32) || defined(WIN16) || defined(_WIN16)
#pragma check_stack
#endif
