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

FILE_NAME               DPGDL.C

PROJECT_NAME            PROFIBUS

MODULE                  DPGDL

COMPONENT_LIBRARY       PAPI Lib
                        PAPI DLL

AUTHOR                  SOFTING

VERSION                 5.45.0.00.release

DATE                    Dezember-2009

STATUS                  finished

FUNCTIONAL_MODULE_DESCRIPTION

This modul contains DP-Service-Specific-Functions which return the length
length of the Request- or Response-Datas.


RELATED_DOCUMENTS
=============================================================================*/

#include "keywords.h"

INCLUDES

#if defined(WIN16) || defined(WIN32)
#include <windows.h>
#endif

#include "pb_type.h"
#include "pb_if.h"
#include "pb_dp.h"
#include "pb_err.h"

FUNCTION_DECLARATIONS

extern USIGN16 swap_16_intel_motorola(USIGN16);

LOCAL_DEFINES

LOCAL_TYPEDEFS

EXPORT_DATA

IMPORT_DATA

LOCAL_DATA

#if defined(WIN32) || defined(_WIN32) || defined(WIN16) || defined(_WIN16)
#pragma check_stack(off)
#endif

FUNCTION PUBLIC INT16 dpgdl_get_data_len(IN INT16 result, IN USIGN8 service,
                                         IN USIGN8 primitive,
                                         IN USIGN8 FAR* data_ptr,
                                         OUT INT16 FAR* data_len_ptr)
/*------------------------------------------------------------------------*/
/* FUNCTIONAL_DESCRIPTION                                                 */
/*------------------------------------------------------------------------*/
/* - returns the data length of any called PROFIBUS DP service            */
/*------------------------------------------------------------------------*/
{
  LOCAL_VARIABLES

  USIGN16 service_data_len;

  FUNCTION_BODY

  switch (primitive)
  {
  /*--- USER REQUESTS ----------------------------------------------------*/

  case REQ:
  {
    switch (service)
    {
    case DP_DATA_TRANSFER:
      *data_len_ptr = 0;
      break;
    case DP_GLOBAL_CONTROL:
      *data_len_ptr = sizeof(T_DP_GLOBAL_CONTROL_REQ);
      break;

    case DP_ACT_PARAM_LOC:
    case DP_ACT_PARA_BRCT:
    case DP_ACT_PARAM:
      *data_len_ptr = sizeof(T_DP_ACT_PARAM_REQ);
      break;

    case DP_INIT_MASTER:
      *data_len_ptr = sizeof(T_DP_INIT_MASTER_REQ);
      break;

    case DP_UPLOAD_LOC:
    case DP_UPLOAD:
      *data_len_ptr = sizeof(T_DP_UPLOAD_REQ);
      break;

    case DP_DOWNLOAD_LOC:
    case DP_DOWNLOAD:
    {
      if (((T_DP_DOWNLOAD_REQ FAR*)(data_ptr))->data_len >
          DP_MAX_DOWNLOAD_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }

      *data_len_ptr = sizeof(T_DP_DOWNLOAD_REQ) +
                      ((T_DP_DOWNLOAD_REQ FAR*)(data_ptr))->data_len;
      break;
    }

    case DP_START_SEQ_LOC:
    case DP_START_SEQ:
      *data_len_ptr = sizeof(T_DP_START_SEQ_REQ);
      break;

    case DP_END_SEQ_LOC:
    case DP_END_SEQ:
      *data_len_ptr = sizeof(T_DP_END_SEQ_REQ);
      break;

    case DP_GET_SLAVE_DIAG:
      *data_len_ptr = 0;
      break;

    case DP_SET_PRM:
    case DP_SET_PRM_LOC:
    {
      service_data_len = swap_16_intel_motorola(
          ((T_DP_SET_PRM_REQ FAR*)(data_ptr))->prm_data.prm_data_len);

      if (service_data_len > DP_MAX_PRM_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }

      *data_len_ptr =
          sizeof(T_DP_SET_PRM_REQ) - sizeof(T_DP_PRM_DATA) + service_data_len;
      break;
    }

    case DP_CHK_CFG:
    {
      service_data_len = swap_16_intel_motorola(
          ((T_DP_CHK_CFG_REQ FAR*)(data_ptr))->cfg_data.cfg_data_len);

      if (service_data_len > DP_MAX_CFG_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }

      *data_len_ptr =
          sizeof(T_DP_CHK_CFG_REQ) - sizeof(T_DP_CFG_DATA) + service_data_len;
      break;
    }

    case DP_GET_CFG:
      *data_len_ptr = sizeof(T_DP_GET_CFG_REQ);
      break;
    case DP_SLAVE_DIAG:
      *data_len_ptr = sizeof(T_DP_SLAVE_DIAG_REQ);
      break;
    case DP_RD_INP:
      *data_len_ptr = sizeof(T_DP_RD_INP_REQ);
      break;
    case DP_RD_OUTP:
      *data_len_ptr = sizeof(T_DP_RD_OUTP_REQ);
      break;

    case DP_DATA_EXCHANGE:
    {
      if (((T_DP_DATA_EXCHANGE_REQ FAR*)(data_ptr))->outp_data_len >
          DP_MAX_OUTPUT_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }

      *data_len_ptr = sizeof(T_DP_DATA_EXCHANGE_REQ) +
                      ((T_DP_DATA_EXCHANGE_REQ FAR*)(data_ptr))->outp_data_len;
      break;
    }

    case DP_SET_SLAVE_ADD:
    {
      if (((T_DP_SET_SLAVE_ADD_REQ FAR*)(data_ptr))->rem_slave_data_len >
          DP_MAX_REM_SLAVE_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }

      *data_len_ptr =
          sizeof(T_DP_SET_SLAVE_ADD_REQ) +
          ((T_DP_SET_SLAVE_ADD_REQ FAR*)(data_ptr))->rem_slave_data_len;
      break;
    }

    case DP_GET_MASTER_DIAG_LOC:
    case DP_GET_MASTER_DIAG:
      *data_len_ptr = sizeof(T_DP_GET_MASTER_DIAG_REQ);
      break;

    case DP_GET_SLAVE_PARAM:
      *data_len_ptr = sizeof(T_DP_GET_SLAVE_PARAM_REQ);
      break;

    case DP_EXIT_MASTER:
      *data_len_ptr = 0;
      break;

    case DP_SET_BUSPARAMETER:
    {
      if ((((T_DP_SET_BUSPARAMETER_REQ FAR*)(data_ptr))->master_user_data_len >
           DP_MAX_SET_BUSPARAMETER_LEN) ||
          (((T_DP_SET_BUSPARAMETER_REQ FAR*)(data_ptr))->master_user_data_len <
           DP_MASTER_USER_DATA_LEN))
      {
        return (E_IF_INVALID_DATA_SIZE);
      }

      *data_len_ptr =
          sizeof(T_DP_SET_BUSPARAMETER_REQ) +
          ((T_DP_SET_BUSPARAMETER_REQ FAR*)(data_ptr))->master_user_data_len -
          DP_MASTER_USER_DATA_LEN;
      break;
    }

    case DP_SET_MASTER_PARAM:
    {
      if (((T_DP_SET_MASTER_PARAM_REQ FAR*)(data_ptr))->data_len >
          DP_MAX_TELEGRAM_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }

      *data_len_ptr = sizeof(T_DP_SET_MASTER_PARAM_REQ) +
                      ((T_DP_SET_MASTER_PARAM_REQ FAR*)(data_ptr))->data_len;
      break;
    }

    case DP_INITIATE:
      *data_len_ptr =
          sizeof(T_DP_INITIATE_REQ) +
          ((T_DP_INITIATE_REQ FAR*)(data_ptr))->add_addr_param.s_len +
          ((T_DP_INITIATE_REQ FAR*)(data_ptr))->add_addr_param.d_len;
      break;

    case DP_ABORT:
      *data_len_ptr = sizeof(T_DP_ABORT_REQ);
      break;

    case DP_READ:
      *data_len_ptr = sizeof(T_DP_READ_REQ);
      break;

    case DP_WRITE:
      *data_len_ptr =
          sizeof(T_DP_WRITE_REQ) + ((T_DP_WRITE_REQ FAR*)(data_ptr))->length;
      break;

    case DP_DATA_TRANSPORT:
      *data_len_ptr = sizeof(T_DP_DATA_TRANSPORT_REQ) +
                      ((T_DP_DATA_TRANSPORT_REQ FAR*)(data_ptr))->length;
      break;

    default:
      return (E_IF_INVALID_SERVICE);
    } /* switch serivce */

    break;
  } /* case REQ */

  /*---- USER RESPONSES --------------------------------------------------*/

  case RES:
  {
    switch (service)
    {
    case DP_ACT_PARAM:
      *data_len_ptr = sizeof(T_DP_ACT_PARAM_RES_CON);
      break;
    case DP_DOWNLOAD:
      *data_len_ptr = sizeof(T_DP_DOWNLOAD_RES_CON);
      break;
    case DP_START_SEQ:
      *data_len_ptr = sizeof(T_DP_START_SEQ_RES_CON);
      break;
    case DP_END_SEQ:
      *data_len_ptr = sizeof(T_DP_END_SEQ_RES_CON);
      break;

    case DP_UPLOAD:
    {
      if (((T_DP_UPLOAD_RES_CON FAR*)(data_ptr))->data_len >
          DP_MAX_UPLOAD_DATA_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }

      *data_len_ptr = sizeof(T_DP_UPLOAD_RES_CON) +
                      ((T_DP_UPLOAD_RES_CON FAR*)(data_ptr))->data_len;
      break;
    }

    case DP_GET_MASTER_DIAG:
    {
      if (((T_DP_GET_MASTER_DIAG_RES_CON FAR*)(data_ptr))->data_len >
          DP_MAX_TELEGRAM_LEN)
      {
        return (E_IF_INVALID_DATA_SIZE);
      }

      *data_len_ptr = sizeof(T_DP_GET_MASTER_DIAG_RES_CON) +
                      ((T_DP_GET_MASTER_DIAG_RES_CON FAR*)(data_ptr))->data_len;
      break;
    }

    default:
      return (E_IF_INVALID_SERVICE);
    } /* switch serivce */

    break;
  } /* case RES */

  /*---- WRONG PRIMITIVE -------------------------------------------------*/

  default:
    return (E_IF_INVALID_PRIMITIVE);
  } /* switch primitive */

  return (E_OK);
} /* dp_get_data_len */

#if defined(WIN32) || defined(_WIN32) || defined(WIN16) || defined(_WIN16)
#pragma check_stack
#endif
