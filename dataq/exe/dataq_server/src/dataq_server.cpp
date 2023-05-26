
#include <unistd.h>
#include <ctype.h>

#include "pwr.h"
#include "co_cdh.h"
#include "co_time.h"
#include "co_string.h"
#include "co_error.h"
#include "rt_gdh.h"
#include "rt_ini_event.h"
#include "rt_plc_utl.h"
#include "rt_qcom_msg.h"
#include "rt_gdh_msg.h"
#include "rs_dataq_msg.h"
#include "dataq_server.h"
#include "dataq_net.h"
      
static int debug = 0;

unsigned int qsrv_data_converter::feedback_size()
{
  unsigned int size = 0;
  for (unsigned int j = 0; j < conv.size(); j++) {
    if (conv[j].feedback)
      size++;
  }  
  return size;
}

//
//  Create conversion table
//
pwr_tStatus qsrv_data_converter::create_table(pwr_tCid target_cid, pwr_tCid source_cid)
{
  pwr_tStatus sts;
  gdh_sAttrDef *target_bd;
  int target_rows;
  pwr_tAttrRef aref;
  pwr_tTime now;

  time_GetTime(&now);
  version = now.tv_sec;

  sts = gdh_GetObjectBodyDef(target_cid, &target_bd, &target_rows, pwr_cNOid);
  if (EVEN(sts))
    return sts;

  for (int j = 0; j < target_rows; j++) {
    if (target_bd[j].attrClass == pwr_eClass_TargetAttribute) {
      aref = target_bd[j].attr->TargetAttribute.SourceAttr;
      if (aref.Objid.oix != source_cid) {
	free(target_bd);	
	return DATAQ__SOURCEMISMATCH;
      }
      qsrv_convert_table ct;
      conv.push_back(ct);
      int k = conv.size() - 1;
      conv[k].tid = target_bd[j].attr->TargetAttribute.Info.Type;
      conv[k].feedback = target_bd[j].attr->TargetAttribute.Feedback;
      conv[k].target_offset = target_bd[j].attr->TargetAttribute.Info.Offset;
      conv[k].source_offset = aref.Offset;
      conv[k].size = target_bd[j].attr->TargetAttribute.Info.Size;
    }
  }
  free(target_bd);
  return DATAQ__SUCCESS;
}

//
//  Write conversion table and data attributes to message
//
void qsrv_data_converter::table_and_data_to_msg(char *mp, char *sourcep, unsigned int *msize)
{
  char *ap;

  *msize = 0;
  for (unsigned int j = 0; j < conv.size(); j++) {
    ((dataq_sMsgDataAttr*)mp)->TypeId = conv[j].tid;
    ((dataq_sMsgDataAttr*)mp)->Feedback = conv[j].feedback;
    ((dataq_sMsgDataAttr*)mp)->TargetOffset = conv[j].target_offset;
    ((dataq_sMsgDataAttr*)mp)->SourceOffset = conv[j].source_offset;
    ((dataq_sMsgDataAttr*)mp)->Size = conv[j].size;
    mp += sizeof(dataq_sMsgDataAttr);
    ap = sourcep + conv[j].source_offset;
    memcpy(mp, ap, conv[j].size);
    mp += conv[j].size;
    *msize += sizeof(dataq_sMsgDataAttr) + conv[j].size;
  }
}

//
//  Write conversion table to message
//
void qsrv_data_converter::table_to_msg(char *mp, unsigned int *msize)
{
  *msize = 0;
  for (unsigned int j = 0; j < conv.size(); j++) {
    ((dataq_sMsgDataAttr*)mp)->TypeId = conv[j].tid;
    ((dataq_sMsgDataAttr*)mp)->Feedback = conv[j].feedback;
    ((dataq_sMsgDataAttr*)mp)->TargetOffset = conv[j].target_offset;
    ((dataq_sMsgDataAttr*)mp)->SourceOffset = conv[j].source_offset;
    ((dataq_sMsgDataAttr*)mp)->Size = conv[j].size;
    mp += sizeof(dataq_sMsgDataAttr);
    *msize += sizeof(dataq_sMsgDataAttr);
  }
}

//
//  Get size of message containing table and data attributes
//
unsigned int qsrv_data_converter::table_and_data_msg_size()
{
  unsigned int msize = 0;
  for (unsigned int j = 0; j < conv.size(); j++)
    msize += sizeof(dataq_sMsgDataAttr) + conv[j].size;
  
  return msize;
}

//
//  Get size of message containing attributes values
//
unsigned int qsrv_data_converter::data_msg_size()
{
  unsigned int msize = 0;
  for (unsigned int j = 0; j < conv.size(); j++)
    msize += conv[j].size;
  
  return msize;
}

//
//  Get size of message containing feedback attributes values
//
unsigned int qsrv_data_converter::feedback_data_msg_size()
{
  unsigned int msize = 0;
  for (unsigned int j = 0; j < conv.size(); j++) {
    if (conv[j].feedback)
      msize += conv[j].size;
  }  
  return msize;
}

//
//  Create conversion table from message
//
void qsrv_data_converter::msg_to_table(char *mp, unsigned int attr_num, unsigned int *msize)
{
  for (unsigned int i = 0; i < attr_num; i++) {
    qsrv_convert_table ct;
    conv.push_back(ct);
    conv[i].source_offset = ((dataq_sMsgDataAttr*)mp)->SourceOffset;
    conv[i].target_offset = ((dataq_sMsgDataAttr*)mp)->TargetOffset;
    conv[i].size = ((dataq_sMsgDataAttr*)mp)->Size;
    conv[i].tid = ((dataq_sMsgDataAttr*)mp)->TypeId;
    conv[i].feedback = ((dataq_sMsgDataAttr*)mp)->Feedback;

    mp += sizeof(dataq_sMsgDataAttr);
  }
  *msize = attr_num * sizeof(dataq_sMsgDataAttr);
}

void qsrv_data_converter::msg_to_target_data(char *mp, char *op, unsigned int *msize)
{
  *msize = 0;
  for (unsigned int i = 0; i < conv.size(); i++) {
    memcpy(op + conv[i].target_offset, mp, conv[i].size); 
    mp += conv[i].size;
    *msize += conv[i].size;
  }
}

void qsrv_data_converter::msg_to_feedback_data(char *mp, char *op, unsigned int *msize)
{
  *msize = 0;
  for (unsigned int i = 0; i < conv.size(); i++) {
    if (conv[i].feedback) {
      memcpy(op + conv[i].source_offset, mp, conv[i].size); 
      mp += conv[i].size;
      *msize += conv[i].size;
    }
  }
}

void qsrv_data_converter::source_data_to_msg(char *mp, char *op, unsigned int *msize)
{
  *msize = 0;
  for (unsigned int i = 0; i < conv.size(); i++) {
    memcpy(mp, op + conv[i].source_offset, conv[i].size); 
    mp += conv[i].size;
    *msize += conv[i].size;
  }
}

void qsrv_data_converter::feedback_data_to_msg(char *mp, char *op, unsigned int *msize)
{
  *msize = 0;
  for (unsigned int i = 0; i < conv.size(); i++) {
    if (conv[i].feedback) {
      memcpy(mp, op + conv[i].target_offset, conv[i].size); 
      mp += conv[i].size;
      *msize += conv[i].size;
    }
  }
}

pwr_tStatus dataq_server::remote_name(char* source_name, char* format, char *remote_name, 
    unsigned int size)
{
  int num;
  int name_number;
  char *s;
  int found;

  found = 0;
  for (s = source_name; *s; s++) {
    if (isdigit(*s)) {
      found = 1;
      break;
    }
  }
  if (found) {
    num = sscanf(s, "%d", &name_number);
    if (num != 1)
      return DATAQ__INVALIDNAME;
  }
  else
    return DATAQ__INVALIDNAME;

  snprintf(remote_name, size, format, name_number);
  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::node_up(pwr_tNid nid)
{
  dataq_sMsgAny* msg;
  qcom_sQid tgt;
  qcom_sPut put;
  pwr_tStatus sts, lsts;

  tgt.nid = nid;
  tgt.qix = dataq_cProcServer;

  put.reply.nid = m_nodes[0].nid;
  put.reply.qix = dataq_cProcServer;
  put.type.b = (qcom_eBtype)dataq_cMsgClass;
  put.type.s = (qcom_eStype)dataq_eMsgType_NodeUpR;
  put.msg_id = m_msg_id++;
  put.size = sizeof(*msg);
  msg = (dataq_sMsgAny*)qcom_Alloc(&lsts, put.size);

  msg->Type = dataq_eMsgType_NodeUpR;
  msg->Version = dataq_cNetVersion;
  put.data = msg;
  put.allocate = 0;

  if (!qcom_Put(&sts, &tgt, &put)) {
    qcom_Free(&sts, put.data);
  }

  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::node_up_reply(pwr_tNid nid)
{
  for (unsigned int i = 0; i < m_nodes.size(); i++) {
    if (m_nodes[i].nid == nid) {
      m_nodes[i].connection = pwr_eUpDownEnum_Up;
      break;
    }
  }
  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::rdataq_init_msg(dataq_sMsgRDataQInit* mp, int size, pwr_tNid reply_nid)
{
  if (debug)
    printf("RDataQ_Init received %d\n", size);

  int tix;
  int found;
  char *dp;
  pwr_tStatus sts;
  unsigned int msize;

  found = 0;
  for (int i = 0; i < m_tdataq.size(); i++) {
    if (cdh_ObjidIsEqual(mp->RDataQ.Objid, m_tdataq[i].rdataq.Objid) &&
	mp->RDataQ.Offset == m_tdataq[i].rdataq.Offset) {
      tix = i;
      m_tdataq[tix].dc.conv.clear();
      m_tdataq[tix].targetq.clear();
      found = 1;
    }
  }

  if (!found) {
    qsrv_tdataq tdataq;
    m_tdataq.push_back(tdataq);
    tix = m_tdataq.size() - 1;
  }

  m_tdataq[tix].reply_nid = reply_nid;
  m_tdataq[tix].rdataq = mp->RDataQ;
  strncpy(m_tdataq[tix].source_dataname, mp->SourceDataName, sizeof(m_tdataq[0].source_dataname));

  dp = (char*)&mp->Data;

  for (unsigned int i = 0; i < mp->RemoteQNum; i++) {
    qsrv_tdataq_targetq targetq;
    m_tdataq[tix].targetq.push_back(targetq);
    m_tdataq[tix].targetq[i].aref = ((dataq_sMsgRDataQ_RemoteQ*)dp)->Aref;
    m_tdataq[tix].targetq[i].options = ((dataq_sMsgRDataQ_RemoteQ*)dp)->Options;
    m_tdataq[tix].targetq[i].start = ((dataq_sMsgRDataQ_RemoteQ*)dp)->Start;
    m_tdataq[tix].targetq[i].end = ((dataq_sMsgRDataQ_RemoteQ*)dp)->End;

    // Direct link to targetq object
    sts = gdh_DLRefObjectInfoAttrref(&m_tdataq[tix].targetq[i].aref, 
	(pwr_tAddress*)&m_tdataq[tix].targetq[i].op, &m_tdataq[tix].targetq[i].dlid);
    if (EVEN(sts)) {
      errh_Error("RemoteDataQ target queue error, %m, %s", sts);
      m_tdataq[tix].targetq.pop_back();
      continue;
    }
    m_tdataq[tix].targetq[i].op->Super.Intern.RQStatus |= pwr_mRemoteDataQStatusMask_IsTarget;
    if (m_tdataq[tix].targetq[i].options & pwr_mTargetDataQOptionsMask_SendFeedback ||
	m_tdataq[tix].targetq[i].options & pwr_mTargetDataQOptionsMask_TriggerFeedback)
      m_tdataq[tix].targetq[i].op->Super.Intern.RQStatus |= pwr_mRemoteDataQStatusMask_IsFeedbackTarget;

    dp += sizeof(dataq_sMsgRDataQ_RemoteQ);
  }

  m_tdataq[tix].dc.msg_to_table(dp, mp->AttrNum, &msize);
  m_tdataq[tix].dc.version = mp->TableVersion;
  m_tdataq[tix].msg_size = m_tdataq[tix].dc.data_msg_size();

  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::rdataq_feedback(dataq_sMsgRDataQFeedback* mp, 
    int size, pwr_tNid reply_nid)
{
  pwr_tStatus sts;
  int found;
  int rix;
  char *op;
  unsigned int msize;

  if (debug)
    printf("RDataQFeedback received %d\n", size);

  // Find source RDataQ
  found = 0;
  for (unsigned int i = 0; i < m_rdataq.size(); i++) {
    if (cdh_ObjidIsEqual(mp->RDataQ.Objid, m_rdataq[i].aref.Objid)) {
      rix = i;
      found = 1;
      break;
    }
  }
  if (!found)
    return 0;

  if (m_rdataq[rix].dc.version != mp->TableVersion)
    return 0;

  if (mp->FeedbackData) {
    sts = gdh_NameToPointer(mp->DataName, (void **)&op);
    if (EVEN(sts)) {
      m_rdataq[rix].op->MsgStatus = sts;
      return sts;
    }
    m_rdataq[rix].dc.msg_to_feedback_data((char*)&mp->Data, op, &msize);
  }
  if (mp->FeedbackTrigger)
    m_rdataq[rix].op->Feedback = 1;
  m_rdataq[rix].op->MsgStatus = DATAQ__SUCCESS;

  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::rdataq_reset(dataq_sMsgRDataQReset* mp, 
    int size, pwr_tNid reply_nid)
{
  int found;

  if (debug)
    printf("RDataQReset received %d\n", size);

  for (unsigned int i = 0; i < m_tdataq.size(); i++) {
    if (cdh_ObjidIsEqual(mp->RemoteDataQ.Objid, m_tdataq[i].rdataq.Objid) &&
	mp->RemoteDataQ.Offset == m_tdataq[i].rdataq.Offset) {
      found = 1;
      for (unsigned int j = 0; j < m_tdataq[i].targetq.size(); j++) {
	m_tdataq[i].targetq[j].op->Super.Intern.QReset = 1;
      }
    }
  }

  return DATAQ__SUCCESS;
}

//
// Remote dataq message received
//
pwr_tStatus dataq_server::rdataq(dataq_sMsgRDataQ* mp, int size, pwr_tNid reply_nid)
{
  if (debug)
    printf("RDataQ received %d %s\n", size, mp->DataName);

  int tix;
  int qix;
  int found;
  pwr_tStatus sts;
  pwr_tCid cid;
  pwr_tOid oid;
  char *op;
  unsigned int msize;

  found = 0;
  for (int i = 0; i < m_tdataq.size(); i++) {
    if (cdh_ObjidIsEqual(mp->RDataQ.Objid, m_tdataq[i].rdataq.Objid)) {
      found = 1;
      tix = i;
      break;
    }
  }
  if (!found)
    return 0;

  if (m_tdataq[tix].dc.version != mp->TableVersion)
    return 0;

  if (/* NewData */ 1) {
    if (mp->Options & pwr_mRemoteDataQOptionsMask_AttachData) {
      sts = gdh_NameToObjid(mp->DataName, &oid);
      if (ODD(sts))
	sts = gdh_GetObjectClass(oid, &cid);
      if (ODD(sts) && cid != mp->DataClass)
	sts = DATAQ__DATACLASS;
    } else {
      // Create data object
      sts = gdh_CreateObject(mp->DataName, mp->DataClass, 0, &oid, 
	  pwr_cNOid, 0, pwr_cNOid);
      if (sts == GDH__DUPLNAME && 
	  mp->Options & pwr_mRemoteDataQOptionsMask_AttachOrCreateData) {
	sts = gdh_NameToObjid(mp->DataName, &oid);
	if (ODD(sts))
	  sts = gdh_GetObjectClass(oid, &cid);
	if (ODD(sts) && cid != mp->DataClass)
	  sts = DATAQ__DATACLASS;
      }
    }
    if (EVEN(sts)) {
      errh_Error("Target queue create object error, %m", sts);
      return sts;
    }
    sts = gdh_ObjidToPointer(oid, (void **)&op);
    if (EVEN(sts)) {
      errh_Error("Target queue attach object error, %m", sts);
      return sts;
    }

    m_tdataq[tix].dc.msg_to_target_data((char*)&mp->Data, op, &msize);

    // Insert data object into target queue
    found = 0;
    for (int j = 0; j < m_tdataq[tix].targetq.size(); j++) {
      if (m_tdataq[tix].targetq[j].start) {
	found = 1;
	qix = j;
	break;
      }
    }
    if (!found)
      return 0;

    m_tdataq[tix].targetq[qix].op->Super.Control.Objid = oid;
    m_tdataq[tix].targetq[qix].op->Super.Control.Operation = pwr_eDataQCtlEnum_Insert;
    m_tdataq[tix].targetq[qix].op->Super.Control.Commit = 1;
    
  }
  return DATAQ__SUCCESS;
}

//
// Remote order init  message received
//

pwr_tStatus dataq_server::rorder_init_msg(dataq_sMsgROrderInit* mp, int size, pwr_tNid reply_nid)
{
  int found;
  int tix;
  unsigned int msize;

  if (debug)
    printf("ROrderInit received %d\n", size);

  // Find target order
  found = 0;
  for (unsigned int i = 0; i < m_torder.size(); i++) {
    if (cdh_ObjidIsEqual(mp->Target, m_torder[i].oid)) {
      tix = i;
      found = 1;
    }
  }
  if (!found)
    return 0;

  m_torder[tix].rorder = mp->ROrder;
  m_torder[tix].options = mp->Options;
  m_torder[tix].data_class = mp->DataClass;
  m_torder[tix].reply_nid = reply_nid;

  m_torder[tix].dc.msg_to_table((char*)&mp->Data[0], mp->AttrNum, &msize);

  m_torder[tix].dc.version = mp->TableVersion;
  
  m_torder[tix].op->MsgStatus = DATAQ__SUCCESS;
  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::rorder(dataq_sMsgROrder* mp, int size, pwr_tNid reply_nid)
{
  pwr_tStatus sts;
  pwr_tOid oid;
  pwr_tCid cid;
  int found;
  int tix;
  char *op;
  unsigned int msize;

  if (debug)
    printf("ROrder received %d %s\n", size, mp->DataName);

  // Find target order
  found = 0;
  for (unsigned int i = 0; i < m_torder.size(); i++) {
    if (cdh_ObjidIsEqual(mp->Target, m_torder[i].oid)) {
      tix = i;
      found = 1;
    }
  }
  if (!found)
    return 0;

  if (mp->Type == dataq_eMsgType_ROrderStatusReset) {
    if (m_torder[tix].op->Out.Status)
      m_torder[tix].op->QTStatusReset = 1;
  }
  else if (mp->Type == dataq_eMsgType_ROrder) {
    if (m_torder[tix].dc.version != mp->TableVersion)
      return 0;

    if (m_torder[tix].options & pwr_mQRemoteOrderOptionsMask_AttachData) {
      sts = gdh_NameToObjid(mp->DataName, &oid);
      if (ODD(sts))
	sts = gdh_GetObjectClass(oid, &cid);
      if (ODD(sts) && cid != mp->DataClass)
	sts = DATAQ__DATACLASS;
    } else {
      // Create data object
      sts = gdh_CreateObject(mp->DataName, mp->DataClass, 0, &oid, 
	  pwr_cNOid, 0, pwr_cNOid);
      if (sts == GDH__DUPLNAME && 
	  m_torder[tix].options & pwr_mQRemoteOrderOptionsMask_AttachOrCreateData) {
	sts = gdh_NameToObjid(mp->DataName, &oid);
	if (ODD(sts))
	  sts = gdh_GetObjectClass(oid, &cid);
	if (ODD(sts) && cid != mp->DataClass)
	  sts = DATAQ__DATACLASS;
      }
    }
    if (EVEN(sts)) {
      m_torder[tix].op->MsgStatus = sts;
      return sts;
    }
    sts = gdh_ObjidToPointer(oid, (void **)&op);
    if (EVEN(sts)) {
      m_torder[tix].op->MsgStatus = sts;
      return sts;
    }

    m_torder[tix].dc.msg_to_target_data((char*)&mp->Data, op, &msize);

    m_torder[tix].op->DataObject = oid;
    m_torder[tix].op->New = 1;
    m_torder[tix].reply_source = mp->DataSource;
    
    m_torder[tix].op->MsgStatus = DATAQ__SUCCESS;
  }
  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::rorder_feedback(dataq_sMsgROrderFeedback* mp, 
    int size, pwr_tNid reply_nid)
{
  pwr_tStatus sts;
  int found;
  int rix;
  char *op;
  unsigned int msize;

  if (debug)
    printf("ROrderFeedback received %d\n", size);

  // Find source order
  found = 0;
  for (unsigned int i = 0; i < m_rorder.size(); i++) {
    if (cdh_ObjidIsEqual(mp->ROrder, m_rorder[i].oid)) {
      rix = i;
      found = 1;
      break;
    }
  }
  if (!found)
    return 0;

  if (m_rorder[rix].dc.version != mp->TableVersion)
    return 0;

  // Check data object
  if (cdh_ObjidIsNotEqual(mp->DataObject, m_rorder[rix].op->Out.Data.Aref.Objid)) {
    m_rorder[rix].op->MsgStatus = DATAQ__DATAREMOVED;
    // return DATAQ__DATAREMOVED;
  }
  sts = gdh_ObjidToPointer(mp->DataObject, (void **)&op);
  if (EVEN(sts)) {
    m_rorder[rix].op->MsgStatus = sts;
    return sts;
  }

  m_rorder[rix].dc.msg_to_feedback_data((char*)&mp->Data, op, &msize);

  m_rorder[rix].op->MsgStatus = DATAQ__SUCCESS;
  m_rorder[rix].op->QRFeedback = 1;
  return DATAQ__SUCCESS;
}

void dataq_server::rdataq_close()
{
  for (unsigned int i = 0; i < m_rdataq.size(); i++) {
    if (m_rdataq[i].dlid.nid != 0)
      gdh_DLUnrefObjectInfo(m_rdataq[i].dlid);
  }
}

pwr_tStatus dataq_server::rdataq_init()
{
  pwr_sAttrRef aref;
  pwr_tStatus sts;
  int i;
  pwr_tOName name;

  for (sts = gdh_GetClassListAttrRef(pwr_cClass_RemoteDataQ, &aref);
       ODD(sts);
       sts = gdh_GetNextAttrRef(pwr_cClass_RemoteDataQ, &aref, &aref)) {
    qsrv_rdataq rdataq;

    m_rdataq.push_back(rdataq);
    i = m_rdataq.size() - 1;
    m_rdataq[i].aref = aref;

    // Direct link to rdataq object
    sts = gdh_DLRefObjectInfoAttrref(&aref, (pwr_tAddress*)&m_rdataq[i].op, 
        &m_rdataq[i].dlid);
    if (EVEN(sts)) 
      return sts;

    // Direct link to source dataq object
    sts = gdh_DLRefObjectInfoAttrref(&m_rdataq[i].op->SourceDataQ, 
	(pwr_tAddress*)&m_rdataq[i].src_op, &m_rdataq[i].src_dlid);
    if (EVEN(sts)) 
      return sts;

    for (int j = 0; 
	 j < sizeof(m_rdataq[i].op->TargetDataQ)/sizeof(m_rdataq[i].op->TargetDataQ[0]); 
	 j++) {
      qsrv_rdataq_remoteq remoteq;

      if (cdh_ObjidIsNull(m_rdataq[i].op->TargetDataQ[j].TargetDataQ.Objid))
	break;
      m_rdataq[i].remoteq.push_back(remoteq);
      m_rdataq[i].remoteq[j].aref = m_rdataq[i].op->TargetDataQ[j].TargetDataQ;
      m_rdataq[i].remoteq[j].options = m_rdataq[i].op->TargetDataQ[j].Options;
      if (j == 0)
	m_rdataq[i].remoteq[j].start = 1;	
    }
    if (m_rdataq[i].remoteq.size() == 0) {
      m_rdataq[i].op->MsgStatus = DATAQ__INIT;
      m_rdataq.pop_back();
      errh_Error("RemoteDataQ error, no remote queues found, %s", name);
      continue;
    }    

    m_rdataq[i].remoteq[m_rdataq[i].remoteq.size() - 1].end = 1;

    sts = m_rdataq[i].dc.create_table(m_rdataq[i].op->RemoteDataClass, 
        m_rdataq[i].op->DataClass);
    if (EVEN(sts)) {
      m_rorder[i].op->MsgStatus = DATAQ__INIT;
      m_rorder.pop_back();
      errh_Error("RemoteDataClass error, %m, %s", sts, name);
      continue;
    }
    m_rdataq[i].msg_size = m_rdataq[i].dc.data_msg_size();

    pwr_tNodeId nid;
    sts = gdh_GetObjectNodeIndex(m_rdataq[i].remoteq[0].aref.Objid, &nid);
    int found = 0;
    for (unsigned int m = 0; m < m_nodes.size(); m++) {
      if (m_nodes[m].nid == nid) {
	m_rdataq[i].node_idx = m;
	found = 1;
	break;
      }
    }
    if (!found) {
      m_rdataq[i].op->MsgStatus = DATAQ__INIT;
      m_rdataq.pop_back();
      errh_Error("RemoteDataClass error, invalid target node, %s", name);
      continue;
    }    
    m_rdataq[i].src_op->Super.Intern.RQStatus |= pwr_mRemoteDataQStatusMask_HasRemote;
    m_rdataq[i].op->MsgStatus = DATAQ__SUCCESS;
  }
  return DATAQ__SUCCESS;
}

void dataq_server::rorder_close()
{
  for (unsigned int i = 0; i < m_rorder.size(); i++) {
    if (m_rorder[i].dlid.nid != 0)
      gdh_DLUnrefObjectInfo(m_rorder[i].dlid);
  }
}

pwr_tStatus dataq_server::rorder_init()
{
  pwr_sAttrRef aref;
  pwr_tStatus sts;
  pwr_tOid oid;
  int i;
  pwr_tOName name;

  for (sts = gdh_GetClassList(pwr_cClass_QRemoteOrder, &oid);
       ODD(sts);
       sts = gdh_GetNextObject(oid, &oid)) {
    qsrv_rorder rorder;

    m_rorder.push_back(rorder);
    i = m_rorder.size() - 1;
    m_rorder[i].oid = oid;

    sts = gdh_ObjidToName(oid, name, sizeof(name), cdh_mName_pathStrict);
    if (EVEN(sts))
      return sts;

    // Direct link to order object
    aref = cdh_ObjidToAref(oid);
    sts = gdh_DLRefObjectInfoAttrref(&aref, (pwr_tAddress*)&m_rorder[i].op, 
        &m_rorder[i].dlid);
    if (EVEN(sts)) 
      return sts;

    sts = m_rorder[i].dc.create_table(m_rorder[i].op->RemoteDataClass, 
        m_rorder[i].op->DataClass);
    if (EVEN(sts)) {
      m_rorder[i].op->MsgStatus = DATAQ__INIT;
      m_rorder.pop_back();
      errh_Error("RemoteDataClass error, %m, %s", sts, name);
      continue;
    }
    m_rorder[i].msg_size = m_rorder[i].dc.table_and_data_msg_size();


    pwr_tNodeId nid;
    sts = gdh_GetObjectNodeIndex(m_rorder[i].op->Target, &nid);
    int found = 0;
    for (unsigned int m = 0; m < m_nodes.size(); m++) {
      if (m_nodes[m].nid == nid) {
	m_rorder[i].node_idx = m;
	found = 1;
	break;
      }
    }
    if (!found) {
      m_rorder[i].op->MsgStatus = DATAQ__INIT;
      m_rorder.pop_back();
      errh_Error("RemoteDataClass error, invalid target node, %s", name);
      continue;
    }    
  }

  return DATAQ__SUCCESS;
}

void dataq_server::torder_close()
{
  for (unsigned int i = 0; i < m_torder.size(); i++) {
    if (m_torder[i].dlid.nid != 0)
      gdh_DLUnrefObjectInfo(m_torder[i].dlid);
  }
}

pwr_tStatus dataq_server::torder_init()
{
  pwr_sAttrRef aref;
  pwr_tStatus sts;
  pwr_tOid oid;
  int i;

  for (sts = gdh_GetClassList(pwr_cClass_QTargetOrder, &oid);
       ODD(sts);
       sts = gdh_GetNextObject(oid, &oid)) {
    qsrv_torder torder;

    m_torder.push_back(torder);
    i = m_torder.size() - 1;
    m_torder[i].initialized = 0;
    m_torder[i].oid = oid;

    // Direct link to order object
    aref = cdh_ObjidToAref(oid);
    sts = gdh_DLRefObjectInfoAttrref(&aref, (pwr_tAddress*)&m_torder[i].op, 
        &m_torder[i].dlid);
    if (EVEN(sts)) 
      return sts;
  }

  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::send_rdataq_init(pwr_tNid nid)
{
  qcom_sQid tgt;
  qcom_sPut put;
  pwr_tStatus sts, lsts;  

  // Send init remote dataq
  for (unsigned int i = 0; i < m_rdataq.size(); i++) {
    dataq_sMsgRDataQInit* msg;
    unsigned int size;

    //    if (nid != m_nodes[m_rdataq[i].node_idx].nid) {

      tgt.nid = m_nodes[m_rdataq[i].node_idx].nid;
      tgt.qix = dataq_cProcServer;

      put.reply.nid = m_nodes[0].nid;
      put.reply.qix = dataq_cProcServer;
      put.type.b = (qcom_eBtype)dataq_cMsgClass;
      put.type.s = (qcom_eStype)dataq_eMsgType_RDataQInit;
      put.msg_id = m_msg_id++;
      put.size = sizeof(dataq_sMsgRDataQInit) - sizeof(int) + 
          m_rdataq[i].remoteq.size() * sizeof(dataq_sMsgRDataQ_RemoteQ) +
          m_rdataq[i].dc.conv.size() * sizeof(dataq_sMsgDataAttr);
      msg = (dataq_sMsgRDataQInit*)qcom_Alloc(&lsts, put.size);
      
      msg->Type = dataq_eMsgType_RDataQInit;
      msg->Version = dataq_cNetVersion;
      msg->RDataQ = m_rdataq[i].aref;
      strncpy(msg->SourceDataName, m_rdataq[i].op->DataName, sizeof(msg->SourceDataName));
      msg->DataClass = m_rdataq[i].op->RemoteDataClass;
      msg->Options = m_rdataq[i].op->Options;
      msg->TableVersion = m_rdataq[i].dc.version;
      msg->RemoteQNum = m_rdataq[i].remoteq.size();
      msg->AttrNum = m_rdataq[i].dc.conv.size();
    
      char *dp = (char*)&msg->Data;
      for (unsigned int j = 0; j < m_rdataq[i].remoteq.size(); j++) {
	((dataq_sMsgRDataQ_RemoteQ*)dp)->Aref = m_rdataq[i].remoteq[j].aref;
	((dataq_sMsgRDataQ_RemoteQ*)dp)->Options = m_rdataq[i].remoteq[j].options;
	((dataq_sMsgRDataQ_RemoteQ*)dp)->Start = m_rdataq[i].remoteq[j].start;
	((dataq_sMsgRDataQ_RemoteQ*)dp)->End = m_rdataq[i].remoteq[j].end;
	dp += sizeof(dataq_sMsgRDataQ_RemoteQ);
      }

      m_rdataq[i].dc.table_to_msg(dp, &size);

      put.data = msg;
      put.allocate = 0;

      if (!qcom_Put(&sts, &tgt, &put)) {
	qcom_Free(&sts, put.data);
	errh_Info("No connection to %s (%s)", m_nodes[i].name,
	    cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
      } else
	errh_Info("Remote DataQ init sent to %s (%s)", m_nodes[i].name,
            cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
    }
//}
  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::send_rorder_init(pwr_tNid nid)
{
  qcom_sQid tgt;
  qcom_sPut put;
  pwr_tStatus sts, lsts;  

  // Send init remote order
  for (unsigned int i = 0; i < m_rorder.size(); i++) {
    dataq_sMsgROrderInit* msg;
    unsigned int size;

    //    if (nid != m_nodes[m_rdataq[i].node_idx].nid) {

    tgt.nid = m_nodes[m_rdataq[i].node_idx].nid;
    tgt.qix = dataq_cProcServer;

    put.reply.nid = m_nodes[0].nid;
    put.reply.qix = dataq_cProcServer;
    put.type.b = (qcom_eBtype)dataq_cMsgClass;
    put.type.s = (qcom_eStype)dataq_eMsgType_ROrderInit;
    put.msg_id = m_msg_id++;
    put.size = sizeof(dataq_sMsgROrderInit) - sizeof(int) + 
        m_rorder[i].dc.conv.size() * sizeof(dataq_sMsgDataAttr);
    msg = (dataq_sMsgROrderInit*)qcom_Alloc(&lsts, put.size);
      
    msg->Type = dataq_eMsgType_ROrderInit;
    msg->Version = dataq_cNetVersion;
    msg->ROrder = m_rorder[i].oid;
    msg->Target = m_rorder[i].op->Target;
    msg->Options = m_rorder[i].op->Options;
    msg->DataClass = m_rorder[i].op->RemoteDataClass;
    msg->TableVersion = m_rorder[i].dc.version;
    msg->AttrNum = m_rorder[i].dc.size();

    m_rorder[i].dc.table_to_msg((char*)&msg->Data[0], &size);

    put.data = msg;
    put.allocate = 0;

    if (!qcom_Put(&sts, &tgt, &put)) {
      qcom_Free(&sts, put.data);
      errh_Info("No connection to %s (%s)", m_nodes[i].name,
	  cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
    } else
      errh_Info("Remote order init sent to %s (%s)", m_nodes[i].name,
          cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
  }

  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::trans_handler()
{
  qcom_sQid tgt;
  qcom_sPut put;
  pwr_tStatus sts, lsts;  

  // Examine target dataq
  for (unsigned int i = 0; i < m_tdataq.size(); i++) {
    for (unsigned int j = 0; j < m_tdataq[i].targetq.size(); j++) {
      if (m_tdataq[i].targetq[j].op->Super.Intern.RQStatus & pwr_mRemoteDataQStatusMask_NewFeedback) {
	printf("Send feedback...\n");

	pwr_tOid oid;
	pwr_tOName name;
	pwr_tOName dataname;
	dataq_sMsgRDataQFeedback* msg;
	char *targetp;
	unsigned int msize;
      
	tgt.nid = m_tdataq[i].reply_nid;
	tgt.qix = dataq_cProcServer;


	oid = m_tdataq[i].targetq[j].op->Data[0].Data.Aref.Objid;
	sts = gdh_ObjidToPointer(oid, (void **)&targetp); 
	if (EVEN(sts)) {
	  m_tdataq[i].targetq[j].op->Super.Intern.RQStatus &= ~pwr_mRemoteDataQStatusMask_NewFeedback;
	  continue;
	}
	sts = gdh_ObjidToName(oid, name, sizeof(name), cdh_mName_object);
	if (EVEN(sts)) {
	  m_tdataq[i].targetq[j].op->Super.Intern.RQStatus &= ~pwr_mRemoteDataQStatusMask_NewFeedback;
	  continue;
	}
      
	sts = remote_name(name, m_tdataq[i].source_dataname, dataname, sizeof(dataname));
	if (EVEN(sts)) {
	  m_tdataq[i].targetq[j].op->Super.Intern.RQStatus &= ~pwr_mRemoteDataQStatusMask_NewFeedback;
	  continue;	
	}

	put.reply.nid = m_nodes[0].nid;
	put.reply.qix = dataq_cProcServer;
	put.type.b = (qcom_eBtype)dataq_cMsgClass;
	put.type.s = (qcom_eStype)dataq_eMsgType_RDataQFeedback;
	put.msg_id = m_msg_id++;
	put.size = sizeof(dataq_sMsgRDataQFeedback) - sizeof(int) + m_tdataq[i].dc.feedback_data_msg_size();
	msg = (dataq_sMsgRDataQFeedback*)qcom_Alloc(&lsts, put.size);
      
	msg->Type = dataq_eMsgType_RDataQFeedback;
	msg->Version = dataq_cNetVersion;
	msg->RDataQ = m_tdataq[i].rdataq;
	msg->TableVersion = m_tdataq[i].dc.version;
	if (m_tdataq[i].targetq[j].options & pwr_mTargetDataQOptionsMask_SendFeedback) {
	  msg->FeedbackData = 1;
	  strncpy(msg->DataName, dataname, sizeof(msg->DataName));
	  msg->AttrNum = m_tdataq[i].dc.feedback_size();	
	
	  m_tdataq[i].dc.feedback_data_to_msg((char*)&msg->Data, targetp, &msize);
	}
	if (m_tdataq[i].targetq[j].options & pwr_mTargetDataQOptionsMask_TriggerFeedback)
	  msg->FeedbackTrigger = 1;
	  
	put.data = msg;
	put.allocate = 0;

	if (!qcom_Put(&sts, &tgt, &put)) {
	  qcom_Free(&sts, put.data);
	  errh_Info("No connection to %s (%s)", m_nodes[i].name,
	      cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
	} else
	  errh_Info("Connect sent to %s (%s)", m_nodes[i].name,
              cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));

#if 0
	if (m_torder[i].options & pwr_mRemoteDataQOptionsMask_DeleteData) {
	  sts = gdh_DeleteObject(oid);
	  if (EVEN(sts)) {
	    m_tdataq[i].targetq[j].op->Super.Intern.RQStatus &= ~pwr_mRemoteDataQStatusMask_NewFeedback;
	    return sts;
	  }
	}
#endif
	m_tdataq[i].targetq[j].op->Super.Intern.RQStatus &= ~pwr_mRemoteDataQStatusMask_NewFeedback;
      }
    }
  }

  // Examine remote dataq
  for (unsigned int i = 0; i < m_rdataq.size(); i++) {
    if (m_rdataq[i].src_op->Super.Intern.RQStatus & pwr_mRemoteDataQStatusMask_NewData) {
      // Send new data object to first remote queue
      pwr_tOid oid;
      pwr_tObjName name;
      pwr_tOName dataname;
      dataq_sMsgRDataQ* msg;
      char *sourcep;
      unsigned int msize;

      // Link to data object
      oid = m_rdataq[i].src_op->Data[0].Data.Aref.Objid;
      sts = gdh_ObjidToPointer(oid, (void **)&sourcep); 
      if (EVEN(sts))
	continue;

      sts = gdh_ObjidToName(oid, name, sizeof(name), cdh_mName_object);
      if (EVEN(sts))
	continue;
      
      sts = remote_name(name, m_rdataq[i].op->RemoteDataName, dataname, sizeof(dataname));
      if (EVEN(sts))
	continue;

      tgt.nid = m_nodes[m_rdataq[i].node_idx].nid;
      tgt.qix = dataq_cProcServer;

      put.reply.nid = m_nodes[0].nid;
      put.reply.qix = dataq_cProcServer;
      put.type.b = (qcom_eBtype)dataq_cMsgClass;
      put.type.s = (qcom_eStype)dataq_eMsgType_RDataQ;
      put.msg_id = m_msg_id++;
      put.size = sizeof(dataq_sMsgRDataQ) - sizeof(int) + m_rdataq[i].msg_size;
      msg = (dataq_sMsgRDataQ*)qcom_Alloc(&lsts, put.size);
      
      msg->Type = dataq_eMsgType_RDataQ;
      msg->Version = dataq_cNetVersion;
      strncpy(msg->DataName, dataname, sizeof(msg->DataName));
      msg->DataSource = oid;
      msg->RDataQ = m_rdataq[i].aref;
      msg->Target = m_rdataq[i].remoteq[0].aref;
      msg->DataClass = m_rdataq[i].op->RemoteDataClass;
      msg->Options = m_rdataq[i].op->Options;
      msg->TableVersion = m_rdataq[i].dc.version;
      msg->AttrNum = m_rdataq[i].dc.conv.size();

      m_rdataq[i].dc.source_data_to_msg((char*)&msg->Data, sourcep, &msize);

      put.data = msg;
      put.allocate = 0;

      if (!qcom_Put(&sts, &tgt, &put)) {
	qcom_Free(&sts, put.data);
	errh_Info("No connection to %s (%s)", m_nodes[i].name,
	   cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
      } else
	errh_Info("Connect sent to %s (%s)", m_nodes[i].name,
          cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));

      m_rdataq[i].src_op->Super.Intern.RQStatus &= ~pwr_mRemoteDataQStatusMask_NewData;
    }

    if (m_rdataq[i].src_op->Super.Intern.RQReset) {
      dataq_sMsgRDataQReset* msg;

      // Send reset order to all remote queues

      m_rdataq[i].src_op->Super.Intern.RQReset = 0;

      tgt.nid = m_nodes[m_rdataq[i].node_idx].nid;
      tgt.qix = dataq_cProcServer;

      put.reply.nid = m_nodes[0].nid;
      put.reply.qix = dataq_cProcServer;
      put.type.b = (qcom_eBtype)dataq_cMsgClass;
      put.type.s = (qcom_eStype)dataq_eMsgType_RDataQReset;
      put.msg_id = m_msg_id++;
      put.size = sizeof(dataq_sMsgRDataQReset) - sizeof(int) + 
          m_rdataq[i].remoteq.size() * sizeof(pwr_tAttrRef);
      msg = (dataq_sMsgRDataQReset*)qcom_Alloc(&lsts, put.size);
      
      msg->Type = dataq_eMsgType_RDataQReset;
      msg->Version = dataq_cNetVersion;
      msg->RemoteDataQ = m_rdataq[i].aref;

      put.data = msg;
      put.allocate = 0;

      if (!qcom_Put(&sts, &tgt, &put)) {
	qcom_Free(&sts, put.data);
	errh_Info("No connection to %s (%s)", m_nodes[i].name,
	    cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
      } else if (debug) {
	printf("Remote DataQ reset sent %s (%s)", m_nodes[i].name,
            cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
      }
    }
  }

  // Examine remote orders
  for (unsigned int i = 0; i < m_rorder.size(); i++) {
    if (m_rorder[i].op->QRStatus) {
      pwr_tOid oid;
      pwr_tOName name;
      pwr_tOName dataname;
      dataq_sMsgROrder* msg;
      unsigned int size;
      char *sourcep;

      // Link to data object
      oid = m_rorder[i].op->Out.Data.Aref.Objid;
      sts = gdh_ObjidToPointer(oid, (void **)&sourcep); 
      if (EVEN(sts))
	continue;

      sts = gdh_ObjidToName(oid, name, sizeof(name), cdh_mName_object);
      if (EVEN(sts))
	continue;
      
      sts = remote_name(name, m_rorder[i].op->RemoteDataName, dataname, sizeof(dataname));
      if (EVEN(sts))
	continue;

      tgt.nid = m_nodes[m_rorder[i].node_idx].nid;
      tgt.qix = dataq_cProcServer;

      put.reply.nid = m_nodes[0].nid;
      put.reply.qix = dataq_cProcServer;
      put.type.b = (qcom_eBtype)dataq_cMsgClass;
      put.type.s = (qcom_eStype)dataq_eMsgType_ROrder;
      put.msg_id = m_msg_id++;
      put.size = sizeof(dataq_sMsgROrder) - sizeof(int) + m_rorder[i].msg_size;
      msg = (dataq_sMsgROrder*)qcom_Alloc(&lsts, put.size);
      
      msg->Type = dataq_eMsgType_ROrder;
      msg->Version = dataq_cNetVersion;
      strncpy(msg->DataName, dataname, sizeof(msg->DataName));
      msg->DataSource = oid;
      msg->ROrder = m_rorder[i].oid;
      msg->Target = m_rorder[i].op->Target;
      msg->DataClass = m_rorder[i].op->RemoteDataClass;
      msg->TableVersion = m_rorder[i].dc.version;
      msg->AttrNum = m_rorder[i].dc.conv.size();

      m_rorder[i].dc.source_data_to_msg((char*)&msg->Data, sourcep, &size);

      put.data = msg;
      put.allocate = 0;

      if (!qcom_Put(&sts, &tgt, &put)) {
	qcom_Free(&sts, put.data);
	errh_Info("No connection to %s (%s)", m_nodes[i].name,
	   cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
      } else
	errh_Info("Remote order sent to %s (%s)", m_nodes[i].name,
          cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));

      m_rorder[i].op->QRStatus = 0;
    }
    else if (m_rorder[i].op->QRStatusReset) {
      dataq_sMsgROrder* msg;

      tgt.nid = m_nodes[m_rorder[i].node_idx].nid;
      tgt.qix = dataq_cProcServer;

      put.reply.nid = m_nodes[0].nid;
      put.reply.qix = dataq_cProcServer;
      put.type.b = (qcom_eBtype)dataq_cMsgClass;
      put.type.s = (qcom_eStype)dataq_eMsgType_ROrderStatusReset;
      put.msg_id = m_msg_id++;
      put.size = sizeof(dataq_sMsgROrder) - sizeof(int) + m_rorder[i].msg_size;
      msg = (dataq_sMsgROrder*)qcom_Alloc(&lsts, put.size);
      
      msg->Type = dataq_eMsgType_ROrderStatusReset;
      msg->Version = dataq_cNetVersion;
      msg->DataSource = pwr_cNOid;
      strcpy(msg->DataName, "");
      msg->ROrder = m_rorder[i].oid;
      msg->Target = m_rorder[i].op->Target;
      msg->DataClass = m_rorder[i].op->RemoteDataClass;
      msg->AttrNum = 0;

      put.data = msg;
      put.allocate = 0;

      if (!qcom_Put(&sts, &tgt, &put)) {
	qcom_Free(&sts, put.data);
	errh_Info("No connection to %s (%s)", m_nodes[i].name,
	   cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
      } else
	errh_Info("Connect sent to %s (%s)", m_nodes[i].name,
          cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));

      m_rorder[i].op->QRStatusReset = 0;
    }
  }

  // Examine target orders
  for (unsigned int i = 0; i < m_torder.size(); i++) {
    if (m_torder[i].op->QTFeedback) {
      pwr_tOid oid;
      pwr_tOName name;
      dataq_sMsgROrderFeedback* msg;
      char *targetp;
      unsigned int msize;

      // Link to data object
      oid = m_torder[i].op->Out.Data.Aref.Objid;
      sts = gdh_ObjidToPointer(oid, (void **)&targetp); 
      if (EVEN(sts))
	continue;

      sts = gdh_ObjidToName(oid, name, sizeof(name), cdh_mName_object);
      if (EVEN(sts))
	continue;
      
      tgt.nid = m_torder[i].reply_nid;
      tgt.qix = dataq_cProcServer;

      put.reply.nid = m_nodes[0].nid;
      put.reply.qix = dataq_cProcServer;
      put.type.b = (qcom_eBtype)dataq_cMsgClass;
      put.type.s = (qcom_eStype)dataq_eMsgType_ROrderFeedback;
      put.msg_id = m_msg_id++;
      put.size = sizeof(dataq_sMsgROrderFeedback) - sizeof(int) + m_torder[i].dc.feedback_data_msg_size();
      msg = (dataq_sMsgROrderFeedback*)qcom_Alloc(&lsts, put.size);
      
      msg->Type = dataq_eMsgType_ROrderFeedback;
      msg->Version = dataq_cNetVersion;
      msg->ROrder = m_torder[i].rorder;
      msg->DataObject = m_torder[i].reply_source;
      msg->TableVersion = m_torder[i].dc.version;
      msg->AttrNum = m_torder[i].dc.feedback_size();

      m_torder[i].dc.feedback_data_to_msg((char*)&msg->Data, targetp, &msize);

      put.data = msg;
      put.allocate = 0;

      if (!qcom_Put(&sts, &tgt, &put)) {
	qcom_Free(&sts, put.data);
	errh_Info("No connection to %s (%s)", m_nodes[i].name,
	   cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
      } else
	errh_Info("Connect sent to %s (%s)", m_nodes[i].name,
          cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));

      if (m_torder[i].options & pwr_mQRemoteOrderOptionsMask_DeleteData) {
	sts = gdh_DeleteObject(oid);
	if (EVEN(sts))
	  return sts;
      }

      m_torder[i].op->QTFeedback = 0;
    }
  }
  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::connect()
{
  dataq_sMsgAny* msg;
  qcom_sQid tgt;
  qcom_sPut put;
  pwr_tStatus sts, lsts;

  // Wait for qmon to start
  sleep(5);

  for (unsigned int i = 1; i < m_nodes.size(); i++) {
    tgt.nid = m_nodes[i].nid;
    tgt.qix = dataq_cProcServer;

    put.reply.nid = m_nodes[0].nid;
    put.reply.qix = dataq_cProcServer;
    put.type.b = (qcom_eBtype)dataq_cMsgClass;
    put.type.s = (qcom_eStype)dataq_eMsgType_NodeUp;
    put.msg_id = m_msg_id++;
    put.size = sizeof(dataq_sMsgAny);
    msg = (dataq_sMsgAny*)qcom_Alloc(&lsts, put.size);

    msg->Type = dataq_eMsgType_NodeUp;
    msg->Version = dataq_cNetVersion;
    put.data = msg;
    put.allocate = 0;

    if (!qcom_Put(&sts, &tgt, &put)) {
      qcom_Free(&sts, put.data);
      errh_Info("No connection to %s (%s)", m_nodes[i].name,
          cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
    } else
      errh_Info("Connect sent to %s (%s)", m_nodes[i].name,
          cdh_NodeIdToString(0, m_nodes[i].nid, 0, 0));
  }

  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::get_config()
{
  pwr_tStatus sts;
  pwr_tOid oid;
  pwr_tAttrRef aref;

  /* Get pointer to the transconfig object */
  sts = gdh_GetClassList(pwr_cClass_DataQServer, &oid);
  if (EVEN(sts))
    return DATAQ__SRVCONFIG;

  /* Direct link to the cell */
  aref = cdh_ObjidToAref(oid);
  sts = gdh_DLRefObjectInfoAttrref(&aref, (pwr_tAddress*)&m_config, 
      &m_config_dlid);
  if (EVEN(sts))
    return sts;
  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::init()
{
  pwr_tStatus sts;
  qcom_sNode node;
  qcom_sQid qid = qcom_cNQid;
  qcom_sQid qini;
  qcom_sQattr qAttr;

  // Init gdh
  sts = gdh_Init("dataq_server");
  if (EVEN(sts)) {
    errh_Fatal("gdh_Init, %m", sts);
    exit(sts);
  }

  // Init qcom and bind event que
  if (!qcom_Init(&sts, 0, "dataq_server")) {
    errh_Fatal("qcom_Init, %m", sts);
    exit(sts);
  }

  qid.qix = dataq_cProcServer;
  qid.nid = 0;
  qAttr.type = qcom_eQtype_private;
  qAttr.quota = 200;
  if (!qcom_CreateQ(&sts, &qid, &qAttr, "DataQServer")) {
    if (sts == QCOM__QALLREXIST) {
      if (!qcom_AttachQ(&sts, &qid)) {
        if (!qcom_DeleteQ(&sts, &qid))
          throw co_error(sts);
        if (!qcom_CreateQ(&sts, &qid, &qAttr, "DataQServer"))
          throw co_error(sts);
      }
    } else
      throw co_error(sts);
  }

  qini = qcom_cQini;
  if (!qcom_Bind(&sts, &qid, &qini)) {
    errh_Fatal("qcom_Bind(Qini), %m", sts);
    exit(-1);
  }

  sts = get_config();
  if (EVEN(sts)) {
    errh_Info("Not configured");
    exit(0);
  }

  // Get all qcom nodes
  qcom_MyNode(&m_sts, &node);
  if (EVEN(m_sts))
    throw co_error(m_sts);

  qsrv_node myn;
  myn.nid = node.nid;
  strncpy(myn.name, node.name, sizeof(myn.name));
  myn.connection = pwr_eUpDownEnum_Up;
  m_nodes.push_back(myn);

  for (pwr_tNid nid = qcom_cNNid; qcom_NextNode(&sts, &node, nid); nid = node.nid) {
    qsrv_node n;

    n.nid = node.nid;
    strncpy(n.name, node.name, sizeof(n.name));
    n.connection = pwr_eUpDownEnum_Down;
    m_nodes.push_back(n);
  }


  // Wait for the plcpgm has flagged initizated
  plc_UtlWaitForPlc();

  sts = rdataq_init();
  if (EVEN(sts)) {
    errh_Error("Init remote dataq error, %m", sts);
    exit(0);
  }

  sts = rorder_init();
  if (EVEN(sts)) {
    errh_Error("Init remote order error, %m", sts);
    exit(0);
  }

  sts = torder_init();
  if (EVEN(sts)) {
    errh_Error("Init target order error, %m", sts);
    exit(0);
  }

  // Init remote dataq and orders on local node
  send_rdataq_init(m_nodes[0].nid);
  send_rorder_init(m_nodes[0].nid);

  return DATAQ__SUCCESS;
}

pwr_tStatus dataq_server::mainloop()
{
  pwr_tStatus sts;
  int tmo;
  char mp[20000];
  qcom_sGet get;
  int swap = 0;
  qcom_sQid qid;

  qid.qix = dataq_cProcServer;
  qid.nid = 0;

  tmo = 1000 * m_config->ScanTime - 1;

  for (;;) {
    get.maxSize = sizeof(mp);
    get.data = mp;
    qcom_Get(&sts, &qid, &get, tmo);
    if (sts == QCOM__TMO || sts == QCOM__QEMPTY) {
      if (!swap) {
        sts = trans_handler();
        if (EVEN(sts)) {
	  errh_Error("Trans handler error, %m", sts);
	  exit(0);
	}
      }
      continue;
    } 
    switch ((int)get.type.b) {
    case dataq_cMsgClass:
      if (debug)
	printf("Message recevied %d\n", (int)get.type.s);
      switch ((int)get.type.s) {
      case dataq_eMsgType_NodeUp:
        errh_Info("Node up %s", cdh_NodeIdToString(0, get.reply.nid, 0, 0));
        node_up(get.reply.nid);
        break;
      case dataq_eMsgType_NodeUpR:
        errh_Info("Node up %s", cdh_NodeIdToString(0, get.reply.nid, 0, 0));
        node_up_reply(get.reply.nid);
        break;
      case dataq_eMsgType_RDataQInit:
        rdataq_init_msg((dataq_sMsgRDataQInit*)mp, get.size, get.reply.nid);
        break;
      case dataq_eMsgType_RDataQFeedback:
        rdataq_feedback((dataq_sMsgRDataQFeedback*)mp, get.size, get.reply.nid);
        break;
      case dataq_eMsgType_RDataQReset:
        rdataq_reset((dataq_sMsgRDataQReset*)mp, get.size, get.reply.nid);
        break;
      case dataq_eMsgType_RDataQ:
        rdataq((dataq_sMsgRDataQ*)mp, get.size, get.reply.nid);
        break;
      case dataq_eMsgType_ROrderInit:
        rorder_init_msg((dataq_sMsgROrderInit*)mp, get.size, get.reply.nid);
        break;
      case dataq_eMsgType_ROrder:
        rorder((dataq_sMsgROrder*)mp, get.size, get.reply.nid);
        break;
      case dataq_eMsgType_ROrderStatusReset:
        rorder((dataq_sMsgROrder*)mp, get.size, get.reply.nid);
        break;
      case dataq_eMsgType_ROrderFeedback:
        rorder_feedback((dataq_sMsgROrderFeedback*)mp, get.size, get.reply.nid);
        break;
      }
      break;
    case qcom_eBtype_event: {
      ini_mEvent new_event;
      qcom_sEvent* ep = (qcom_sEvent*)get.data;

      new_event.m = ep->mask;
      if (new_event.b.oldPlcStop && !swap) {
        swap = 1;
	rdataq_close();
	rorder_close();
	torder_close();
      } else if (new_event.b.swapDone && swap) {
        swap = 0;
	
	sts = rdataq_init();
	if (EVEN(sts)) {
	  errh_Error("Init remote dataq error, %m", sts);
	  exit(0);
	}

        sts = rorder_init();
        if (EVEN(sts)) {
	  errh_Error("Init remote order error, %m", sts);
	  exit(0);
	}
	
	sts = torder_init();
	if (EVEN(sts)) {
	  errh_Error("Init target order error, %m", sts);
	  exit(0);
	}
        errh_Info("Warm restart completed");
      } else if (new_event.b.terminate) {
        exit(0);
      }
      break;
    }
    }
    qcom_Free(&sts, mp);
  }
}

dataq_server::~dataq_server()
{
  rdataq_close();
  rorder_close();
  torder_close();
}

int main(int argc, char* argv[])
{
  dataq_server srv;

  if (argc > 1 && streq(argv[1], "-d"))
    debug = 1;

  srv.init();
  srv.connect();
  srv.mainloop();
}
