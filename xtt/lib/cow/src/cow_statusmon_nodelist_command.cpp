/*
 * ProviewR   Open Source Process Control.
 * Copyright (C) 2005-2023 SSAB EMEA AB.
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

/* ge_nodelist_command.cpp
   This module contains routines for handling of command line in ge. */

#include <stdlib.h>

#include "co_ccm.h"
#include "co_ccm_msg.h"
#include "co_dcli.h"
#include "co_dcli_msg.h"
#include "co_string.h"
#include "cow_statusmon_nodelist.h"

static int nodelist_select_func(void* client_data, void* client_flag);
static int nodelist_open_func(void* client_data, void* client_flag);

dcli_tCmdTable nodelist_command_table[] = { 
  { "SELECT", &nodelist_select_func, { "dcli_arg1", "/NODE", "" } },
  { "OPEN", &nodelist_open_func, { "dcli_arg1", "dcli_arg2",
      "/WIDTH", "/HEIGHT", "" } },
  { "", NULL, { "" } }
};

static int nodelist_select_func(void* client_data, void* client_flag)
{
  Nodelist* nl = (Nodelist*)client_data;

  char arg1_str[80];
  int arg1_sts;

  arg1_sts = dcli_get_qualifier("dcli_arg1", arg1_str, sizeof(arg1_str));

  if (str_NoCaseStrncmp(arg1_str, "NODE", strlen(arg1_str)) == 0) {
    // Command is "SELECT NODE"
    char node_str[80];
    int sts;

    if (EVEN(dcli_get_qualifier("/NODE", node_str, sizeof(node_str)))) {
      nl->message('E', "Node is missing");
      return DCLI__QUAL_NODEF;
    }

    sts = nl->nodelistnav->select_node(node_str);   
    if (EVEN(sts))
      nl->message('E', "No such node");
    else
      nl->pop();

  } else {
    nl->message('E', "Unknown qualifier");
    return DCLI__QUAL_NODEF;
  }
  return DCLI__SUCCESS;
}

static int nodelist_open_func(void* client_data, void* client_flag)
{
  Nodelist* nl = (Nodelist*)client_data;

  char arg1_str[80];
  int arg1_sts;

  arg1_sts = dcli_get_qualifier("dcli_arg1", arg1_str, sizeof(arg1_str));

  if (str_NoCaseStrncmp(arg1_str, "GRAPH", strlen(arg1_str)) == 0) {
    // Command is "OPEN GRAPH"
    char graph_str[80];
    int sts;
    int nr;
    char tmp_str[80];
    int width;
    int height;

    sts = dcli_get_qualifier("dcli_arg2", graph_str, sizeof(graph_str));

    if (ODD(dcli_get_qualifier("/WIDTH", tmp_str, sizeof(tmp_str)))) {
      nr = sscanf(tmp_str, "%d", &width);
      if (nr != 1) {
	nl->message('E', "Syntax error in width");
	return DCLI__SYNTAX;
      }
    } else
      width = 0;
    
    if (ODD(dcli_get_qualifier("/HEIGHT", tmp_str, sizeof(tmp_str)))) {
      nr = sscanf(tmp_str, "%d", &height);
      if (nr != 1) {
	nl->message('E', "Syntax error in height");
	return DCLI__SYNTAX;
      }
    } else
      height = 0;

    sts = nl->open_graph(graph_str, width, height);   
    if (EVEN(sts))
      nl->message('E', "Open graph error");

  } else {
    nl->message('E', "Unknown qualifier");
    return DCLI__QUAL_NODEF;
  }
  return DCLI__SUCCESS;
}

int Nodelist::command(char* input_str)
{
  char command[160];
  int sts, sym_sts;
  char symbol_value[80];

  if (input_str[0] == '@') {
    /* Read command file */
    // char *s;
    // if ( (s = strchr( input_str, ' ')))
    //   dcli_toupper( s, s);
    sts = dcli_replace_symbol(input_str, command, sizeof(command));
    if (EVEN(sts))
      return sts;

    sts = readcmdfile(&command[1], 0);
    if (sts == DCLI__NOFILE) {
      char tmp[200];
      snprintf(tmp, 200, "Unable to open file \"%s\"", &command[1]);
      message('E', tmp);
      return DCLI__SUCCESS;
    } else if (EVEN(sts))
      return sts;
    return DCLI__SUCCESS;
  }

  // dcli_toupper( input_str, input_str);
  sts = dcli_replace_symbol(input_str, command, sizeof(command));
  if (EVEN(sts))
    return sts;

  sts = dcli_cli(
      (dcli_tCmdTable*)&nodelist_command_table, command, (void*)this, 0);
  if (sts == DCLI__COM_NODEF) {
    /* Try to find a matching symbol */
    sym_sts = dcli_get_symbol_cmd(command, symbol_value);
    if (ODD(sym_sts)) {
      if (symbol_value[0] == '@') {
        /* Read command file */
        sts = readcmdfile(&symbol_value[1], 0);
        if (sts == DCLI__NOFILE) {
          char tmp[200];
          snprintf(tmp, 200, "Unable to open file \"%s\"", &symbol_value[1]);
          message('E', tmp);
          return DCLI__SUCCESS;
        } else if (EVEN(sts))
          return sts;
        return DCLI__SUCCESS;
      }
      sts = dcli_cli(
          (dcli_tCmdTable*)&nodelist_command_table, symbol_value, (void*)this, 0);
    } else if (sym_sts == DCLI__SYMBOL_AMBIG)
      sts = sym_sts;
  }
  if (sts == DCLI__COM_AMBIG)
    message('E', "Ambiguous command");
  else if (sts == DCLI__COM_NODEF)
    message('E', "Undefined command");
  else if (sts == DCLI__QUAL_AMBIG)
    message('E', "Ambiguous qualifier");
  else if (sts == DCLI__QUAL_NODEF)
    message('E', "Undefined qualifier");
  else if (sts == DCLI__SYMBOL_AMBIG)
    message('E', "Ambiguous symbol abbrevation");

  return sts;
}

static int nodelist_ccm_deffilename_func(
    char* outfile, char* infile, void* client_data)
{
  dcli_get_defaultfilename(infile, outfile, ".nl_com");
  dcli_translate_filename(outfile, outfile);
  return 1;
}

static int nodelist_ccm_errormessage_func(
    char* msg, int severity, void* client_data)
{
  Nodelist* nl = (Nodelist*)client_data;

  if (EVEN(severity))
    nl->message('I', msg);
  else
    nl->message('E', msg);
  return 1;
}

int nodelist_externcmd_func(char* cmd, void* client_data)
{
  Nodelist* nl = (Nodelist*)client_data;
  int sts;

  nl->scriptmode++;
  sts = nl->command(cmd);
  nl->scriptmode--;
  return sts;
}

/************************************************************************
*
* Name:		readcmdfile()
*
* Type		int
*
* Type		Parameter	IOGF	Description
*
* Description:
*
**************************************************************************/

int Nodelist::readcmdfile(char* incommand, char *script)
{
  char input_str[160];
  int sts;
  int appl_sts;

  if (!ccm_func_registred) {
    ccm_func_registred = 1;

    //script_func_register();

  }

  //script_store_nodelist();

  // Read and execute the command file
  if (incommand) {
    str_trim(input_str, incommand);
    sts = ccm_file_exec(input_str, nodelist_externcmd_func,
        nodelist_ccm_deffilename_func, nodelist_ccm_errormessage_func, &appl_sts,
	verify, 0, NULL, 0, 0, NULL, (void*)this);
  }
  else if (script) {
    sts = ccm_buffer_exec(script, 0, nodelist_externcmd_func,
	nodelist_ccm_deffilename_func, nodelist_ccm_errormessage_func, &appl_sts, verify,
	0, NULL, 0, NULL, (void *)this);
  }
  if (EVEN(sts))
    return sts;
  return 1;
}