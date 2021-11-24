/*
  Print test status
*/

#include <stdio.h>
#include "pwr.h"
#include "co_cdh.h"
#include "rt_gdh.h"
#include "co_tst_log.h"

static char item[][80] = {
  "Test01f-Remote-RemSerial",
  "Test01f-Remote-RemUDP",
  "Test01f-Remote-RemTCP"
};

int main()
{
  pwr_tStatus sts;
  unsigned int i;
  pwr_tAName aname;

  tst_log *log = new tst_log(&sts, "rt-Remote", "$pwrp_log/remote.tlog");
  if (EVEN(sts))
    printf("** Unable to open log file");

  sts = gdh_Init("ra_remotetest");
  if (EVEN(sts)) {
    printf("E pwrtest01 status, gdh_Init error status: %u\n", sts);
    exit(0);
  }
  
  for (i = 0; i < sizeof(item)/sizeof(item[0]); i++) {
    pwr_tBoolean val;
    pwr_tInt32 state;

    strcpy(aname, item[i]);
    strcat(aname, "-TestExecuted.ActualValue");
    sts = gdh_GetObjectInfo(aname, &val, sizeof(val));
    if (EVEN(sts)) {
      log->log('E', "Can't find test object", item[i], sts);
      continue;
    }
    if (val != 1) {
      log->log('E', "Not tested", item[i]);
      continue;
    }

    strcpy(aname, item[i]);
    strcat(aname, "-ErrorDetected.ActualValue");
    sts = gdh_GetObjectInfo(aname, &val, sizeof(val));
    if (EVEN(sts)) {
      log->log('E', "gdh_GetObjectInfo", item[i], sts);
      continue;
    }
    if (val == 1) {
      char state_str[80];

      strcpy(aname, item[i]);
      strcat(aname, "-ErrorState.ActualValue");
      sts = gdh_GetObjectInfo(aname, &state, sizeof(state));
      if (EVEN(sts)) {
	log->log('E', "State not found", item[i], sts);
	state = -1;
      }
      sprintf(state_str, "error state %u", state);
      log->log('E', item[i], state_str);
    }
    else
      log->log('I', "Test ok", item[i]);
  }
}
