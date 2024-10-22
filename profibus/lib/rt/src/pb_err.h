/*****************************************************************************/
/*                                                                           */
/*                    SOFTING Industrial Automation GmbH                     */
/*                        Richard-Reitzner-Allee 6                           */
/*                              D-85540 Haar                                 */
/*                      Phone: (++49)-(0)89-45656-0                          */
/*                      Fax:   (++49)-(0)89-45656-399                        */
/*                                                                           */
/*         Copyright (C) SOFTING Industrial Automation GmbH 1995-2012        */
/*                            All Rights Reserved                            */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/*      PROFIBUS ABORT-, REJECT-, EVENT- and ERROR DEFINES and -TYPES        */
/*                                                                           */
/*  Filename    : PB_ERR.H                                                   */
/*  Version     : 5.46.0.00.release                                          */
/*  Date        : October-2011                                               */
/*  Author      : SOFTING                                                    */
/*                                                                           */
/*  Description : This file contains the ABORT, REJECT, EVENT and ERROR      */
/*                types and defines and the according reason codes.          */
/*                                                                           */
/*****************************************************************************/

#ifndef __PB_ERR__
#define __PB_ERR__

#if defined(WIN32) || defined(_WIN32) || defined(WIN16) || defined(_WIN16)
#pragma warning(disable : 4103) /* used #pragma pack to change alignment */
#ifdef WIN32
#pragma pack(push, 2)
#else
#pragma pack(2)
#endif
#pragma warning(default : 4103)
#endif

/*****************************************************************************/
/*************   ABORT REASON CODES                      *********************/
/*****************************************************************************/

/* --- USER abort reasons -------------------------------------------------- */
#define USR_ABT_RC1 0 /* disconnected by user                        */
#define USR_ABT_RC2 1 /* version object dictionary incompatible      */
#define USR_ABT_RC3 2 /* password error                              */
#define USR_ABT_RC4 3 /* profile number incompatible                 */
#define USR_ABT_RC5 4 /* limited services permitted                  */
#define USR_ABT_RC6 5 /* OD loading interacting (OD is beeing loaded)*/

/* --- FMS abort reasons --------------------------------------------------- */
#define FMS_ABT_RC1 0   /* FMS-CRL error (CRL entry invalid)            */
#define FMS_ABT_RC2 1   /* user error (protocol violation by user)      */
#define FMS_ABT_RC3 2   /* FMS-PDU error (invalid FMS PDU received)     */
#define FMS_ABT_RC4 3   /* connection state conflict LLI                */
#define FMS_ABT_RC5 4   /* LLI error                                    */
#define FMS_ABT_RC6 5   /* PDU size exceeds maximum PDU size            */
#define FMS_ABT_RC7 6   /* feature not supported                        */
#define FMS_ABT_RC8 7   /* invoke id error in service response          */
#define FMS_ABT_RC9 8   /* max services overflow                        */
#define FMS_ABT_RC10 9  /* connection state conflict FMS (Initiate.req) */
#define FMS_ABT_RC11 10 /* service error (res != ind or con != req)     */
#define FMS_ABT_RC12 11 /* invoke id error in service request           */
#define FMS_ABT_RC13 12 /* FMS is disabled                              */

/* --- FM7 abort reasons  -------------------------------------------------- */
#define FM7_ABT_RC1 0   /* FM7-CRL error                                */
#define FM7_ABT_RC2 1   /* user error                                   */
#define FM7_ABT_RC3 2   /* FM7-PDU error                                */
#define FM7_ABT_RC4 3   /* connection state conflict LLI                */
#define FM7_ABT_RC5 4   /* LLI error                                    */
#define FM7_ABT_RC6 5   /* PDU size                                     */
#define FM7_ABT_RC7 6   /* feature not supported                        */
#define FM7_ABT_RC8 7   /* response error                               */
#define FM7_ABT_RC9 8   /* max services overflow                        */
#define FM7_ABT_RC10 9  /* connection state conflict FM7                */
#define FM7_ABT_RC11 10 /* service error                                */

#if (PB_VER < 500)
#define FMA_ABT_RC1 FM7_ABT_RC1
#define FMA_ABT_RC2 FM7_ABT_RC2
#define FMA_ABT_RC3 FM7_ABT_RC3
#define FMA_ABT_RC4 FM7_ABT_RC4
#define FMA_ABT_RC5 FM7_ABT_RC5
#define FMA_ABT_RC6 FM7_ABT_RC6
#define FMA_ABT_RC7 FM7_ABT_RC7
#define FMA_ABT_RC8 FM7_ABT_RC8
#define FMA_ABT_RC9 FM7_ABT_RC9
#define FMA_ABT_RC10 FM7_ABT_RC10
#define FMA_ABT_RC11 FM7_ABT_RC11
#endif

/* --- LLI abort reasons --------------------------------------------------- */
#define LLI_ABT_RC1 0   /* LLI context check neg., remote context in AD */
#define LLI_ABT_RC2 1   /* invalid LLI PDU during associate or abort    */
#define LLI_ABT_RC3 2   /* invalid LLI PDU during data transfer phase   */
#define LLI_ABT_RC4 3   /* unknown or invalid LLI PDU received          */
#define LLI_ABT_RC5 4   /* DTA_ACK_PDU received and SAC = 0             */
#define LLI_ABT_RC6 5   /* max no of parallel services exceeded (by PDU)*/
#define LLI_ABT_RC7 6   /* unkown invoke id                             */
#define LLI_ABT_RC8 7   /* priority error                               */
#define LLI_ABT_RC9 8   /* local error at remote station                */
#define LLI_ABT_RC10 9  /* timer 1 expired (associate)                  */
#define LLI_ABT_RC11 10 /* timer 3 expired (supervision of connection)  */
#define LLI_ABT_RC12 11 /* receive timer expired                        */
#define LLI_ABT_RC13 12 /* error while activating LSAP (state in AD)    */
#define LLI_ABT_RC14 13 /* illegal FDL prim. during ASS or ABT (see AD) */
#define LLI_ABT_RC15 14 /* illegal FDL prim. in data transfer  (see AD) */
#define LLI_ABT_RC16 15 /* unkown FDL primitive                         */
#define LLI_ABT_RC17 16 /* unkown LLI primitive                         */
#define LLI_ABT_RC18 17 /* illegal LLI prim. during ASS or ABT (see AD) */
#define LLI_ABT_RC19 18 /* illegal LLI prim. in data transfer  (see AD) */
#define LLI_ABT_RC20 19 /* invalid CRL entry                            */
#define LLI_ABT_RC21 20 /* conflict during associate                    */
#define LLI_ABT_RC22 21 /* procedural error on cyclic connection        */
#define LLI_ABT_RC23 22 /* max no of parallel services exceeded (by FMS)*/
#define LLI_ABT_RC24 23 /* CRL beeing loaded, LLI disabled              */
#define LLI_ABT_RC25 24 /* confirm / indication mode error              */
#define LLI_ABT_RC26 25 /* illegal FM1/2 primitive received             */
#define LLI_ABT_RC27 26 /* illegal FMS service on cyclic connection     */
#define LLI_ABT_RC28 27 /* FMS PDU too large on cyclic connection       */
#define LLI_ABT_RC29 28 /* resource error during associate              */
#define LLI_ABT_RC30 29 /* resource error in data transfer phase        */
#define LLI_ABT_RC31 30 /* resource error during abort                  */
#define LLI_ABT_RC32 31 /* LLI state error                              */
#define LLI_ABT_RC33 32 /* timer error                                  */
#define LLI_ABT_RC34 33 /* resource transfer to FDL failed              */

/* --- LLI abort details --------------------------------------------------- */
#define LLI_ABT_AD1 0 /* error in the loading of the update buffer    */
#define LLI_ABT_AD2 1 /* error in the activating of a poll list entry */
#define LLI_ABT_AD3 2 /* error in the deactivating of apoll list entry*/
#define LLI_ABT_AD4 3 /* transmit error (SDA.con)                     */
#define LLI_ABT_AD5 4 /* transmit error (CSRD.con)                    */
#define LLI_ABT_AD6 5 /* transmit error (SRD.con)                     */
#define LLI_ABT_AD7 6 /* receive error (CSRD.con)                     */

/* --- FDL abort reasons --------------------------------------------------- */
#define FDL_ABT_UE 1   /* remote user interface error                  */
#define FDL_ABT_RR 2   /* no remote resources available                */
#define FDL_ABT_RS 3   /* service not activated at remote sap          */
#define FDL_ABT_RA 4   /* no access to remote sap                      */
#define FDL_ABT_RDL 12 /* no resource for send response data low       */
#define FDL_ABT_RDH 13 /* no resource for send response data high      */
#define FDL_ABT_LS 16  /* service not activated at local sap           */
#define FDL_ABT_NA 17  /* no reaction from remote station              */
#define FDL_ABT_DS 18  /* disconnected station  (NLT)                  */
#define FDL_ABT_NO 19  /* FDL servcice not OK                          */
#define FDL_ABT_LR 20  /* no local resources available                 */
#define FDL_ABT_IV 21  /* invalid request parameters                   */

/*****************************************************************************/
/*************   DP/V1 ABORT INSTANCES AND REASON CODES  *********************/
/*****************************************************************************/

#define DP_ABORT_SUBNET_NO 0 /* abort: subnet */
#define DP_ABORT_SUBNET_LOCAL 1
#define DP_ABORT_SUBNET_REMOTE 2

#define DP_ABORT_REASON_MASK 0x3F /* abort: instance+reason */
#define DP_ABORT_INSTANCE_MASK 0xC0

#define DP_ABORT_INSTANCE_FDL 0x00 /* abort: instance */
#define DP_ABORT_INSTANCE_DDLM 0x40
#define DP_ABORT_INSTANCE_USER 0x80

/*abort: fdl-reason as defined in draft */
#define DP_ABORT_FDL_UE (DP_ABORT_INSTANCE_FDL | 0x01)
#define DP_ABORT_FDL_RR (DP_ABORT_INSTANCE_FDL | 0x02)
#define DP_ABORT_FDL_RS (DP_ABORT_INSTANCE_FDL | 0x03)
#define DP_ABORT_FDL_NR (DP_ABORT_INSTANCE_FDL | 0x09)
#define DP_ABORT_FDL_DH (DP_ABORT_INSTANCE_FDL | 0x0A)
#define DP_ABORT_FDL_RDL (DP_ABORT_INSTANCE_FDL | 0x0C)
#define DP_ABORT_FDL_RDH (DP_ABORT_INSTANCE_FDL | 0x0D)

/* abort: fdl-reason locally softing specific */
#define DP_ABORT_FDL_LS (DP_ABORT_INSTANCE_FDL | 0x10)
#define DP_ABORT_FDL_NA (DP_ABORT_INSTANCE_FDL | 0x11)
#define DP_ABORT_FDL_DS (DP_ABORT_INSTANCE_FDL | 0x12)
#define DP_ABORT_FDL_NO (DP_ABORT_INSTANCE_FDL | 0x13)
#define DP_ABORT_FDL_LR (DP_ABORT_INSTANCE_FDL | 0x14)
#define DP_ABORT_FDL_IV (DP_ABORT_INSTANCE_FDL | 0x15)

/* abort: ddlm-reason as defined in draft */
#define DP_ABORT_DDLM_ABT_SE (DP_ABORT_INSTANCE_DDLM | 0x01)
#define DP_ABORT_DDLM_ABT_FE (DP_ABORT_INSTANCE_DDLM | 0x02)
#define DP_ABORT_DDLM_ABT_TO (DP_ABORT_INSTANCE_DDLM | 0x03)
#define DP_ABORT_DDLM_ABT_RE (DP_ABORT_INSTANCE_DDLM | 0x04)
#define DP_ABORT_DDLM_ABT_IV (DP_ABORT_INSTANCE_DDLM | 0x05)
#define DP_ABORT_DDLM_ABT_STO (DP_ABORT_INSTANCE_DDLM | 0x06)
#define DP_ABORT_DDLM_ABT_IA (DP_ABORT_INSTANCE_DDLM | 0x07)
#define DP_ABORT_DDLM_ABT_OC (DP_ABORT_INSTANCE_DDLM | 0x08)

/*****************************************************************************/
/*************   REJECT PDU TYPES and REASON CODES       *********************/
/*****************************************************************************/

/* --- PDU types ----------------------------------------------------------- */
#define CONFIRMED_REQUEST_PDU 1
#define CONFIRMED_RESPONSE_PDU 2
#define UNCONFIRMED_PDU 3
#define UNKNOWN_PDU_TYPE 4

/* --- reason codes -------------------------------------------------------- */
#define REJ_RC0 0 /* other than RC1 to RC6                       */
#define REJ_RC1 1 /* invoke id exists already                    */
#define REJ_RC2 2 /* max services overflow (max. SCC exceeded)   */
#define REJ_RC3 3 /* feature not supported - connection oriented */
#define REJ_RC4 4 /* feature not supported - connectionless      */
#define REJ_RC5 5 /* PDU size exceeds maximum PDU size allowed   */
#define REJ_RC6 6 /* user error on connectionless relation       */

/*****************************************************************************/
/*************   FMS ERROR CLASSES and ERROR CODES       *********************/
/*****************************************************************************/

/* The error class is encoded in the high byte of the 16-bit-result,         */
/* the error code in the low byte.                                           */

#define E_FMS_INIT 0x0000
#define E_FMS_INIT_OTHER 0x0000
#define E_FMS_INIT_MAX_PDU_SIZE_INSUFF 0x0001
#define E_FMS_INIT_FEAT_NOT_SUPPORTED 0x0002
#define E_FMS_INIT_OD_VERSION_INCOMP 0x0003
#define E_FMS_INIT_USER_DENIED 0x0004
#define E_FMS_INIT_PASSWORD_ERROR 0x0005
#define E_FMS_INIT_PROFILE_NUMB_INCOMP 0x0006

#define E_FMS_VFD_STATE_OTHER 0x0100

#define E_FMS_APPLICATION_OTHER 0x0200
#define E_FMS_APPLICATION_UNREACHABLE 0x0201

#define E_FMS_DEF_OTHER 0x0300
#define E_FMS_DEF_OBJ_UNDEF 0x0301
#define E_FMS_DEF_OBJ_ATTR_INCONSIST 0x0302
#define E_FMS_DEF_OBJECT_ALREADY_EXISTS 0x0303

#define E_FMS_RESOURCE_OTHER 0x0400
#define E_FMS_RESOURCE_MEM_UNAVAILABLE 0x0401

#define E_FMS_SERV_OTHER 0x0500
#define E_FMS_SERV_OBJ_STATE_CONFLICT 0x0501
#define E_FMS_SERV_PDU_SIZE 0x0502
#define E_FMS_SERV_OBJ_CONSTR_CONFLICT 0x0503
#define E_FMS_SERV_PARAM_INCONSIST 0x0504
#define E_FMS_SERV_ILLEGAL_PARAM 0x0505

#define E_FMS_ACCESS_OTHER 0x0600
#define E_FMS_ACCESS_OBJ_INVALIDATED 0x0601
#define E_FMS_ACCESS_HARDWARE_FAULT 0x0602
#define E_FMS_ACCESS_OBJ_ACCESS_DENIED 0x0603
#define E_FMS_ACCESS_ADDR_INVALID 0x0604
#define E_FMS_ACCESS_OBJ_ATTR_INCONST 0x0605
#define E_FMS_ACCESS_OBJ_ACCESS_UNSUPP 0x0606
#define E_FMS_ACCESS_OBJ_NON_EXIST 0x0607
#define E_FMS_ACCESS_TYPE_CONFLICT 0x0608
#define E_FMS_ACCESS_NAME_ACCESS_UNSUP 0x0609

#define E_FMS_OD_OTHER 0x0700
#define E_FMS_OD_NAME_LEN_OVERFLOW 0x0701
#define E_FMS_OD_OVERFLOW 0x0702
#define E_FMS_OD_WRITE_PROTECT 0x0703
#define E_FMS_OD_EXTENSION_LEN_OVERFLOW 0x0704
#define E_FMS_OD_OBJ_DESCR_OVERFLOW 0x0705
#define E_FMS_OD_OPERAT_PROBLEM 0x0706

#define E_FMS_OTHER 0x0800

#if (PB_VER < 500)
#define E_INIT 0x0000
#define E_INIT_OTHER 0x0000
#define E_INIT_MAX_PDU_SIZE_INSUFF 0x0001
#define E_INIT_FEAT_NOT_SUPPORTED 0x0002
#define E_INIT_OV_VERSION_INCOMP 0x0003
#define E_INIT_USER_DENIED 0x0004
#define E_INIT_PASSWORD_ERROR 0x0005
#define E_INIT_PROFILE_NUMB_INCOMP 0x0006

#define E_VFD_STATE_OTHER 0x0100

#define E_APPLICATION_OTHER 0x0200
#define E_APPLICATION_UNREACHABLE 0x0201

#define E_DEF_OTHER 0x0300
#define E_DEF_OBJ_UNDEF 0x0301
#define E_DEF_OBJ_ATTR_INCONSIST 0x0302
#define E_DEF_OBJECT_ALREADY_EXISTS 0x0303

#define E_RESOURCE_OTHER 0x0400
#define E_RESOURCE_MEM_UNAVAILABLE 0x0401

#define E_SERV_OTHER 0x0500
#define E_SERV_OBJ_STATE_CONFLICT 0x0501
#define E_SERV_PDU_SIZE 0x0502
#define E_SERV_OBJ_CONSTR_CONFLICT 0x0503
#define E_SERV_PARAM_INCONSIST 0x0504
#define E_SERV_ILLEGAL_PARAM 0x0505

#define E_ACCESS_OTHER 0x0600
#define E_ACCESS_OBJ_INVALIDATED 0x0601
#define E_ACCESS_HARDWARE_FAULT 0x0602
#define E_ACCESS_OBJ_ACCESS_DENIED 0x0603
#define E_ACCESS_ADDR_INVALID 0x0604
#define E_ACCESS_OBJ_ATTR_INCONST 0x0605
#define E_ACCESS_OBJ_ACCESS_UNSUPP 0x0606
#define E_ACCESS_OBJ_NON_EXIST 0x0607
#define E_ACCESS_TYPE_CONFLICT 0x0608
#define E_ACCESS_NAME_ACCESS_UNSUP 0x0609

#define E_OV_OTHER 0x0700
#define E_OV_NAME_LEN_OVERFLOW 0x0701
#define E_OV_OVERFLOW 0x0702
#define E_OV_WRITE_PROTECT 0x0703
#define E_OV_EXTENSION_LEN_OVERFLOW 0x0704
#define E_OV_OBJ_DESCR_OVERFLOW 0x0705
#define E_OV_OPERAT_PROBLEM 0x0706

#define E_OTHER 0x0800
#endif

/*****************************************************************************/
/*************   FM7 ERROR CLASSES and ERROR CODES       *********************/
/*****************************************************************************/

/* The error class is encoded in the high byte of the 16-bit-result,         */
/* the error code in the low byte.                                           */

#define E_FM7_INIT_OTHER 0x0000
#define E_FM7_INIT_MAX_PDU_SIZE_INSUFF 0x0001
#define E_FM7_INIT_FEAT_NOT_SUPPORTED 0x0002
#define E_FM7_INIT_USER_DENIED 0x0003

#define E_FM7_APPLICATION_OTHER 0x0100
#define E_FM7_APPLICATION_UNREACHABLE 0x0101

#define E_FM7_RESOURCE_OTHER 0x0200
#define E_FM7_RESOURCE_MEM_UNAVAILABLE 0x0201

#define E_FM7_SERV_OTHER 0x0300
#define E_FM7_SERV_OBJ_STATE_CONFLICT 0x0301
#define E_FM7_SERV_OBJ_CONSTR_CONFLICT 0x0302
#define E_FM7_SERV_PARAM_INCONSIST 0x0303
#define E_FM7_SERV_ILLEGAL_PARAM 0x0304
#define E_FM7_SERV_PERM_INTERN_FAULT 0x0305

#define E_FM7_USR_OTHER 0x0400
#define E_FM7_USR_DONT_WORRY_BE_HAPPY 0x0401
#define E_FM7_USR_MEM_UNAVAILABLE 0x0402

#define E_FM7_ACCESS_OTHER 0x0500
#define E_FM7_ACCESS_OBJ_ACC_UNSUP 0x0501
#define E_FM7_ACCESS_OBJ_NON_EXIST 0x0502
#define E_FM7_ACCESS_OBJ_ACCESS_DENIED 0x0503
#define E_FM7_ACCESS_HARDWARE_FAULT 0x0504
#define E_FM7_ACCESS_TYPE_CONFLICT 0x0505

#define E_FM7_CRL_OTHER 0x0600
#define E_FM7_CRL_INVALID_ENTRY 0x0601
#define E_FM7_CRL_NO_CRL_ENTRY 0x0602
#define E_FM7_CRL_INVALID_CRL 0x0603
#define E_FM7_CRL_NO_CRL 0x0604
#define E_FM7_CRL_WRITE_PROTECTED 0x0605
#define E_FM7_CRL_NO_ENTRY_FOUND 0x0606
#define E_FM7_CRL_NO_MULT_VFD_SUPP 0x0607

#define E_FM7_OTHER 0x0700

#if (PB_VER < 500)
#define E_FMA7_INIT_OTHER 0x0000
#define E_FMA7_INIT_MAX_PDU_SIZE_INSUFF 0x0001
#define E_FMA7_INIT_FEAT_NOT_SUPPORTED 0x0002
#define E_FMA7_INIT_USER_DENIED 0x0003

#define E_FMA7_APPLICATION_OTHER 0x0100
#define E_FMA7_APPLICATION_UNREACHABLE 0x0101

#define E_FMA7_RESOURCE_OTHER 0x0200
#define E_FMA7_RESOURCE_MEM_UNAVAILABLE 0x0201

#define E_FMA7_SERV_OTHER 0x0300
#define E_FMA7_SERV_OBJ_STATE_CONFLICT 0x0301
#define E_FMA7_SERV_OBJ_CONSTR_CONFLICT 0x0302
#define E_FMA7_SERV_PARAM_INCONSIST 0x0303
#define E_FMA7_SERV_ILLEGAL_PARAM 0x0304
#define E_FMA7_SERV_PERM_INTERN_FAULT 0x0305

#define E_FMA7_USR_OTHER 0x0400
#define E_FMA7_USR_DONT_WORRY_BE_HAPPY 0x0401
#define E_FMA7_USR_MEM_UNAVAILABLE 0x0402

#define E_FMA7_ACCESS_OTHER 0x0500
#define E_FMA7_ACCESS_OBJ_ACC_UNSUP 0x0501
#define E_FMA7_ACCESS_OBJ_NON_EXIST 0x0502
#define E_FMA7_ACCESS_OBJ_ACCESS_DENIED 0x0503
#define E_FMA7_ACCESS_HARDWARE_FAULT 0x0504
#define E_FMA7_ACCESS_TYPE_CONFLICT 0x0505

#define E_FMA7_KBL_OTHER 0x0600
#define E_FMA7_KBL_INVALID_ENTRY 0x0601
#define E_FMA7_KBL_NO_KBL_ENTRY 0x0602
#define E_FMA7_KBL_INVALID_KBL 0x0603
#define E_FMA7_KBL_NO_KBL 0x0604
#define E_FMA7_KBL_WRITE_PROTECTED 0x0605

#define E_FMA7_OTHER 0x0700
#endif

/*****************************************************************************/
/************* ADDITIONAL ERROR CODES          *******************************/
/*****************************************************************************/
#define NO_ADD_DETAIL 0x00

#define AD_LLI_UNEXP_FDL_OR_TIMER_EVT 0x01
#define AD_LLI_LSAP_ACT_FAILED 0x02
#define AD_LLI_POLL_LIST_LOAD_FAILED 0x03
#define AD_LLI_PUT_RESRC_FAILED 0x04
#define AD_LLI_FDL_RESET_FAILED 0x05

#define AD_LLI_INVAL_SERV 0x01
#define AD_LLI_INVAL_SERV_CHANGE 0x02
#define AD_LLI_INVAL_INDEX_CHANGE 0x03

#define AD_FM7_TO_MANY_CRL_ENTRIES 0x10
#define AD_FM7_COMM_REF_NOT_ALLOWED 0x11
#define AD_FM7_TO_MANY_PARALLEL_SERV 0x12
#define AD_FM7_ILLEGAL_FMS_PDU_SIZE 0x13

#if (PB_VER < 500)
#define AD_FMA7_TO_MANY_KBL_ENTRIES 0x10
#define AD_FMA7_COMM_REF_NOT_ALLOWED 0x11
#define AD_FMA7_TO_MANY_PARALLEL_SERV 0x12
#define AD_FMA7_ILLEGAL_FMS_PDU_SIZE 0x13
#endif

/*****************************************************************************/
/************* FM7 EVENTS                      *******************************/
/*****************************************************************************/

/* --- LLI-Fault-Indications ---------------------------------------------- */
#define LLI_FM7_RC1 1   /* error while activating SAP (result in AD)    */
#define LLI_FM7_RC2 2   /* error while deactivating SAP (result in AD)  */
#define LLI_FM7_RC3 3   /* error during FDL_XXX_UPDATE (result in AD)   */
#define LLI_FM7_RC4 4   /* error while activ. poll list entry (see AD)  */
#define LLI_FM7_RC5 5   /* error while deactiv. poll list entry (see AD)*/
#define LLI_FM7_RC6 6   /* illegal FDL prim. during ASS or ABT (see AD) */
#define LLI_FM7_RC7 7   /* illegal FDL prim. in data transfer (see AD)  */
#define LLI_FM7_RC8 8   /* unknown FDL primitive                        */
#define LLI_FM7_RC9 9   /* unknown LLI primitive                        */
#define LLI_FM7_RC10 10 /* illegal LLI prim. during ASS or ABT (see AD) */
#define LLI_FM7_RC11 11 /* illegal LLI prim. in data transfer (see AD)  */
#define LLI_FM7_RC12 12 /* SDA failed (result in AD)                    */
#define LLI_FM7_RC13 13 /* CSRD transmission failed (result in AD)      */
#define LLI_FM7_RC14 14 /* SRD failed (result in AD)                    */
#define LLI_FM7_RC15 15 /* SDN failed (result in AD)                    */
#define LLI_FM7_RC16 16 /* CSRD reception failed (result in AD)         */
#define LLI_FM7_RC17 17 /* poll list loading failed (result in AD)      */
#define LLI_FM7_RC18 18 /* timer 1 expired (associate)                  */
#define LLI_FM7_RC19 19 /* timer 2 expired (abort)                      */
#define LLI_FM7_RC20 20 /* poll list deactivation failed (result in AD) */
#define LLI_FM7_RC21 21 /* no matching comm. reference found            */
#define LLI_FM7_RC22 22 /* illegal FM1/2 prim. (see AD)                 */
#define LLI_FM7_RC23 23 /* illegal FDL prim. during LLI start (see AD)  */
#define LLI_FM7_RC24 24 /* confirm / indication mode error              */
#define LLI_FM7_RC25 25 /* timer error                                  */
#define LLI_FM7_RC26 26 /* resource transfer to FDL failed              */
#define LLI_FM7_RC27 27 /* resource error during associate              */
#define LLI_FM7_RC28 28 /* resource error in data transfer phase        */
#define LLI_FM7_RC29 29 /* resource error during abort                  */
#define LLI_FM7_RC30 30 /* LLI state error                              */

#if (PB_VER < 500)
#define LLI_FMA7_RC1 1   /* error while activating SAP (result in AD)    */
#define LLI_FMA7_RC2 2   /* error while deactivating SAP (result in AD)  */
#define LLI_FMA7_RC3 3   /* error during FDL_XXX_UPDATE (result in AD)   */
#define LLI_FMA7_RC4 4   /* error while activ. poll list entry (see AD)  */
#define LLI_FMA7_RC5 5   /* error while deactiv. poll list entry (see AD)*/
#define LLI_FMA7_RC6 6   /* illegal FDL prim. during ASS or ABT (see AD) */
#define LLI_FMA7_RC7 7   /* illegal FDL prim. in data transfer (see AD)  */
#define LLI_FMA7_RC8 8   /* unknown FDL primitive                        */
#define LLI_FMA7_RC9 9   /* unknown LLI primitive                        */
#define LLI_FMA7_RC10 10 /* illegal LLI prim. during ASS or ABT (see AD) */
#define LLI_FMA7_RC11 11 /* illegal LLI prim. in data transfer (see AD)  */
#define LLI_FMA7_RC12 12 /* SDA failed (result in AD)                    */
#define LLI_FMA7_RC13 13 /* CSRD transmission failed (result in AD)      */
#define LLI_FMA7_RC14 14 /* SRD failed (result in AD)                    */
#define LLI_FMA7_RC15 15 /* SDN failed (result in AD)                    */
#define LLI_FMA7_RC16 16 /* CSRD reception failed (result in AD)         */
#define LLI_FMA7_RC17 17 /* poll list loading failed (result in AD)      */
#define LLI_FMA7_RC18 18 /* timer 1 expired (associate)                  */
#define LLI_FMA7_RC19 19 /* timer 2 expired (abort)                      */
#define LLI_FMA7_RC20 20 /* poll list deactivation failed (result in AD) */
#define LLI_FMA7_RC21 21 /* no matching comm. reference found            */
#define LLI_FMA7_RC22 22 /* illegal FMA1/2 prim. (see AD)                */
#define LLI_FMA7_RC23 23 /* illegal FDL prim. during LLI start (see AD)  */
#define LLI_FMA7_RC24 24 /* confirm / indication mode error              */
#define LLI_FMA7_RC25 25 /* timer error                                  */
#define LLI_FMA7_RC26 26 /* resource transfer to FDL failed              */
#define LLI_FMA7_RC27 27 /* resource error during associate              */
#define LLI_FMA7_RC28 28 /* resource error in data transfer phase        */
#define LLI_FMA7_RC29 29 /* resource error during abort                  */
#define LLI_FMA7_RC30 30 /* LLI state error                              */
#endif

/****************************************************************************/
/***********************    FM2 EVENT CODES     *****************************/
/****************************************************************************/

#define FM2_FAULT_ADDRESS 0x01     /* duplicate address recognized */
#define FM2_FAULT_PHY 0x02         /* phys.layer is malfunctioning */
#define FM2_FAULT_TTO 0x03         /* time out on bus detected     */
#define FM2_FAULT_SYN 0x04         /* no receiver synchronization  */
#define FM2_FAULT_OUT_OF_RING 0x05 /* station out of ring          */
#define FM2_GAP_EVENT 0x06         /* new station in ring          */

#if (PB_VER < 500)
#define FMA2_FAULT_ADDRESS 0x01     /* duplicate address recognized */
#define FMA2_FAULT_TRANSCEIVER 0x02 /* transceiver error occured    */
#define FMA2_FAULT_TTO 0x03         /* time out on bus detected     */
#define FMA2_FAULT_SYN 0x04         /* no receiver synchronization  */
#define FMA2_FAULT_OUT_OF_RING 0x05 /* station out of ring          */
#define FMA2_GAP_EVENT 0x06         /* new station in ring          */
#endif

/* --- Additional FM2-Events ---------------------------------------------- */
#define FM2_MAC_ERROR 0x13 /* fatal MAC error              */
#define FM2_HW_ERROR 0x14  /* fatal HW error               */

/****************************************************************************/
/*************    FMB ERROR CLASSES and ERROR CODES     *********************/
/****************************************************************************/

#define E_FMB_RESOURCE_OTHER 0x0200
#define E_FMB_RESOURCE_MEM_UNAVAILABLE 0x0201

#define E_FMB_SERV_OTHER 0x0300
#define E_FMB_SERV_OBJ_STATE_CONFLICT 0x0301
#define E_FMB_SERV_OBJ_CONSTR_CONFLICT 0x0302
#define E_FMB_SERV_PARAM_INCONSIST 0x0303
#define E_FMB_SERV_ILLEGAL_PARAM 0x0304
#define E_FMB_SERV_PERM_INTERN_FAULT 0x0305
#define E_FMB_SERV_LOCAL_DISCONNECT 0x0306
#define E_FMB_SERV_NO_ACKNOWLEDGE 0x0307

#define E_FMB_ACCESS_OTHER 0x0500
#define E_FMB_ACCESS_OBJ_ACC_UNSUP 0x0501
#define E_FMB_ACCESS_OBJ_NON_EXIST 0x0502
#define E_FMB_ACCESS_OBJ_ACCESS_DENIED 0x0503
#define E_FMB_ACCESS_HARDWARE_FAULT 0x0504
#define E_FMB_ACCESS_TYPE_CONFLICT 0x0505

#define E_FMB_OTHER 0x0700

#define E_FMB_CFG_DP_OTHER 0x0900
#define E_FMB_CFG_DP_TOO_MANY_SLAVES 0x0901
#define E_FMB_CFG_DP_WRONG_IO_DATA_LEN 0x0902
#define E_FMB_CFG_DP_IO_ALIGNMENT_ERROR 0x0903
#define E_FMB_CFG_DP_TOO_FEW_DIAG_ENTRIES 0x0904
#define E_FMB_CFG_DP_WRONG_DIAG_DATA_LEN 0x0905
#define E_FMB_CFG_DP_WRONG_BUS_PARA_LEN 0x0906
#define E_FMB_CFG_DP_WRONG_SLAVE_PARA_LEN 0x0907
#define E_FMB_CFG_DP_DPRAM_ERROR 0x0908

/****************************************************************************/
/*************    DP ERROR CLASSES and ERROR CODES      *********************/
/****************************************************************************/

/*--- DP SERVICE RESULTS ---------------------------------------------------*/

#define E_DP_OK 0x0000 /* acknowledgement positive */
#define E_DP_UE 0x0001 /* remote user error */
#define E_DP_RR 0x0002 /* remote resource insufficient */
#define E_DP_RS 0x0003 /* remote service/SAP deactivated */
#define E_DP_RA 0x0004 /* access of remote SAP blocked */
#define E_DP_NA 0x0011 /* no reaction from remote station */
#define E_DP_DS 0x0012 /* local entity disconnected */
#define E_DP_NO 0x0013 /* not possible in this state */
#define E_DP_LR 0x0014 /* local resource not available */
#define E_DP_IV 0x0015 /* invalid parameter in request */
#define E_DP_TO 0x0016 /* timeout expired */

#define E_DP_FE 0x00C1 /* format error in request frame */
#define E_DP_NI 0x00C2 /* function not implemented */
#define E_DP_AD 0x00C3 /* access denied */
#define E_DP_EA 0x00C4 /* area too large */
#define E_DP_LE 0x00C5 /* data block length exceeded */
#define E_DP_RE 0x00C6 /* format error in response frame */
#define E_DP_IP 0x00C7 /* invalid parameter */
#define E_DP_SC 0x00C8 /* sequence conflict */
#define E_DP_SE 0x00C9 /* sequence error */
#define E_DP_NE 0x00CA /* area non-existent */
#define E_DP_DI 0x00CB /* data incomplete */
#define E_DP_NC 0x00CC /* not connected */

#define E_DP_DATA_ALIGNMENT 0x0100
#define E_DP_TOO_MANY_SLAVES 0x0200
#define E_DP_WRONG_SLAVE_ADD 0x0300
#define E_DP_AAM_NOT_SUPPORTED 0x0400
#define E_DP_TOO_FEW_DIAG_ENTRIES 0x0500
#define E_DP_WRONG_PRM_DATA_LEN 0x0600
#define E_DP_WRONG_CFG_DATA_LEN 0x0700
#define E_DP_WRONG_DIAG_LEN 0x0800
#define E_DP_WRONG_BUS_PARA_LEN 0x0900
#define E_DP_WRONG_SLAVE_PARA_LEN 0x0A00
#define E_DP_WRONG_IO_DATA_LEN 0x0B00
#define E_DP_NOT_ENOUGH_MEMORY 0x0C00
#define E_DP_WRONG_USIF_STATE 0x0D00
#define E_DP_SLAVE_ACCESS_DENIED 0x0E00
#define E_DP_WRONG_AREA_CODE 0x0F00
#define E_DP_NOT_SUPPORTED 0x1000
#define E_DP_PRM_DATA_FAULT 0x1100
#define E_DP_CFG_DATA_FAULT 0x1200
#define E_DP_AAT_DATA_FAULT 0x1300
#define E_DP_USER_DATA_FAULT 0x1400
#define E_DP_SLAVE_PARA_FAULT 0x1500
#define E_DP_AREA_NOT_ACCESSED 0x1600
#define E_DP_WRONG_BAUDRATE 0x1700
#define E_DP_WRONG_BP_FLAG 0x1800
#define E_DP_WRONG_FDL_STATE 0x1900
#define E_DP_WRONG_ACTIVATION 0x1A00
#define E_DP_WRONG_MASTER_ADD 0x1B00
#define E_DP_DPRAM_INIT_ERROR 0x1C00
#define E_DP_WRONG_LEN 0x1D00
#define E_DP_WRONG_IDENTIFIER 0x1F00
#define E_DP_LOAD_BUS_PARAMETER 0x2000
#define E_DP_ACTIVATE_SAP 0x2100
#define E_DP_WRONG_REMOTE_SERVICE 0x2200
#define E_DP_ILLEGAL_INDEX 0x2300
#define E_DP_ILLEGAL_SLOT 0x2400
#define E_DP_ILLEGAL_LENGTH 0x2500
#define E_DP_ILLEGAL_EXTENSION 0x2600

/****************************************************************************/
/*************    DP ERROR CODE DEFINITIONS      ****************************/
/****************************************************************************/

/*--- DPS SERVICE RESULTS ---------------------------------------------------*/

#define E_DPS_OK 0x0000 /* acknowledgement positive */
#define E_DPS_UE 0x0001 /* remote user error */
#define E_DPS_RR 0x0002 /* remote resource insufficient */
#define E_DPS_RS 0x0003 /* remote service/SAP deactivated */
#define E_DPS_RA 0x0004 /* access of remote SAP blocked */
#define E_DPS_NA 0x0011 /* no reaction from remote station */
#define E_DPS_DS 0x0012 /* local entity disconnected */
#define E_DPS_NO 0x0013 /* not possible in this state */
#define E_DPS_LR 0x0014 /* local resource not available */
#define E_DPS_IV 0x0015 /* invalid parameter in request */
#define E_DPS_TO 0x0016 /* timeout expired */

#define E_DPS_FE 0x00C1 /* format error in request frame */
#define E_DPS_NI 0x00C2 /* function not implemented */
#define E_DPS_AD 0x00C3 /* access denied */
#define E_DPS_EA 0x00C4 /* area too large */
#define E_DPS_LE 0x00C5 /* data block length exceeded */
#define E_DPS_RE 0x00C6 /* format error in response frame */
#define E_DPS_IP 0x00C7 /* invalid parameter */
#define E_DPS_SC 0x00C8 /* sequence conflict */
#define E_DPS_SE 0x00C9 /* sequence error */
#define E_DPS_NE 0x00CA /* area non-existent */
#define E_DPS_DI 0x00CB /* data incomplete */
#define E_DPS_NC 0x00CC /* not connected */

/*--- DPS ERROR EXTENSIONS --------------------------------------------------*/

#define E_DPS_DUPLICATED_SERVICE 0x0100
#define E_DPS_WRONG_SLAVE_ADDRESS 0x0200
#define E_DPS_WRONG_CFG_LEN 0x0300
#define E_DPS_WRONG_PRM_LEN 0x0400
#define E_DPS_WRONG_DIAG_LEN 0x0500
#define E_DPS_WRONG_SSA_LEN 0x0600
#define E_DPS_WRONG_ENHANCED_INIT_LEN 0x0700
#define E_DPS_ILLEGAL_CFG_DATA 0x0800
#define E_DPS_NOT_ENOUGH_FRAME_MEMORY 0x0900
#define E_DPS_WRONG_INPUT_LEN 0x0A00
#define E_DPS_WRONG_OUTPUT_LEN 0x0B00
#define E_DPS_INVALID_DIAG_STATE 0x0C00
#define E_DPS_INVALID_EXT_DIAG_DATA 0x0D00
#define E_DPS_SSA_REQUIRED 0x0E00
#define E_DPS_SSA_NOT_SUPPORTED 0x0F00

/*****************************************************************************/
/*************   DP/V1 ERROR PDU CODING                  *********************/
/*****************************************************************************/

#define DP_ERROR_DECODE_DPV1 128 /* error_decode */
#define DP_ERROR_DECODE_FMS 254
#define DP_ERROR_DECODE_HART 255

/* error_code_1 */

/* If error_decode is DP_ERROR_DECODE_DPV1, the following is valid */
/* error classes 0 to 9 are reserved, classes 13 to 15 are slave specific */

#define DP_ERROR_CLASS_APPLICATION 0xA0
/* errors 3 to 7 are reserved, errors 10 to 15 are slave specific */
#define DP_ERROR_APP_READ (DP_ERROR_CLASS_APPLICATION | 0x00)
#define DP_ERROR_APP_WRITE (DP_ERROR_CLASS_APPLICATION | 0x01)
#define DP_ERROR_APP_MODUL_FAILURE (DP_ERROR_CLASS_APPLICATION | 0x02)
#define DP_ERROR_APP_VERSION_CONFLICT (DP_ERROR_CLASS_APPLICATION | 0x08)
#define DP_ERROR_APP_FEATURE (DP_ERROR_CLASS_APPLICATION | 0x09)

#define DP_ERROR_CLASS_ACCESS 0xB0
/* errors 10 to 15 are slave specific */
#define DP_ERROR_ACCESS_INVALID_INDEX (DP_ERROR_CLASS_ACCESS | 0x00)
#define DP_ERROR_ACCESS_INVALID_LENGTH (DP_ERROR_CLASS_ACCESS | 0x01)
#define DP_ERROR_ACCESS_INVALID_SLOT (DP_ERROR_CLASS_ACCESS | 0x02)
#define DP_ERROR_ACCESS_TYPE_CONFLICT (DP_ERROR_CLASS_ACCESS | 0x03)
#define DP_ERROR_ACCESS_INVALID_AREA (DP_ERROR_CLASS_ACCESS | 0x04)
#define DP_ERROR_ACCESS_STATE_CONFLICT (DP_ERROR_CLASS_ACCESS | 0x05)
#define DP_ERROR_ACCESS_DENIED_ACCESS (DP_ERROR_CLASS_ACCESS | 0x06)
#define DP_ERROR_ACCESS_INVALID_RANGE (DP_ERROR_CLASS_ACCESS | 0x07)
#define DP_ERROR_ACCESS_INVALID_PARAM (DP_ERROR_CLASS_ACCESS | 0x08)
#define DP_ERROR_ACCESS_INVALID_TYPE (DP_ERROR_CLASS_ACCESS | 0x09)

#define DP_ERROR_CLASS_RESOURCE 0xC0
/* errors 4 to 7 are reserved, errors 8 to 15 are slave specific */
#define DP_ERROR_RES_READ_CONFLICT (DP_ERROR_CLASS_RESOURCE | 0x00)
#define DP_ERROR_RES_WRITE_CONFLICT (DP_ERROR_CLASS_RESOURCE | 0x01)
#define DP_ERROR_RES_BUSY (DP_ERROR_CLASS_RESOURCE | 0x02)
#define DP_ERROR_RES_UNAVAILABLE (DP_ERROR_CLASS_RESOURCE | 0x03)

/* error_code_2 */
/* If error_decode is DP_ERROR_DECODE_DPV1, the following is valid */
/* error_code_2 is completely slave specific */

/*****************************************************************************/
/*************            INTERFACE ERRORS               *********************/
/*****************************************************************************/

#if (PB_VER < 500)
#define E_FATAL_ERROR 7            /* unrecoverable error on board */
#define E_INIT_INVALID_PARAMETER 8 /* invalid initialize parameter*/

#define E_LOADER_ERROR 9 /* download error */

#define E_NO_CNTRL_RES 10               /* controller does not respond */
#define E_INVALID_CNTRL_TYPE_VERSION 11 /* inv. controller type or SW vers.*/
#define E_INVALID_LAYER 12              /* invalid layer */
#define E_INVALID_SERVICE 13            /* invalid service identifier */
#define E_INVALID_PRIMITIVE 14          /* invalid service primitive */
#define E_INVALID_DATA_SIZE 15          /* not enough cmi data block memory*/
#define E_INVALID_COMM_REF 16           /* invalid communication reference */
#define E_INVALID_FMS_COMM_REF 17       /* invalid FMS comm. reference */
#define E_INVALID_FMA_COMM_REF 18       /* invalid FMA7 comm. reference */
#define E_INVALID_CMI_CALL 19           /* invalid CMI call */
#define E_CMI_ERROR 20                  /* error occured in CMI */
#define E_RESOURCE_UNAVAILABLE 21       /* no resource available */
#define E_NO_PARALLEL_SERVICES 22       /* no parallel services allowed */
#define E_SERVICE_CONSTR_CONFLICT 23    /* serv. tempor. not executable */
#define E_SERVICE_NOT_SUPPORTED 24      /* service not supported */
#define E_SERVICE_NOT_EXECUTABLE 25     /* service not executable */
#endif

#define E_IF_FATAL_ERROR 7                 /* unrecoverable error on board */
#define E_IF_INIT_INVALID_PARAMETER 8      /* invalid initialize parameter */
#define E_IF_LOADER_ERROR 9                /* download firmware error */
#define E_IF_NO_CNTRL_RES 10               /* controller does not respond */
#define E_IF_INVALID_CNTRL_TYPE_VERSION 11 /* inv. controller type or SW       \
                                              vers.*/
#define E_IF_INVALID_LAYER 12              /* invalid layer */
#define E_IF_INVALID_SERVICE 13            /* invalid service identifier */
#define E_IF_INVALID_PRIMITIVE 14          /* invalid service primitive */
#define E_IF_INVALID_DATA_SIZE 15          /* not enough cmi data block memory*/
#define E_IF_INVALID_COMM_REF 16           /* invalid communication reference */
#define E_IF_INVALID_CMI_CALL 19           /* invalid CMI call */
#define E_IF_CMI_ERROR 20                  /* error occured in CMI */
#define E_IF_RESOURCE_UNAVAILABLE 21       /* no resource available */
#define E_IF_NO_PARALLEL_SERVICES 22       /* no parallel services allowed */
#define E_IF_SERVICE_CONSTR_CONFLICT 23    /* serv. tempor. not executable */
#define E_IF_SERVICE_NOT_SUPPORTED 24      /* service not supported */
#define E_IF_SERVICE_NOT_EXECUTABLE 25     /* service not executable */
#define E_IF_INVALID_VERSION 26            /* invalid version */
#define E_IF_STATE_CONFLICT 27             /* state conflict  */
#define E_IF_NO_CNTRL_PRESENT 28           /* controller not available */
#define E_IF_INVALID_PARAMETER 30          /* wrong parameter in REQ or RES */
#define E_IF_INIT_FAILED 31                /* init. API or Controller failed */
#define E_IF_EXIT_FAILED 32                /* exit API or Controller failed */
#define E_IF_PAPI_NOT_INITIALIZED 33       /* API not initialized */

#define E_IF_NO_DEVICE_CONNECTION 34 /* no PROFIBUS device connection */

/* error codes available only in Win NT ---------------------------------------
 */
#define E_IF_SLAVE_DIAG_DATA 0xF0  /* no data available */
                                   /* new diagnostics data available */
#define E_IF_SLAVE_ERROR 0xF1      /* no data exchange */
#define E_IF_INVALID_DP_STATE 0xF2 /* DP is not in state clear/operate*/
#define E_IF_READING_REGISTRY 0xF3 /* error reading registry */
#define E_IF_SOCKET_ERROR 0xFE     /* TCP socket error */
                                   /* get detail with WSAGetLastError */
#define E_IF_OS_ERROR 0xFF         /* OS system (WIN,DOS) error */
                                   /* get detail with GetLastError */

/* INTERFACE ERROR DETAIL CODES (only Win95/98) -------------------------------
 */
/* for folLowing INTERFACE-ERRORS */
/* - E_IF_NO_CNTRL_PRESENT */
/* - E_IF_INIT_FAILED */
/* - E_IF_EXIT_FAILED */

#define PBDRV PROFIBRD
#define PROFIBRD_INTERNAL_ERROR 0x0030       /* internal driver error */
#define PROFIBRD_GENERAL_ERROR 0x0031        /* general driver error */
#define PROFIBRD_IO_PENDING 0x0032           /* driver timeout */
#define PROFIBRD_IO_CANCELLED 0x0033         /* driver call not finished */
#define PROFIBRD_ILLEGAL_CALL 0x0034         /* driver call cancelled */
#define PROFIBRD_NOT_SUPPORTED 0x0035        /* driver call not supported */
#define PROFIBRD_VERSION_ERROR 0x0036        /* driver interface version  */
#define PROFIBRD_DRIVER_VERSION_ERROR 0x0037 /* driver version */
#define PROFIBRD_DRIVER_NOT_FOUND 0x0038     /* driver not found */
#define PROFIBRD_NOT_ENOUGH_MEMORY 0x0039    /* not enough memory */
#define PROFIBRD_TOO_MANY_DEVICES 0x003A     /* to many devices */
#define PROFIBRD_UNKNOWN_DEVICE 0x003B       /* unknown device */
#define PROFIBRD_DEVICE_ACCESS_ERROR 0x003C  /* device access error */
#define PROFIBRD_DPRAM_ALREADY_USED 0x003D   /* device already exists */
#define PROFIBRD_MAP_MEMORY_ERROR 0x003E     /* memory mapping error */
#define PROFIBRD_MEMORY_UNKNOWN 0x003F       /* unknown phys. memory */
#define PROFIBRD_READ_REGISTRY_ERROR 0x0040  /* read registry error */
#define PROFIBRD_DEVICE_NOT_FOUND 0x0041     /* device not found */
#define PROFIBRD_DEVICE_ALREADY_OPEN 0x0042  /* device already opened */
#define PROFIBRD_PORT_ALREADY_USED 0x0043    /* IO port already in use */

/*****************************************************************************/
/*************        ERROR DATA STRUCTURES      *****************************/
/*****************************************************************************/

#define MAX_ERROR_DESCR_LENGTH _NAME_LENGTH(ERROR_DESCR_LENGTH)

/* --- standard error data structure --------------------------------------- */
typedef struct _T_ERROR
{
  USIGN16 class_code;                              /* class and code         */
  INT16 add_detail;                                /* additional detail      */
  STRINGV add_description[MAX_ERROR_DESCR_LENGTH]; /* additional description */
} T_ERROR;

/* --- PI error data structure --------------------------------------------- */
typedef struct _T_PI_ERROR
{
  T_ERROR error;   /*  standard error type    */
  USIGN8 pi_state; /*  pi state               */
  USIGN8 dummy;    /*  alignment              */
} T_PI_ERROR;

/* --- PI-LOC error data structure ----------------------------------------- */
typedef struct _T_PI_LOC_ERROR
{
  T_ERROR error;      /*  standard error type    */
  USIGN8 pi_state;    /*  pi state               */
  USIGN8 dummy;       /*  alignment              */
  USIGN32 vfd_number; /*  vfd number             */
} T_PI_LOC_ERROR;

/* --- OD error data structure --------------------------------------------- */
typedef struct _T_OD_ERROR
{
  T_ERROR error; /* standard error type     */
  USIGN16 index; /* error index             */
} T_OD_ERROR;

#if (PB_VER < 500)
typedef T_OD_ERROR T_OV_ERROR;
#endif

/* --- source OD error data structure -------------------------------------- */
typedef struct _T_SRC_OD_ERROR
{
  T_ERROR error;      /* standard error type     */
  USIGN32 vfd_number; /* vfd number              */
  USIGN16 index;      /* error index             */
} T_SRC_OD_ERROR;

#if (PB_VER < 500)
typedef T_SRC_OD_ERROR T_SRC_OV_ERROR;
#endif

/* --- VFD error data structure -------------------------------------------- */
typedef struct _T_VFD_ERROR
{
  T_ERROR error;      /* standard error type     */
  USIGN32 vfd_number; /* vfd number              */
} T_VFD_ERROR;

/* --- CRL error data structure -------------------------------------------- */
typedef struct _T_CRL_ERROR
{
  T_ERROR error;    /* standard error type     */
  USIGN16 error_cr; /* error cr                */
} T_CRL_ERROR;

#if (PB_VER < 500)
typedef T_CRL_ERROR T_KBL_ERROR;
#endif

/*****************************************************************************/
/*************   FDLIF-ERROR DATA STRUCTURES     *****************************/
/*****************************************************************************/
typedef struct _T_FDLIF_ERROR
{
  USIGN8 result;      /* class and code */
  USIGN8 dummy;       /* alignment byte */
  USIGN16 add_detail; /* additional detail */
} T_FDLIF_ERROR;

/*****************************************************************************/
/*************      EXCEPTION STRUCTURES         *****************************/
/*****************************************************************************/

typedef struct _T_EXCEPTION
{
  USIGN8 task_id; /* task identifier in which execption occurs */
  USIGN8 par1;    /* parameter 1 */
  USIGN16 par2;   /* parameter 2 */
  USIGN16 par3;   /* parameter 3 */
} T_EXCEPTION;

#if defined(WIN32) || defined(_WIN32) || defined(WIN16) || defined(_WIN16)
#pragma warning(disable : 4103) /* used #pragma pack to reset alignment */
#ifdef WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif
#pragma warning(default : 4103)
#endif

#endif
