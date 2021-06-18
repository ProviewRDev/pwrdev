#include <pthread.h>
#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "pwr.h"
#include "co_string.h"
#include "co_time.h"
#include "co_cdh.h"
#include "rt_gdh.h"
#include "rt_aproc.h"
#include "rt_sevcli.h"
#include "rt_ini_event.h"
#include "rs_remote_msg.h"
#include "rt_qcom_msg.h"
#include "rt_pwr_msg.h"
#include "pwr_baseclasses.h"



static int debug = 0;

typedef enum {
  mqtt_eCon_NotConnected = 0,
  mqtt_eCon_Connected = 1,
  mqtt_eCon_WaitConnect = 2
} mqtt_eCon;

typedef enum {
  mEventOpt_Time = 1,
  mEventOpt_Type = 2,
  mEventOpt_Prio = 4,
  mEventOpt_Text = 8,
  mEventOpt_Name = 16,
  mEventOpt_Id = 32
} mEventOpt;

class Sub {
public:
  pwr_tStatus sts;
  pwr_tAName attribute;
  char reply[80];
  float cycle;
  float duration;
  void *valp;
  pwr_tRefId refid;
  unsigned int size;
  pwr_tTypeId tid;
  unsigned int cnt;
  int subref;
  
  Sub() : sts(0), cycle(0), duration(0), valp(0), refid(pwr_cNRefId), size(0), tid(0), cnt(0), subref(0) {
    strcpy(attribute, "");
    strcpy(reply, "");
  }
  Sub(const Sub& x) : sts(x.sts), cycle(x.cycle), duration(x.duration), valp(x.valp),
      refid(x.refid), size(x.size), tid(x.tid), cnt(x.cnt), subref(x.subref) {
    strcpy(attribute, x.attribute);
    strcpy(reply, x.reply);
  }
  void unref() {
    if (debug)
      printf("Unref (%d,%d)\n", refid.nid, refid.rix);
    if (refid.nid != 0)
      gdh_UnrefObjectInfo(refid);
  }
  void ref() {
    pwr_tStatus sts;
    sts = gdh_RefObjectInfo(attribute, &valp, &refid, size);
    if (debug)
      printf("Ref (%d,%d) %s\n", refid.nid, refid.rix, attribute);
  }
};

class SublistSub {
public:
  pwr_tStatus sts;
  pwr_tAName attribute;
  int idx;
  void *valp;
  pwr_tRefId refid;
  unsigned int size;
  pwr_tTypeId tid;
  
  SublistSub() : sts(0), idx(0), valp(0), refid(pwr_cNRefId), size(0), tid(0) {
    strcpy(attribute, "");
  }    
  SublistSub(const SublistSub& x) : sts(x.sts), idx(x.idx), valp(x.valp),
      refid(x.refid), size(x.size), tid(x.tid) {
    strcpy(attribute, x.attribute);
  }
  void unref() {
    if (debug)
      printf("Unref (%d,%d)\n", refid.nid, refid.rix);
    if (refid.nid != 0)
      gdh_UnrefObjectInfo(refid);
  }
  void ref() {
    pwr_tStatus sts;
    sts = gdh_RefObjectInfo(attribute, &valp, &refid, size);
    if (debug)
      printf("Ref (%d,%d) %s\n", refid.nid, refid.rix, attribute);
  }
};

class Sublist {
public:
  pwr_tStatus sts;
  std::vector<SublistSub> slist;
  char reply[80];
  float cycle;
  float duration;
  unsigned int cnt;
  int subref;
  
  Sublist() : sts(0), cycle(0), duration(0), cnt(0), subref(0) {
    strcpy(reply, "");
  }
  Sublist(const Sublist& x) : sts(x.sts), slist(x.slist), cycle(x.cycle), duration(x.duration),
      cnt(x.cnt), subref(x.subref) {
    strcpy(reply, x.reply);
  }
  void unref() {
    for (int i = 0; i < slist.size(); i++)
      slist[i].unref();
  }
  void ref() {
    for (int i = 0; i < slist.size(); i++)
      slist[i].ref();
  }
};

class MqttServer {
public:
  pwr_sClass_MqttServer *conf;
  struct mosquitto *mosq;
  mqtt_eCon connected;
  pthread_t loop_thread;
  sevcli_tCtx sevclictx;
  int next_subref;
  std::vector<Sub> subs;
  std::vector<Sublist> sublists;

  MqttServer() : conf(0), mosq(0), connected(mqtt_eCon_NotConnected),
		 loop_thread(0), sevclictx(0), next_subref(1) {}
  void init(qcom_sQid* qid);
  int open(int restart);
  void close();
  int mqtt_connect();
  void *loop();

  static void* mqtt_loop(void* arg);
};

static MqttServer *srv;


static pwr_tStatus mqtt_error_to_sts(int err)
{
  pwr_tStatus sts = 0;

  switch (err) {
  case MOSQ_ERR_CONN_PENDING:
    sts = REM__TT_CONN_PENDING;
    break;
  case MOSQ_ERR_SUCCESS:
    sts = REM__SUCCESS;
    break;
  case MOSQ_ERR_NOMEM:
    sts = REM__TT_NOMEM;
    break;
  case MOSQ_ERR_PROTOCOL:
    sts = REM__TT_PROTOCOL;
    break;
  case MOSQ_ERR_INVAL:
    sts = REM__TT_INVAL;
    break;
  case MOSQ_ERR_NO_CONN:
    sts = REM__TT_NO_CONN;
    break;
  case MOSQ_ERR_CONN_REFUSED:
    sts = REM__TT_CONN_REFUSED;
    break;
  case MOSQ_ERR_NOT_FOUND:
    sts = REM__TT_NOT_FOUND;
    break;
  case MOSQ_ERR_CONN_LOST:
    sts = REM__TT_CONN_LOST;
    break;
  case MOSQ_ERR_TLS:
    sts = REM__TT_TLS;
    break;
  case MOSQ_ERR_PAYLOAD_SIZE:
    sts = REM__TT_PAYLOAD_SIZE;
    break;
  case MOSQ_ERR_NOT_SUPPORTED:
    sts = REM__TT_NOT_SUPPORTED;
    break;
  case MOSQ_ERR_AUTH:
    sts = REM__TT_AUTH;
    break;
  case MOSQ_ERR_ACL_DENIED:
    sts = REM__TT_ACL_DENIED;
    break;
  case MOSQ_ERR_UNKNOWN:
    sts = REM__TT_UNKNOWN;
    break;
  case MOSQ_ERR_ERRNO:
    sts = REM__TT_ERRNO;
    break;
  case MOSQ_ERR_EAI:
    sts = REM__TT_EAI;
    break;
  case MOSQ_ERR_PROXY:
    sts = REM__TT_PROXY;
    break;
  case MOSQ_ERR_PLUGIN_DEFER:
    sts = REM__TT_PLUGIN_DEFER;
    break;
  case MOSQ_ERR_MALFORMED_UTF8:
    sts = REM__TT_MALFORMED_UTF8;
    break;
  case MOSQ_ERR_KEEPALIVE:
    sts = REM__TT_KEEPALIVE;
    break;
  case MOSQ_ERR_LOOKUP:
    sts = REM__TT_LOOKUP;
    break;
  }
  return sts;
}

static int json_find(char *msg, const char *name, char *value, int is_string)
{
  char *s;
  char mvalue[500];
  int colon_found = 0;
  int value_found = 0;
  int parcnt = 0;

  s = strstr(msg, name);
  if (!s)
    return 0;
  s += strlen(name);
  while (*s) {
    if (*s == ' ' || *s == '	') {
      s++;
      continue;
    }
    if (*s == ':') {
      if (colon_found)
	return 0;
      colon_found = 1;
      s++;
      continue;
    }
    if (colon_found && !value_found) {
      strncpy(mvalue, s, sizeof(mvalue));
      value_found = 1;
      break;
    }
    if (*s == ',')
      break;
    s++;
  }
  if (!colon_found || !value_found)
    return 0;
  s = mvalue;
  while (*s) {
    if (*s == '{' || *s == '[')
      parcnt++;
    else if (*s == '}' || *s == ']') {
      if (parcnt == 0) {
	*s = 0;
	break;
      } else {
	parcnt--;
	if (parcnt == 0) {
	  *(s + 1) = 0;
	  break;
	}
      }
    }
    else if (*s == 0 || (*s == ',' && parcnt == 0)) {
      *s = 0;
      break;
    }
    s++;
  }
  str_trim(mvalue, mvalue);
  if (is_string && mvalue[0] == '\"' && mvalue[strlen(mvalue)-1] == '\"') {
    strcpy(value, &mvalue[1]);
    value[strlen(value)-1] = 0;
  }
  else
    strcpy(value, mvalue);
  return 1;
}

static void message_cb(struct mosquitto *mosq, void *obj, 
    const struct mosquitto_message *msg)
{
  if (msg->payloadlen > 0) {
    int sts;
    char action[100];
    char reply[200];

    if (debug)
      printf("Msg:\"%s\"\n", (char *)msg->payload);

    sts = json_find((char *)msg->payload, "\"action\"", action, 1);
    
    if (streq(action, "get")) {
      char attribute[500];

      sts = json_find((char *)msg->payload, "\"attribute\"", attribute, 1);

      sts = json_find((char *)msg->payload, "\"reply\"", reply, 1);

      pwr_tFloat32 val;
      char rmsg[100];
      int rc;

      sts = gdh_GetObjectInfo(attribute, &val, sizeof(val));
      if (EVEN(sts)) {
	sprintf(rmsg, "{\"status\":%u}", sts);
      } else {
	sprintf(rmsg, "{\"status\":%u,\"value\":%g}", sts, val);
	rc = mosquitto_publish(srv->mosq, NULL, reply, strlen(rmsg), 
	rmsg, 1, 0);
	if (debug)
	  printf("get: %d %s\n", rc, rmsg);
      }
	
    }
    else if (streq(action, "set")) {
      char attribute[500];
      char valstr[500];

      sts = json_find((char *)msg->payload, "\"attribute\"", attribute, 1);

      sts = json_find((char *)msg->payload, "\"reply\"", reply, 1);

      sts = json_find((char *)msg->payload, "\"value\"", valstr, 1);

      char rmsg[100];
      int rc;
      int n;
      pwr_tTid tid;

      sts = gdh_GetAttributeCharacteristics(attribute, &tid, 0, 0, 0);
      switch (tid) {
      case pwr_eType_Float32: {
	pwr_tFloat32 val;
	n = sscanf(valstr, "%g", &val);
	if (n != 1)
	  sts = 0;
	else
	  sts = gdh_SetObjectInfo(attribute, &val, sizeof(val));
	break;
      }
      case pwr_eType_Boolean: {
	pwr_tBoolean val;
	n = sscanf(valstr, "%d", &val);
	if (n != 1)
	  sts = 0;
	else
	  sts = gdh_SetObjectInfo(attribute, &val, sizeof(val));
	break;
      }
      }
      sprintf(rmsg, "{\"status\":%u}", sts);
      rc = mosquitto_publish(srv->mosq, NULL, reply, strlen(rmsg), 
	  rmsg, 1, 0);
      if (debug)
	printf("set: %d %s %s\n", rc, reply, rmsg);	
    }
    else if (streq(action, "subscribe")) {
      char attribute[500];
      char cyclestr[20];
      char durationstr[20];

      sts = json_find((char *)msg->payload, "\"attribute\"", attribute, 1);

      if (ODD(sts))
	sts = json_find((char *)msg->payload, "\"reply\"", reply, 1);

      if (ODD(sts))
	sts = json_find((char *)msg->payload, "\"cycle\"", cyclestr, 1);

      if (ODD(sts))
	sts = json_find((char *)msg->payload, "\"duration\"", durationstr, 1);

      pwr_tFloat32 cycle;
      pwr_tFloat32 duration;
      char rmsg[100];
      int rc;
      int n;
      pwr_tUInt32 size;
      pwr_tRefId refid;
      void *refp;
      pwr_tTid tid;

      if (ODD(sts)) {
	n = sscanf(cyclestr, "%g", &cycle);
	if (n != 1)
	  sts = 0;
      }
      if (ODD(sts)) {
	n = sscanf(durationstr, "%g", &duration);
	if (n != 1)
	  sts = 0;
      }
      if (ODD(sts))
	sts = gdh_GetAttributeCharacteristics(attribute, &tid, &size, 0, 0);
      if (ODD(sts))
	sts = gdh_RefObjectInfo(attribute, &refp, &refid, size);
      if (EVEN(sts)) {
	sprintf(rmsg, "{\"status\":%u}", sts);
	rc = mosquitto_publish(srv->mosq, NULL, reply, strlen(rmsg), 
	    rmsg, 1, 0);
	if (debug)
	  printf("subscribe: %d %s\n", rc, rmsg);	
      }
      else {
	Sub sub;
	strcpy(sub.attribute, attribute);
	strcpy(sub.reply, reply);
	sub.cycle = cycle;
	sub.duration = duration;
	sub.valp = refp;
	sub.refid = refid;
	sub.size = size;
	sub.tid = tid;
	sub.subref = srv->next_subref++;
	srv->subs.push_back(sub);
      }
    }
    else if (streq(action, "sublist")) {
      char attribute[500];
      char cyclestr[20];
      char durationstr[20];
      Sublist sl;

      sts = json_find((char *)msg->payload, "\"attribute\"", attribute, 1);

      if (ODD(sts))
	sts = json_find((char *)msg->payload, "\"reply\"", reply, 1);

      if (ODD(sts))
	sts = json_find((char *)msg->payload, "\"cycle\"", cyclestr, 1);

      if (ODD(sts))
	sts = json_find((char *)msg->payload, "\"duration\"", durationstr, 1);

      pwr_tFloat32 cycle;
      pwr_tFloat32 duration;
      char rmsg[100];
      int rc;
      int n;
      pwr_tUInt32 size;
      pwr_tRefId refid;
      void *refp;
      pwr_tTid tid;

      if (ODD(sts)) {
	n = sscanf(cyclestr, "%g", &cycle);
	if (n != 1)
	  sts = 0;
      }
      if (ODD(sts)) {
	n = sscanf(durationstr, "%g", &duration);
	if (n != 1)
	  sts = 0;
      }
      if (ODD(sts)) {
	int num;
	int idx;
	pwr_tAName attr;
	int parcnt = 0;
	int sqrparcnt = 0;
	char *s1;
	char *s = attribute;
	while (*s) {
	  if (*s == '[') {
	    sqrparcnt++;
	  }
	  if (*s == ']') {
	    sqrparcnt--;
	    if (sqrparcnt == 0)
	      break;
	  }
	  if (*s == '{') {
	    parcnt++;
	    if (parcnt == 1) {
	      // New sub
	      num = sscanf(s+1, "%d", &idx);
	      if (num != 1) {
		sts = 0;
		break;
	      }
	    }
	  }
	  else if (*s == '}') {
	    parcnt--;
	    if (parcnt == 0) {
	      // Sub end
	      SublistSub sub;

	      sts = gdh_GetAttributeCharacteristics(attr, &tid, &size, 0, 0);
	      if (ODD(sts))
		sts = gdh_RefObjectInfo(attr, &refp, &refid, size);
	      if (ODD(sts)) {
		strcpy(sub.attribute, attr);
		sub.idx = idx;
		sub.valp = refp;
		sub.refid = refid;
		sub.size = size;
		sub.tid = tid;
		sl.slist.push_back(sub);
	      }
	    }
	  }
	  else if (*s == ',') {
	    if (parcnt == 1) {
	      s++;
	      if (*s == '\"') {
		s++;
		s1 = s;
		while (*s && *s != '\"')
		  s++;
		strncpy(attr, s1, s - s1);
		attr[s - s1] = 0;
	      }
	    }
	  }
	  s++;
	}
      }
      if (EVEN(sts)) {
	sprintf(rmsg, "{\"status\":%u}", sts);
	rc = mosquitto_publish(srv->mosq, NULL, reply, strlen(rmsg), 
	    rmsg, 1, 0);
	if (debug)
	  printf("sublist %d %s\n", rc, rmsg);	
      }
      strcpy(sl.reply, reply);
      sl.cycle = cycle;
      sl.duration = duration;
      sl.subref = srv->next_subref++;
      srv->sublists.push_back(sl);
    }
    else if (streq(action, "closesub")) {
      char subrefstr[40];
      int subref;
      int n;

      sts = json_find((char *)msg->payload, "\"subref\"", subrefstr, 1);
      if (ODD(sts)) {
	n = sscanf(subrefstr, "%d", &subref);
	if (n != 1)
	  sts = 0;
      }
      if (ODD(sts)) {
	for (int i = 0; i < srv->subs.size(); i++) {
	  if (subref == srv->subs[i].subref) {
	    if (debug)
	      printf("Unref subref %d\n", srv->subs[i].subref);
	    srv->subs[i].unref();
	    srv->subs.erase(srv->subs.begin() + i);
	  }
	}
      }
      else {
	if (debug)
	  printf("closesub: sts %d\n", sts);
      }
    }
    else if (streq(action, "closesublist")) {
      char subrefstr[40];
      int subref;
      int n;

      sts = json_find((char *)msg->payload, "\"subref\"", subrefstr, 1);
      if (ODD(sts)) {
	n = sscanf(subrefstr, "%d", &subref);
	if (n != 1)
	  sts = 0;
      }
      if (ODD(sts)) {
	for (int i = 0; i < srv->sublists.size(); i++) {
	  if (subref == srv->sublists[i].subref) {
	    if (debug)
	      printf("closesublist subref %d\n", srv->sublists[i].subref);
	    srv->sublists[i].unref();
	    srv->sublists.erase(srv->sublists.begin() + i);
	  }
	}
      }
      else {
	if (debug)
	  printf("closesub: sts %d\n", sts);
      }
    }
    else if (streq(action, "history")) {
      pwr_tOName object;
      pwr_tOName attribute;
      char server[80];
      char fromstr[30];
      char tostr[30];
      char maxrowsstr[20];
      int maxrows;
      pwr_tTime to, from;
      pwr_tDeltaTime fromdelta;
      char *rmsg;
      int msize;
      int rc;
      int n;
      pwr_tTime *tbuf;
      void *vbuf;
      int rows;
      pwr_eType vtype;
      unsigned int vsize;
      char timstr[30];
      pwr_tOid oid;
      int nyi = 0;

      while (1) {
	sts = json_find((char *)msg->payload, "\"server\"", server, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"object\"", object, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"attribute\"", attribute, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"reply\"", reply, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"from\"", fromstr, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"to\"", tostr, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"maxrows\"", maxrowsstr, 1);
	if (EVEN(sts)) {
	  maxrows = 1000;
	  sts = 1;
	}
	else {
	  n = sscanf(maxrowsstr, "%d", &maxrows);
	  if (n != 1) {
	    sts = 0;
	    break;
	  }
	}
	
	if ( strncmp("_O", object, 2) == 0) 
	  sts = cdh_StringToObjid(object, &oid);
	else
	  sts = gdh_NameToObjid(object, &oid);
	if (EVEN(sts))
	  break;
      

	if ( strcmp(tostr, "now") == 0) {
	  /* fromstr is a deltatime */
	  sts = time_AsciiToD(fromstr, &fromdelta);
	  if ( EVEN(sts))
	    break;

	  time_GetTime(&to);
	  time_Asub(&from, &to, &fromdelta);
	}
	else {
	  sts = time_AsciiToA(fromstr, &from);
	  if ( EVEN(sts))
	    break;

	  sts = time_AsciiToA(tostr, &to);
	  if ( EVEN(sts))
	    break;
	}

	if (!srv->sevclictx) {
	  sevcli_init( &sts, &srv->sevclictx);
	  if ( EVEN(sts))
	    break;
	}

	sevcli_set_servernode( &sts, srv->sevclictx, server);
	if ( EVEN(sts))
	  break;

	sevcli_get_itemdata( &sts, srv->sevclictx, oid, attribute, from, to, maxrows, &tbuf, &vbuf,
  		       &rows, &vtype, &vsize);
	if (EVEN(sts))
	  break;

	switch (vtype) {
	case pwr_eType_Float32:
	  msize = rows * (13 + 23 + 4) + 32;
	  break;
	case pwr_eType_Boolean:
	  msize = rows * (1 + 23 + 4) + 32;
	  break;
	case pwr_eType_Int32:
	case pwr_eType_UInt32:
	  msize = rows * (13 + 23 + 4) + 32;
	  break;
	default:
	  nyi = 1;
	}
	msize += 500;
	if (nyi) {
	  sts = PWR__NYI;
	  break;
	}

	rmsg = (char *)calloc(1, msize);

	n = 0;
	n = sprintf(rmsg, "{\"status\":%u,\"values\":[", sts);
	for (int i = 0; i < rows; i++) {
	  switch (vtype) {
	  case pwr_eType_Float32:
	    n += sprintf(&rmsg[n], "%e", ((pwr_tFloat32 *)vbuf)[i]);
	    break;
	  case pwr_eType_Int32:
	    n += sprintf(&rmsg[n], "%d", ((pwr_tInt32 *)vbuf)[i]);
	    break;
	  case pwr_eType_Boolean:
	  case pwr_eType_UInt32:
	    n += sprintf(&rmsg[n], "%u", ((pwr_tUInt32 *)vbuf)[i]);
	    break;
	  default:;
	  }
	  if (i == rows - 1) {
	    strcat(rmsg, "],\"time\":[");
	    n += 10;
	  }
	  else {
	    strcat(rmsg, ",");
	    n += 1;
	  }
	}

	for (int i = 0; i < rows; i++) {
	  time_AtoAscii( &tbuf[i], time_eFormat_DateAndTime, timstr, sizeof(timstr));
	  strcat(rmsg, "\"");
	  strcat(rmsg, timstr);
	  strcat(rmsg, "\"");
	  n += strlen(timstr) + 2;
	  if (i == rows - 1) {
	    strcat(rmsg, "]}");
	    n += 2;
	  }
	  else {
	    strcat(rmsg, ",");
	    n += 1;
	  }
	}
	if (debug)
	  printf("size %d %d %d\n", msize, n, (int)strlen(rmsg));

	rc = mosquitto_publish(srv->mosq, NULL, reply, strlen(rmsg), 
	    rmsg, 1, 0);

	if (debug) {
	  char txt[100];
	  strncat(txt, rmsg, 100);
	  txt[99] = 0;
	  printf("history %d %s...\n", rc, txt);
	}
	free(rmsg);

	break;
      }
      if (EVEN(sts)) {
	rmsg = (char *)calloc(1, 40);
	sprintf(rmsg, "{\"status\":%u}", sts);
	rc = mosquitto_publish(srv->mosq, NULL, reply, strlen(rmsg), 
	    rmsg, 1, 0);
	if (debug)
	  printf("history %d %s\n", rc, rmsg);	
	free(rmsg);
      }
    }
    else if (streq(action, "eventhist")) {
      pwr_tOName object;
      char server[80];
      char optionsstr[20];
      unsigned int options;
      char fromstr[30];
      char tostr[30];
      char eventtypestr[20];
      pwr_tUInt32 eventtypemask;
      char eventpriostr[20];
      pwr_tUInt32 eventpriomask;
      char eventtext[80];
      pwr_tOName eventname;
      char maxrowsstr[20];
      int maxrows;
      pwr_tTime to, from;
      pwr_tDeltaTime fromdelta;
      sevcli_sEvents *list;
      unsigned int listcnt;
      char *rmsg;
      int msize;
      int rc;
      int n;
      char timstr[30];
      pwr_tOid oid;

      while (1) {
	sts = json_find((char *)msg->payload, "\"server\"", server, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"object\"", object, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"options\"", optionsstr, 1);
	if (EVEN(sts)) {
	  options = mEventOpt_Time | mEventOpt_Type | mEventOpt_Text;
	  sts = 1;
	}
	else {
	  n = sscanf(optionsstr, "%u", &options);
	  if (n != 1) {
	    sts = 0;
	    break;
	  }
	}
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"eventtype\"", eventtypestr, 1);
	if (EVEN(sts)) {
	  eventtypemask = 0;
	  sts = 1;
	}
	else {
	  n = sscanf(eventtypestr, "%u", &eventtypemask);
	  if (n != 1) {
	    sts = 0;
	    break;
	  }
	}
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"eventprio\"", eventpriostr, 1);
	if (EVEN(sts)) {
	  eventpriomask = 0;
	  sts = 1;
	}
	else {
	  n = sscanf(eventpriostr, "%u", &eventpriomask);
	  if (n != 1) {
	    sts = 0;
	    break;
	  }
	}
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"eventtext\"", eventtext, 1);
	if (EVEN(sts))
	  strcpy(eventtext, "");

	sts = json_find((char *)msg->payload, "\"eventname\"", eventname, 1);
	if (EVEN(sts))
	  strcpy(eventname, "");

	sts = json_find((char *)msg->payload, "\"reply\"", reply, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"from\"", fromstr, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"to\"", tostr, 1);
	if (EVEN(sts))
	  break;

	sts = json_find((char *)msg->payload, "\"maxrows\"", maxrowsstr, 1);
	if (EVEN(sts)) {
	  maxrows = 1000;
	  sts = 1;
	}
	else {
	  n = sscanf(maxrowsstr, "%d", &maxrows);
	  if (n != 1) {
	    sts = 0;
	    break;
	  }
	}
	
	if ( strncmp("_O", object, 2) == 0) 
	  sts = cdh_StringToObjid(object, &oid);
	else
	  sts = gdh_NameToObjid(object, &oid);
	if (EVEN(sts))
	  break;
      

	if ( strcmp(tostr, "now") == 0) {
	  /* fromstr is a deltatime */
	  sts = time_AsciiToD(fromstr, &fromdelta);
	  if ( EVEN(sts))
	    break;

	  time_GetTime(&to);
	  time_Asub(&from, &to, &fromdelta);
	}
	else {
	  sts = time_AsciiToA(fromstr, &from);
	  if ( EVEN(sts))
	    break;

	  sts = time_AsciiToA(tostr, &to);
	  if ( EVEN(sts))
	    break;
	}

	if (!srv->sevclictx) {
	  sevcli_init( &sts, &srv->sevclictx);
	  if ( EVEN(sts))
	    break;
	}

	sevcli_set_servernode( &sts, srv->sevclictx, server);
	if ( EVEN(sts))
	  break;

	sevcli_get_events( &sts, srv->sevclictx, oid, from, to, eventtypemask, eventpriomask,
			   eventtext, eventname, maxrows, &list, &listcnt);
	if (EVEN(sts))
	  break;

	msize = 100;
	msize += 20 + 10 + 10 + 10 + 10 + 10 + 1;
	for (int i = 0; i < listcnt; i++) {
	  if (options & mEventOpt_Time)
	    msize += 23 + 3;
	  if (options & mEventOpt_Type)
	    msize += 11;
	  if (options & mEventOpt_Prio)
	    msize += 3;
	  if (options & mEventOpt_Text)
	    msize += strlen(list[i].EventText) + 3;
	  if (options & mEventOpt_Name)
	    msize += strlen(list[i].EventName) + 3;  
	  if (options & mEventOpt_Id)
	    msize += 11;
	}
	rmsg = (char *)calloc(1, msize);

	n = 0;
	n += sprintf(rmsg, "{\"status\":%u", sts);
	if (options & mEventOpt_Time) {
	  n += sprintf(&rmsg[strlen(rmsg)], ",\"time\":[");
	  for (int i = 0; i < listcnt; i++) {
	    time_AtoAscii( &list[i].Time, time_eFormat_DateAndTime, timstr, sizeof(timstr));
	    strcat(rmsg, "\"");
	    strcat(rmsg, timstr);
	    strcat(rmsg, "\"");
	    n += strlen(timstr) + 2;
	    if (i == listcnt - 1)
	      strcat(rmsg, "]");
	    else
	      strcat(rmsg, ",");
	    n += 1;
	  }
	}
	if (options & mEventOpt_Type) {
	  strcat(rmsg, ",\"type\":[");
	  n += 9;
	  for (int i = 0; i < listcnt; i++) {
	    n += sprintf(&rmsg[strlen(rmsg)], "%u", list[i].EventType);
	    if (i == listcnt - 1)
	      strcat(rmsg, "]");
	    else
	      strcat(rmsg, ",");
	    n += 1;
	  }
	}
	if (options & mEventOpt_Prio) {
	  strcat(rmsg, ",\"prio\":[");
	  n += 8;
	  for (int i = 0; i < listcnt; i++) {
	    n += sprintf(&rmsg[strlen(rmsg)], "%u", list[i].EventPrio);
	    if (i == listcnt - 1)
	      strcat(rmsg, "]");
	    else
	      strcat(rmsg, ",");
	    n += 1;
	  }
	}
	if (options & mEventOpt_Prio) {
	  strcat(rmsg, ",\"prio\":[");
	  n += 8;
	  for (int i = 0; i < listcnt; i++) {
	    n += sprintf(&rmsg[strlen(rmsg)], "%u", list[i].EventPrio);
	    if (i == listcnt - 1)
	      strcat(rmsg, "]");
	    else
	      strcat(rmsg, ",");
	    n += 1;
	  }
	}
	if (options & mEventOpt_Text) {
	  strcat(rmsg, ",\"text\":[");
	  n += 9;
	  for (int i = 0; i < listcnt; i++) {
	    strcat(rmsg, "\"");
	    strcat(rmsg, list[i].EventText);
	    strcat(rmsg, "\"");
	    n += strlen(list[i].EventText) + 2;
	    if (i == listcnt - 1)
	      strcat(rmsg, "]");
	    else
	      strcat(rmsg, ",");
	    n += 1;
	  }
	}
	if (options & mEventOpt_Name) {
	  strcat(rmsg, ",\"name\":[");
	  n += 9;
	  for (int i = 0; i < listcnt; i++) {
	    strcat(rmsg, "\"");
	    strcat(rmsg, list[i].EventName);
	    strcat(rmsg, "\"");
	    n += strlen(list[i].EventName) + 2;
	    if (i == listcnt - 1)
	      strcat(rmsg, "]");
	    else
	      strcat(rmsg, ",");
	    n += 1;
	  }
	}
	if (options & mEventOpt_Id) {
	  strcat(rmsg, ",\"id_nix\":[");
	  n += 8;
	  for (int i = 0; i < listcnt; i++) {
	    n += sprintf(&rmsg[strlen(rmsg)], "%u", list[i].EventId.Nix);
	    if (i == listcnt - 1)
	      strcat(rmsg, "]");
	    else
	      strcat(rmsg, ",");
	    n += 1;
	  }
	  strcat(rmsg, ",\"id_idx\":[");
	  n += 8;
	  for (int i = 0; i < listcnt; i++) {
	    n += sprintf(&rmsg[strlen(rmsg)], "%u", list[i].EventId.Idx);
	    if (i == listcnt - 1)
	      strcat(rmsg, "]");
	    else
	      strcat(rmsg, ",");
	    n += 1;
	  }
	}
	strcat(rmsg, "}");
	n += 1;

	if (debug)
	  printf("size %d %d %d\n", msize, n, (int)strlen(rmsg));

	rc = mosquitto_publish(srv->mosq, NULL, reply, strlen(rmsg), 
	    rmsg, 1, 0);

	if (debug) {
	  char txt[100];
	  strncat(txt, rmsg, 100);
	  txt[99] = 0;
	  printf("eventhist %d %s...\n", rc, txt);
	}
	free(rmsg);

	break;
      }
      if (EVEN(sts)) {
	rmsg = (char *)calloc(1, 40);
	sprintf(rmsg, "{\"status\":%u}", sts);
	rc = mosquitto_publish(srv->mosq, NULL, reply, strlen(rmsg), 
	    rmsg, 1, 0);
	if (debug)
	  printf("eventhist %d %s\n", rc, rmsg);	
	free(rmsg);
      }
    }
  }
}

static void connect_cb(struct mosquitto *mosq, void *obj, int result)
{
  if (debug)
    printf("connect_cb result %d\n", result);
  if(result){
    srv->conf->Status = mqtt_error_to_sts(result);
    if (debug)
      printf("%s\n", mosquitto_connack_string(result));
    srv->connected = mqtt_eCon_NotConnected;
    if (result == MOSQ_ERR_CONN_REFUSED) {
      printf("Remote mqtt terminated, %s", mosquitto_connack_string(result));
      exit(0);
    }
  }
  else {
    mosquitto_subscribe(srv->mosq, NULL, srv->conf->SubscribeTopic, 0);
    srv->conf->Status = 1;
    srv->connected = mqtt_eCon_Connected;
  }
}

int MqttServer::mqtt_connect()
{
  int rc;
  char id[20];

  if (!mosq) {
    sprintf(id, "%u", 12345);
    mosq = mosquitto_new(id, true, this);
  
    mosquitto_connect_callback_set(mosq, connect_cb);
    mosquitto_message_callback_set(mosq, message_cb);

    if (strcmp(conf->User, "") != 0)
      mosquitto_username_pw_set(mosq, conf->User, conf->Password);
    // mosquitto_tls_set(mosq, "ca-cert.pem", NULL, NULL, NULL, NULL);

    rc = mosquitto_connect(mosq, conf->Server, conf->Port, 60);
    if (rc) {
      conf->Status = mqtt_error_to_sts(rc);
      return conf->Status;
    }
    connected = mqtt_eCon_WaitConnect;
  }
  else {
    rc = mosquitto_reconnect(mosq);
    if (rc) {
      conf->Status = mqtt_error_to_sts(rc);
      return conf->Status;
    }
  }
  return 1;
}

void* MqttServer::mqtt_loop(void* arg)
{
  return srv->loop();
}

void *MqttServer::loop()
{
  int rc;
  pwr_tTime now, last;
  pwr_tFloat32 dtf;
  pwr_tFloat32 scan_time = 1.0;

  time_GetTimeMonotonic(&last);
  while(1) {
    while (1) {
      time_GetTimeMonotonic(&now);
      dtf = time_AdiffToFloat(&now, &last);
      if (dtf >= scan_time) {
	last = now;
	break;
      }
      rc = mosquitto_loop(mosq, 1000*(scan_time - dtf), 1);
      if (rc)
	break;
    }
    if (rc) {
      printf("Error %d\n", rc);
      sleep(10);
      mosquitto_reconnect(mosq);
    }
    else {

      for (int i = 0; i < subs.size(); i++) {
	char rmsg[200];
	switch (subs[i].tid) {
	case pwr_eType_Float32:
	  sprintf(rmsg, "{\"subref\":%d,\"status\":%u,\"value\":%e}", subs[i].subref, subs[i].sts, *(pwr_tFloat32*)subs[i].valp);
	  break;
	case pwr_eType_Boolean:
	  sprintf(rmsg, "{\"subref\":%d,\"status\":%u,\"value\":%d}", subs[i].subref, subs[i].sts, *(pwr_tBoolean*)subs[i].valp);
	  break;
	case pwr_eType_Int32:
	  sprintf(rmsg, "{\"subref\":%d,\"status\":%u,\"value\":%d}", subs[i].subref, subs[i].sts, *(pwr_tInt32*)subs[i].valp);
	  break;
	}
	if (debug)
	  printf("sub: %s %s\n", subs[i].reply, rmsg);
	rc = mosquitto_publish(mosq, NULL, subs[i].reply, strlen(rmsg), 
	    rmsg, 1, 0);
	subs[i].cnt++;
	if (subs[i].cnt > subs[i].duration) {
	  if (debug)
	    printf("Remove sub %s\n", subs[i].attribute);
	  subs[i].unref();
	  subs.erase(subs.begin() + i);
	  i--;
	}
      }
      for (int i = 0; i < sublists.size(); i++) {
	int msize;
	char *rmsg;
	int n = 0;
	  
	msize = 17;
	for (int j = 0; j < sublists[i].slist.size(); j++) {
	  switch (sublists[i].slist[j].tid) {
	  case pwr_eType_Float32:
	    msize += 7 + 10 + 13 + 2;
	    break;
	  case pwr_eType_Boolean:
	    msize += 7 + 10 + 1 + 2;
	    break;
	  case pwr_eType_Int32:
	    msize += 7 + 10 + 10 + 2;
	    break;
	  default:
	    ;
	  }
	}
	rmsg = (char *)calloc(1, msize);

	n += sprintf(rmsg, "{\"subref\":%d,\"a\":[", sublists[i].subref);
	for (int j = 0; j < sublists[i].slist.size(); j++) {
	  switch (sublists[i].slist[j].tid) {
	  case pwr_eType_Float32:
	    n += sprintf(&rmsg[strlen(rmsg)], "{\"idx\":%u,\"value\":%g}", sublists[i].slist[j].idx,
		  *(pwr_tFloat32*)sublists[i].slist[j].valp);
	    break;
	  case pwr_eType_Boolean:
	    n += sprintf(&rmsg[strlen(rmsg)], "{\"idx\":%u,\"value\":%d}", sublists[i].slist[j].idx,
		  *(pwr_tBoolean*)sublists[i].slist[j].valp);
	    break;
	  case pwr_eType_Int32:
	    n += sprintf(&rmsg[strlen(rmsg)], "{\"idx\":%u,\"value\":%d}", sublists[i].slist[j].idx,
		  *(pwr_tInt32*)sublists[i].slist[j].valp);
	    break;
	  }
	  if (j != sublists[i].slist.size() - 1) {
	    strcat(rmsg, ",");
	    n += 1;
	  }
	}
	strcat(rmsg, "]}");
	n += 2;
	if (debug)
	  printf("sublist: %s %s\n", sublists[i].reply, rmsg);
	if (debug)
	  printf("msize: %d %d %d\n", msize, n, (int)strlen(rmsg));
	rc = mosquitto_publish(mosq, NULL, sublists[i].reply, strlen(rmsg), 
				 rmsg, 1, 0);
	sublists[i].cnt++;
	free(rmsg);

	if (sublists[i].cnt > sublists[i].duration) {
	  if (debug)
	    printf("Remove sublist %s\n", sublists[i].reply);
	  sublists[i].unref();
	  sublists.erase(sublists.begin() + i);
	  i--;
	}
      }
    }
  }

  return NULL;
}

int MqttServer::open(int restart)
{
  pwr_tStatus sts;
  pwr_tOid confoid;
  int rc;

  sts = gdh_GetClassList(pwr_cClass_MqttServer, &confoid);
  if (EVEN(sts)) {
    errh_Info("Not configured, terminating");
    exit(0);
  }
  sts = gdh_ObjidToPointer(confoid, (void **)&srv->conf);
  if (EVEN(sts))
    return sts;
  
  if (!restart) {
    if (strcmp(srv->conf->Server, "") == 0)
      strcpy(srv->conf->Server, "localhost");

    /* Connect to mqtt server */
    sts = srv->mqtt_connect();

    /* Create mosquitto loop thread */
    rc = pthread_create(&srv->loop_thread, NULL, srv->mqtt_loop, 0);
    if (rc)
      return 0;
  }
  else {
    // Setup new subscriptions
    for (int i = 0; i < subs.size(); i++)
      subs[i].ref();
    for (int i = 0; i < sublists.size(); i++)
      sublists[i].ref();
  }

  aproc_RegisterObject(confoid);
  return 1;
}

void MqttServer::close()
{
  for (int i = 0; i < subs.size(); i++)
    subs[i].unref();
  for (int i = 0; i < sublists.size(); i++)
    sublists[i].unref();
}


void MqttServer::init(qcom_sQid* qid)
{
  qcom_sQid qini;
  qcom_sQattr qAttr;
  pwr_tStatus sts;

  sts = gdh_Init("rt_mqtt_server");
  if (EVEN(sts)) {
    errh_Fatal("gdh_Init, %m", sts);
    exit(sts);
  }

  errh_Init("pwr_mqtt_server", errh_eAnix_mqtt_server);
  errh_SetStatus(PWR__SRVSTARTUP);

  if (!qcom_Init(&sts, 0, "pwr_mqtt_server")) {
    errh_Fatal("qcom_Init, %m", sts);
    errh_SetStatus(PWR__SRVTERM);
    exit(sts);
  }

  qAttr.type = qcom_eQtype_private;
  qAttr.quota = 100;
  if (!qcom_CreateQ(&sts, qid, &qAttr, "events")) {
    errh_Fatal("qcom_CreateQ, %m", sts);
    errh_SetStatus(PWR__SRVTERM);
    exit(sts);
  }

  qini = qcom_cQini;
  if (!qcom_Bind(&sts, qid, &qini)) {
    errh_Fatal("qcom_Bind(Qini), %m", sts);
    errh_SetStatus(PWR__SRVTERM);
    exit(-1);
  }
}

int main()
{
  pwr_tStatus sts;
  int tmo = 0;
  char mp[2000];
  qcom_sQid qid = qcom_cNQid;
  qcom_sGet get;
  int swap = 0;
  bool first_scan = true;
  float scantime = 2;

  srv = new MqttServer();
  srv->init(&qid);

  sts = srv->open(0);
  if (EVEN(sts)) {
    errh_Error("Open error, %m", sts);
    errh_Fatal("rt_mqtt_server aborting");
    errh_SetStatus(PWR__SRVTERM);
    exit(0);
  }

  aproc_TimeStamp(scantime, 10);
  errh_SetStatus(PWR__SRUN);

  first_scan = true;
  for (;;) {
    if (first_scan) {
      tmo = (int)(scantime * 1000 - 1);
    }

    get.maxSize = sizeof(mp);
    get.data = mp;
    qcom_Get(&sts, &qid, &get, tmo);
    if (!(sts == QCOM__TMO || sts == QCOM__QEMPTY)) {
      ini_mEvent new_event;
      qcom_sEvent* ep = (qcom_sEvent*)get.data;

      new_event.m = ep->mask;
      if (new_event.b.oldPlcStop && !swap) {
        errh_SetStatus(PWR__SRVRESTART);
        srv->conf->Status = PWR__SRVRESTART;
        swap = 1;
        srv->close();
      } else if (new_event.b.swapDone && swap) {
        swap = 0;
        srv->open(1);
        errh_SetStatus(PWR__SRUN);
        srv->conf->Status = PWR__SRUN;
      } else if (new_event.b.terminate) {
        exit(0);
      }
    }
    first_scan = false;
    aproc_TimeStamp(scantime, 10);
  }
}

