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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <clidef.h>
#include <climsgdef.h>
#include <ssdef.h>
#include <descrip.h>
#include <lib$routines.h>
#include <processes.h>
#include <smgdef.h>
#include <trmdef.h>

#include "pwr.h"
#include "tlog_diff.h"

/* What should be included to declare this ????? */
int cli$get_value();
int cli$present();

static unsigned long userlist = 0;

int sutil_get_qualval(qual, qual_val, sendlen) char* qual;
char* qual_val;
int sendlen;

/*
  qual_val should be the address of the array of char and not a pointer to
  char
*/
{
  struct dsc$descriptor_s q1desc;
  struct dsc$descriptor_s q1strdesc;
  int i;
  short len;
  long sts;

  /*
  Notice that cli$get_value does not terminate the string with '\0' but
  return his length instead.
  */

  /* a descriptor is a complicated object ! */
  q1desc.dsc$b_dtype = DSC$K_DTYPE_T;
  q1desc.dsc$b_class = DSC$K_CLASS_S;
  q1desc.dsc$a_pointer = qual;
  q1desc.dsc$w_length = strlen(qual);

  q1strdesc.dsc$b_dtype = DSC$K_DTYPE_T;
  q1strdesc.dsc$b_class = DSC$K_CLASS_S;
  q1strdesc.dsc$a_pointer = qual_val;
  q1strdesc.dsc$w_length = sendlen;

  /* I have not found a way to oblige the user to enter a qualifier
  so you have to check here if there is one */

  sts = cli$present(&q1desc);
  if (sts != CLI$_PRESENT)
    return 0;
  else {
    sts = cli$get_value(&q1desc, &q1strdesc, &len);
    if (sts == CLI$_ABSENT)
      return 0;
    else if (sts == CLI$_COMMA) {
      while (sts == CLI$_COMMA) {
        *(q1strdesc.dsc$a_pointer + len) = ',';
        q1strdesc.dsc$a_pointer += len + 1;
        q1strdesc.dsc$w_length -= len + 1;
        sts = cli$get_value(&q1desc, &q1strdesc, &len);
      }
      if (sts != SS$_NORMAL) {
        printf("sutil_getqual_val : ERROR no end of list of qual values\n");
        return 0;
      }
    }
    *(q1strdesc.dsc$a_pointer + len) = '\0';
  }
  return 1;
}

int sutil_get_qual(qual, condval_ptr) char* qual;
long* condval_ptr;

/*
  check the presence of the qualifier which name is sended in the qual argument
  in the last parsed command.
  you should call this rtn to chack the presence of qualifier to which
  there is no associated value.

  the return status is true if
  the qual is present (even by default ) and not negated in the last parsed
  command
  (the condition value returned is not CLI$_ABSENT ).

  false otherwise : in this case *condval_ptr contain the condition value :
  CLI$_LOCPRES , CLI$_LOCNEG , CLI$_DEFAULTED

*/

{
  struct dsc$descriptor_s q1desc;
  long sts;

  /* a descriptor is a complicated object ! */
  q1desc.dsc$b_dtype = DSC$K_DTYPE_T;
  q1desc.dsc$b_class = DSC$K_CLASS_S;
  q1desc.dsc$a_pointer = qual;
  q1desc.dsc$w_length = strlen(qual);

  /* I have not found a way to oblige the user to enter a qualifier
  so you have to check here if there is one */

  *condval_ptr = cli$present(&q1desc);
  if (*condval_ptr != CLI$_ABSENT && *condval_ptr != CLI$_NEGATED)
    return 1;
  return 0;
}

int scli_help()
{
  char command[120];

  strcpy(command, "help/libra=ssab_lib:ds_ssabutl_hlp ssab_utils ssab_utl");
  system(command);
  return 1;
}

int scli_quit()
{
  /* Quit */
  exit(1);
  return 1;
}
int scli_exit()
{
  exit(1);
  return 1;
}

int scli_tlog_difference()
{
  char filestr[80];
  char outputstr[80];
  char* outputstr_p;
  char timestr[80];
  char* timestr_p;
  char sincestr[80];
  char* sincestr_p;
  char beforestr[80];
  char* beforestr_p;
  int parallell;
  int attribute;
  int text;
  int ttext;
  int noorder;
  int exact;
  int sts;
  int condval;

  if (!sutil_get_qualval("file", filestr, 80))
    strcpy(filestr, "*");

  if (sutil_get_qualval("output", outputstr, 80))
    outputstr_p = outputstr;
  else
    outputstr_p = NULL;
  if (sutil_get_qualval("time", timestr, 80))
    timestr_p = timestr;
  else
    timestr_p = NULL;
  parallell = sutil_get_qual("parallell", &sts);
  attribute = sutil_get_qual("attribute", &sts);
  text = sutil_get_qual("text", &sts);
  ttext = sutil_get_qual("ttext", &sts);
  noorder = sutil_get_qual("noorder", &sts);
  exact = sutil_get_qual("exact", &sts);

  if (sutil_get_qual("since", &sts)) {
    if (!sutil_get_qualval("since", sincestr, 80))
      strcpy(sincestr, "");
    sincestr_p = sincestr;
  } else
    sincestr_p = NULL;

  if (sutil_get_qual("before", &sts)) {
    if (!sutil_get_qualval("before", beforestr, 80))
      strcpy(beforestr, "");
    beforestr_p = beforestr;
  } else
    beforestr_p = NULL;

  sts = tlog_diff(filestr, outputstr_p, timestr_p, parallell, attribute, text,
      ttext, noorder, exact, sincestr_p, beforestr_p);
  return sts;
}

int scli_tlog_save()
{
  char filestr[80];
  int sts;
  int condval;

  sutil_get_qualval("file", filestr, 80);

  sts = tlog_save(filestr);
  return sts;
}
