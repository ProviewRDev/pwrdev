#include <float.h>
#include <math.h>
#include "pwr.h"
#include "rt_plc.h"
#include "ra_plc_user.h"



void Pwrt01_NotEqual_exec(plc_sThread *tp, pwr_sClass_Pwrt01_NotEqual *o)
{
  o->In1 = *o->In1P;
  o->In2 = *o->In2P;
  if ((isnan(o->In1) && isnan(o->In2)) || 
      (isinf(o->In1) && isinf(o->In2)))
    o->Status = 0;
  else
    o->Status = !(fabsf(o->In1 - o->In2) < o->MaxValue * FLT_EPSILON);
}

void pwrt01_random_string(char *str, int size)
{
  static char c[] = "abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ0123456789";
  int idx;
  int len;
  int i;

  len = (int)(((float)rand()/RAND_MAX) * (size - 1));
  len++;

  for (i = 0; i < len; i++) {
    idx = (int)(((float)rand()/RAND_MAX) * (sizeof(c)-1));
    str[i] = c[idx];
  }
  str[len] = 0;
}

void pwrt01_random_buf(void *buf, int size)
{
  int i;

  for (i = 0; i < size; i++)
    ((unsigned char *)buf)[i] = (unsigned char)(((float)rand()/RAND_MAX) * 256);
}
