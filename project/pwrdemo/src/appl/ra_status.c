/*
  Print current project status
*/

#include <stdio.h>
#include "pwr.h"
#include "rt_gdh.h"

int main()
{
  pwr_tStatus sts, system_sts;

  sts = gdh_Init("ra_status");
  if (EVEN(sts)) {
    printf("E pwrdemo status, gdh_Init error status: %u\n", sts);
    exit(0);
  }
  
  sts = gdh_GetObjectInfo("Nodes-DemoNode.SystemStatus", &system_sts,
			  sizeof(system_sts));
  if (EVEN(sts)) {
    printf("E pwrdemo status, gdh_GetObjectInfo error status: %u\n", sts);
    exit(0);
  }

  if (ODD(system_sts))
    printf("I pwrdemo status success: %u\n", system_sts);
  else
    printf("E pwrdemo status error: %u\n", system_sts);
}