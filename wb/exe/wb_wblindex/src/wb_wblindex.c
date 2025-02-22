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

/* wb_wblindex.c
   Set index in wb_load files. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "co_math.h"

#define GSX__SUCCESS 1
#define GSX__CONVWBLSYNTAX 22
#define GSX__CONVWBLBUFSIZE 32
#define GSX__NOMEMORY 42

static int typedef_idx = 0;
static int classdef_idx = 0;
static int attribute_idx = 0;
static int idx_count;
static int replace_idx;
static int noclassdef;

typedef int pwr_tStatus;

typedef struct convwbl_s_ctx* convwbl_ctx;

typedef struct {
  char item[20];
  char delim_front[20];
  char delim_back[20];
  pwr_tStatus (*func)(convwbl_ctx, char*, char*, char*, int*, char*);
  int hit;
  int hit_count;
} convwbl_t_item;

struct convwbl_s_ctx {
  int line_count;
  int comment;
  int outdata1_eq_indata;
  int outdata2_eq_indata;
  int outdata3_eq_indata;
  int outdata4_eq_indata;
  char classdef_Da1[80];
  char classdef_Da2[80];
  char classdef_Da3[80];
  char classdef_Da4[80];
  char aliasdef_Da1[80];
  char aliasdef_Da2[80];
  char aliasdef_Da3[80];
  char aliasdef_Da4[80];
  convwbl_t_item items[100];
};

/* Local function prototypes. */

static int convwbl_parse(char* string, char* parse_char, char* inc_parse_char,
    char* outstr, int max_rows, int max_cols);
static int convwbl_add_item(convwbl_ctx convwblctx, char* item,
    char* delim_front, char* delim_back, pwr_tStatus (*func)());

/*************************************************************************
*
* Name:		-
*
* Type		int
*
* Type		Parameter	IOGF	Description
* convwbl_ctx	convwblctx	I	convwbl context
* char		*line		I	current line
* char		*pos		I	position in current line
* char		*out		IO	output buffer
* char		*var		I	name of pointer to object
*
* Description:
*	Backcall funktions called when an item is found.
*
**************************************************************************/

/*************************************************************************
*
* Name:		convwbl_comment_start
*
* Type		int
*
* Type		Parameter	IOGF	Description
* convwbl_ctx	convwblctx	I	convwbl context
* char		*line		I	current line
* char		*pos		I	position in current line
* char		*out		IO	output buffer
* char		*var		I	name of pointer to object
*
* Description:
*	Backcall funktions called when an comment start is found.
*	Set convwblctx->comment to stop conversion.
*
**************************************************************************/

static pwr_tStatus convwbl_comment_start(convwbl_ctx convwblctx, char* line,
    char* pos, char* out, int* incr, char* var)
{
  sprintf(out, "/*");
  *incr = strlen(out);
  convwblctx->comment = 1;
  return GSX__SUCCESS;
}

/*************************************************************************
*
* Name:		convwbl_comment_end
*
* Type		static int
*
* Type		Parameter	IOGF	Description
* convwbl_ctx	convwblctx	I	convwbl context
* char		*line		I	current line
* char		*pos		I	position in current line
* char		*out		IO	output buffer
* char		*var		I	name of pointer to object
*
* Description:
*	Backcall funktions called when an comment end is found.
*	Reset convwblctx->comment to start conversion again.
*
**************************************************************************/
static pwr_tStatus convwbl_comment_end(convwbl_ctx convwblctx, char* line,
    char* pos, char* out, int* incr, char* var)
{
  sprintf(out, "*/");
  *incr = strlen(out);
  convwblctx->comment = 0;
  return GSX__SUCCESS;
}

/*************************************************************************
*
* Name:		convwbl_Dax
*
* Type		int
*
* Type		Parameter	IOGF	Description
* convwbl_ctx	convwblctx	I	convwbl context
* char		*line		I	current line
* char		*pos		I	position in current line
* char		*out		IO	output buffer
* char		*var		I	name of pointer to object
*
* Description:
*	Backcall funktions called when a datapointer 'Da1' is found.
* 	If a 'ODay = Dax' is detected the 'convwblctx->outdatax_eq_indata' flag
* 	is set previously by the ODay backcall function, and the code should
*	be a memcpy.
*
**************************************************************************/

static pwr_tStatus convwbl_ClassDef(convwbl_ctx convwblctx, char* line,
    char* pos, char* out, int* incr, char* var)
{
  int num;

  if (noclassdef) {
    sprintf(out, "$ClassDef");
    *incr = strlen(out);
    return GSX__SUCCESS;
  }

  if (!replace_idx)
    sprintf(out, "$ClassDef %d", classdef_idx);
  else {
    sprintf(out, "$ClassDef %d\n", classdef_idx);
    *pos = 0;
  }
  classdef_idx++;
  idx_count++;

  *incr = strlen(out);

  return GSX__SUCCESS;
}
static pwr_tStatus convwbl_TypeDef(convwbl_ctx convwblctx, char* line,
    char* pos, char* out, int* incr, char* var)
{
  if (noclassdef) {
    sprintf(out, "$TypeDef");
    *incr = strlen(out);
    return GSX__SUCCESS;
  }

  if (!replace_idx)
    sprintf(out, "$TypeDef %d", typedef_idx);
  else {
    sprintf(out, "$TypeDef %d\n", typedef_idx);
    *pos = 0;
  }

  typedef_idx++;
  idx_count++;
  *incr = strlen(out);
  return GSX__SUCCESS;
}
static pwr_tStatus convwbl_Attribute(convwbl_ctx convwblctx, char* line,
    char* pos, char* out, int* incr, char* var)
{
  if (!strstr(line, "Object")) {
    sprintf(out, "$Attribute");
    *incr = strlen(out);
    return GSX__SUCCESS;
  }
  attribute_idx++;
  idx_count++;
  if (!replace_idx)
    sprintf(out, "$Attribute %d", attribute_idx);
  else {
    sprintf(out, "$Attribute %d\n", attribute_idx);
    *pos = 0;
  }
  *incr = strlen(out);
  return GSX__SUCCESS;
}
static pwr_tStatus convwbl_ObjXRef(convwbl_ctx convwblctx, char* line,
    char* pos, char* out, int* incr, char* var)
{
  if (!strstr(line, "Object")) {
    sprintf(out, "$ObjXRef");
    *incr = strlen(out);
    return GSX__SUCCESS;
  }
  attribute_idx++;
  idx_count++;
  if (!replace_idx)
    sprintf(out, "$ObjXRef %d", attribute_idx);
  else {
    sprintf(out, "$ObjXRef %d\n", attribute_idx);
    *pos = 0;
  }
  *incr = strlen(out);
  return GSX__SUCCESS;
}
static pwr_tStatus convwbl_AttrXRef(convwbl_ctx convwblctx, char* line,
    char* pos, char* out, int* incr, char* var)
{
  if (!strstr(line, "Object")) {
    sprintf(out, "$AttrXRef");
    *incr = strlen(out);
    return GSX__SUCCESS;
  }
  attribute_idx++;
  idx_count++;
  if (!replace_idx)
    sprintf(out, "$AttrXRef %d", attribute_idx);
  else {
    sprintf(out, "$AttrXRef %d\n", attribute_idx);
    *pos = 0;
  }
  *incr = strlen(out);
  return GSX__SUCCESS;
}
static pwr_tStatus convwbl_Intern(convwbl_ctx convwblctx, char* line, char* pos,
    char* out, int* incr, char* var)
{
  if (!strstr(line, "Object")) {
    sprintf(out, "$Intern");
    *incr = strlen(out);
    return GSX__SUCCESS;
  }
  attribute_idx++;
  idx_count++;
  if (!replace_idx)
    sprintf(out, "$Intern %d", attribute_idx);
  else {
    sprintf(out, "$Intern %d\n", attribute_idx);
    *pos = 0;
  }
  *incr = strlen(out);
  return GSX__SUCCESS;
}
static pwr_tStatus convwbl_Input(convwbl_ctx convwblctx, char* line, char* pos,
    char* out, int* incr, char* var)
{
  if (!strstr(line, "Object")) {
    sprintf(out, "$Input");
    *incr = strlen(out);
    return GSX__SUCCESS;
  }
  attribute_idx++;
  idx_count++;
  if (!replace_idx)
    sprintf(out, "$Input %d", attribute_idx);
  else {
    sprintf(out, "$Input %d\n", attribute_idx);
    *pos = 0;
  }
  *incr = strlen(out);
  return GSX__SUCCESS;
}
static pwr_tStatus convwbl_Output(convwbl_ctx convwblctx, char* line, char* pos,
    char* out, int* incr, char* var)
{
  if (!strstr(line, "Object")) {
    sprintf(out, "$Output");
    *incr = strlen(out);
    return GSX__SUCCESS;
  }
  attribute_idx++;
  idx_count++;
  if (!replace_idx)
    sprintf(out, "$Output %d", attribute_idx);
  else {
    sprintf(out, "$Output %d\n", attribute_idx);
    *pos = 0;
  }
  *incr = strlen(out);
  return GSX__SUCCESS;
}
static pwr_tStatus convwbl_Buffer(convwbl_ctx convwblctx, char* line, char* pos,
    char* out, int* incr, char* var)
{
  if (!strstr(line, "Object")) {
    sprintf(out, "$Buffer");
    *incr = strlen(out);
    return GSX__SUCCESS;
  }
  attribute_idx++;
  idx_count++;
  if (!replace_idx)
    sprintf(out, "$Buffer %d", attribute_idx);
  else {
    sprintf(out, "$Buffer %d\n", attribute_idx);
    *pos = 0;
  }
  *incr = strlen(out);
  return GSX__SUCCESS;
}
static pwr_tStatus convwbl_ObjBodyDef(convwbl_ctx convwblctx, char* line,
    char* pos, char* out, int* incr, char* var)
{
  if (!replace_idx) {
    if (strstr(line, "RtBody"))
      sprintf(out, "$ObjBodyDef 1");
    else if (strstr(line, "SysBody"))
      sprintf(out, "$ObjBodyDef 1");
    else if (strstr(line, "DevBody"))
      sprintf(out, "$ObjBodyDef 2");
    else {
      sprintf(out, "$ObjBodyDef");
      *incr = strlen(out);
      return GSX__SUCCESS;
    }
  } else {
    if (strstr(line, "RtBody"))
      sprintf(out, "$ObjBodyDef 1\n");
    else if (strstr(line, "SysBody"))
      sprintf(out, "$ObjBodyDef 1\n");
    else if (strstr(line, "DevBody"))
      sprintf(out, "$ObjBodyDef 2\n");
    else {
      sprintf(out, "$ObjBodyDef");
      *incr = strlen(out);
      return GSX__SUCCESS;
    }
    *pos = 0;
  }
  *incr = strlen(out);
  attribute_idx = 0;
  idx_count++;
  return GSX__SUCCESS;
}

/* Items includes also 32 dynamic items */
convwbl_t_item template_items[100]
    = { { "*/", "X", "X", &convwbl_comment_end, 0, 0 },
        { "/*", "X", "X", &convwbl_comment_start, 0, 0 },
        { "$ClassDef", "A", "A", &convwbl_ClassDef, 0, 0 },
        { "$TypeDef", "A", "A", &convwbl_TypeDef, 0, 0 },
        { "$Attribute", "A", "A", &convwbl_Attribute, 0, 0 },
        { "$ObjXRef", "A", "A", &convwbl_ObjXRef, 0, 0 },
        { "$AttrXRef", "A", "A", &convwbl_AttrXRef, 0, 0 },
        { "$Param", "A", "A", &convwbl_Attribute, 0, 0 },
        { "$Input", "A", "A", &convwbl_Input, 0, 0 },
        { "$Output", "A", "A", &convwbl_Output, 0, 0 },
        { "$Intern", "A", "A", &convwbl_Intern, 0, 0 },
        { "$Buffer", "A", "A", &convwbl_Buffer, 0, 0 },
        { "$ObjBodyDef", "A", "A", &convwbl_ObjBodyDef, 0, 0 }, { "" } };

/*************************************************************************
*
* Name:		convwbl_parse()
*
* Type		int
*
* Type		Parameter	IOGF	Description
* char		*string		I	string to be parsed.
* char		*parse_char	I	parse charachter(s).
* char		*inc_parse_char	I	parse charachter(s) that will be
*					included in the parsed string.
* char		*outstr		O	parsed strings.
* int		max_rows	I	maximum number of chars in a parsed
*					string.
* int 		max_cols	I	maximum number of parsed elements.
*
* Description:
*	Parses a string.
*
**************************************************************************/

static int convwbl_parse(char* string, char* parse_char, char* inc_parse_char,
    char* outstr, int max_rows, int max_cols)
{
  int row;
  int col;
  char* char_ptr;
  char* inc_char_ptr;
  int parsechar_found;
  int inc_parsechar_found;
  int next_token;
  int char_found;
  int one_token = 0;

  row = 0;
  col = 0;
  char_found = 0;
  next_token = 0;
  while (*string != '\0') {
    char_ptr = parse_char;
    inc_char_ptr = inc_parse_char;
    parsechar_found = 0;
    inc_parsechar_found = 0;
    if (*string == '"') {
      one_token = !one_token;
      string++;
      continue;
    }
    if (!one_token) {
      while (*char_ptr != '\0') {
        /* Check if this is a parse charachter */
        if (*string == *char_ptr) {
          parsechar_found = 1;
          /* Next token */
          if (col > 0) {
            *(outstr + row * max_cols + col) = '\0';
            row++;
            if (row >= max_rows)
              return row;
            col = 0;
            next_token = 0;
          }
          break;
        }
        char_ptr++;
      }
      while (*inc_char_ptr != '\0') {
        /* Check if this is a parse charachter */
        if (*string == *inc_char_ptr) {
          parsechar_found = 1;
          inc_parsechar_found = 1;
          /* Next token */
          if (col > 0) {
            *(outstr + row * max_cols + col) = '\0';
            row++;
            if (row >= max_rows)
              return row;
            col = 0;
            next_token = 0;
          }
          break;
        }
        inc_char_ptr++;
      }
    }
    if (!parsechar_found && !next_token) {
      char_found++;
      *(outstr + row * max_cols + col) = *string;
      col++;
    }
    if (inc_parsechar_found) {
      *(outstr + row * max_cols + col) = *inc_char_ptr;
      col++;
    }
    string++;
    if (col >= (max_cols - 1))
      next_token = 1;
  }
  *(outstr + row * max_cols + col) = '\0';
  row++;

  if (char_found == 0)
    return 0;

  return row;
}

/*************************************************************************
*
* Name:		convwbl_isascii
*
* Type		int
*
* char		c	I	a character
*
* Description:
*	Returns 1 if c is a ascii-character or digit, else return 0.
*
**************************************************************************/
static int convwbl_isascii(char c)
{
  if (c >= '0' && c <= '9')
    return 1;
  if (c >= 'a' && c <= 'z')
    return 1;
  if (c >= 'A' && c <= 'Z')
    return 1;
  if (c >= '_')
    return 1;
  if (c == '�' || c == '�' || c == '�' || c == '�' || c == '�' || c == '�')
    return 1;

  return 0;
}

/*************************************************************************
*
* Name:		convwbl_get_next_line
*
* Type		int
*
* Type		Parameter	IOGF	Description
* char		*str		I	buffer.
* char		**pos		IO	position in buffer for next line.
* char		*line		O	next line.
* char		size		I	max size of line.
* int		first		I	first call for this buffer.
*
* Description:
*	Get the next line of a buffer.
*
**************************************************************************/
static int convwbl_get_next_line(
    char* str, char** pos, char* line, int size, int first)
{
  char *s, *t;
  int i;

  if (first)
    s = str;
  else
    s = *pos;

  i = 0;
  t = line;
  while (i < size) {
    if (*s == 16 || *s == 10)
      break;
    if (*s == 0)
      break;
    *t = *s;
    i++;
    t++;
    s++;
  }
  *t = *s;
  if (*s == 0)
    return 1;
  t++;
  *t = 0;
  *pos = s + 1;
  return 0;
}

/*************************************************************************
*
* Name:		convwbl_add_item
*
* Type		int
*
* Type		Parameter	IOGF	Description
* convwbl_ctx	convwblctx	I	convwbl context
* char		*item		I	new item
* char		*delim_front	I	front delimiter.
* char		*delim_back	I	back delimiter.
* int		*func()		I	callback function
*
* Description:
*	Insert a new item in the context's item list.
*
**************************************************************************/
static int convwbl_add_item(convwbl_ctx convwblctx, char* item,
    char* delim_front, char* delim_back, pwr_tStatus (*func)())
{
  convwbl_t_item* item_ptr;

  item_ptr = convwblctx->items;
  while (item_ptr->item[0] != 0)
    item_ptr++;
  strcpy(item_ptr->item, item);
  strcpy(item_ptr->delim_front, delim_front);
  strcpy(item_ptr->delim_back, delim_back);
  item_ptr->hit = 0;
  item_ptr->hit_count = 0;
  item_ptr->func = func;
  item_ptr++;
  item_ptr->item[0] = 0;
  return GSX__SUCCESS;
}

/*************************************************************************
*
* Name:		convwbl_create_ctx
*
* Type		int
*
* Type		Parameter	IOGF	Description
* convwbl_ctx	*convwblctx	I	convwbl context
*
* Description:
*	Allocate memory and initiziate a new context.
*
**************************************************************************/
static pwr_tStatus convwbl_create_ctx(convwbl_ctx* convwblctx)
{
  /* Create the context */
  *convwblctx = (convwbl_ctx)calloc(1, sizeof(**convwblctx));
  if (*convwblctx == 0)
    return GSX__NOMEMORY;

  /* Initiziate the item list */
  memcpy((*convwblctx)->items, template_items, sizeof(template_items));

  return GSX__SUCCESS;
}

/*************************************************************************
*
* Name:		convwbl_delete_ctx()
*
* Type		void
*
* Type		Parameter	IOGF	Description
* convwbl_ctx	convwblctx	I	convwbl context.
*
* Description:
*	Delete a context.
*	Free's all allocated memory in the convwbl context.
*
**************************************************************************/

static pwr_tStatus convwbl_delete_ctx(convwbl_ctx convwblctx)
{
  free((char*)convwblctx);
  return GSX__SUCCESS;
}

/*************************************************************************
*
* Name:		convwbl_convert
*
* Type		int
*
* Type		Parameter	IOGF	Description
* char		*str		I	buffer to convert.
* char		*newstr		I	converted code.
* char		*object		I	name of object pointer.
* char		*bufsize	I	size of newstr.
*
* Description:
*	Convert a wb_load text.
*
**************************************************************************/
pwr_tStatus convwbl_convert(char* str, char* newstr, char* object, int bufsize,
    char* error_line, int* error_line_size, int* error_line_num, int* outsize)
{
  char line[200];
  char* p;
  convwbl_t_item* item_ptr;
  pwr_tStatus sts;
  char* t;
  char* delim_p;
  int hit, delim_hit;
  char tmpstr[160];
  char* write_from;
  int first_line;
  int end_of_text;
  int len;
  convwbl_ctx convwblctx;
  int pos;
  int incr;
  char* l;

  sts = convwbl_create_ctx(&convwblctx);
  if (EVEN(sts))
    return sts;

  idx_count = 0;
  pos = 0;
  *newstr = 0;
  first_line = 1;
  while (1) {
    end_of_text
        = convwbl_get_next_line(str, &p, line, sizeof(line), first_line);
    convwblctx->line_count++;
    first_line = 0;
    item_ptr = convwblctx->items;
    while (item_ptr->item[0] != 0) {
      item_ptr->hit = 0;
      item_ptr->hit_count = 0;
      item_ptr++;
    }
    hit = 0;

    for (l = line; *l != 0; l++) {
      if (!(*l == ' ' || *l == '	'))
        break;
    }
    if (*l == '!') {
      sprintf(newstr + pos, "%s", line);
      pos += strlen(line);
    } else {
      t = line;
      write_from = t;
      item_ptr = convwblctx->items;
      while (item_ptr->item[0] != 0) {
        if (item_ptr->hit == 0) {
          /* Check front delimiter */
          delim_p = &item_ptr->delim_front[0];
          while (*delim_p != 0) {
            if (*delim_p == 'A' || *delim_p == 'X') {
              item_ptr->hit = 1;
              item_ptr->hit_count = 1;
            }
            delim_p++;
          }
        }
        item_ptr++;
      }

      while (*t != 0) {
        item_ptr = convwblctx->items;
        while (item_ptr->item[0] != 0) {
          if (item_ptr->hit == 0) {
            /* Check front delimiter */
            delim_p = &item_ptr->delim_front[0];
            while (*delim_p != 0) {
              if ((*delim_p == 'A' && !convwbl_isascii(*t)) || (*delim_p == 'X')
                  || (*delim_p == *t && *delim_p != 'A')) {
                item_ptr->hit = 1;
                item_ptr->hit_count = 1;
              }
              delim_p++;
            }
          } else {
            if (item_ptr->hit_count >= 1
                && item_ptr->hit_count < strlen(item_ptr->item) + 1) {
              if (item_ptr->item[item_ptr->hit_count - 1] == *t) {
                /* Still hit */
                item_ptr->hit_count++;
              } else {
                delim_hit = 0;
                if (item_ptr->hit_count == 1) {
                  /* More delimiter */
                  delim_p = &item_ptr->delim_front[0];
                  while (*delim_p != 0) {
                    if ((*delim_p == 'A' && !convwbl_isascii(*t))
                        || (*delim_p == 'X'))
                      delim_hit = 1;
                    else if (*delim_p == *t)
                      delim_hit = 1;
                    delim_p++;
                  }
                }
                if (!delim_hit) {
                  item_ptr->hit = 0;
                  item_ptr->hit_count = 0;
                }
              }
            } else if (item_ptr->hit_count == strlen(item_ptr->item) + 1) {
              delim_p = &item_ptr->delim_back[0];
              while (*delim_p != 0) {
                if ((*delim_p == 'A' && !convwbl_isascii(*t))
                    || (*delim_p == 'X')) {
                  hit = 1;
                } else if (*delim_p == *t) {
                  hit = 1;
                }
                delim_p++;
              }
              if (hit)
                break;

              item_ptr->hit = 0;
              item_ptr->hit_count = 0;
            }
          }
          item_ptr++;
        }
        if (hit) {
          if (convwblctx->comment && (item_ptr->func != &convwbl_comment_end)) {
            /* Reset all except the comment_end (first item)*/
            hit = 0;
            item_ptr = convwblctx->items;
            item_ptr++;
            while (item_ptr->item[0] != 0) {
              item_ptr->hit = 0;
              item_ptr->hit_count = 0;
              item_ptr++;
            }
          } else {
            /* Write line */
            strcpy(tmpstr, write_from);
            tmpstr[t - write_from - strlen(item_ptr->item)] = 0;
            strcpy(newstr + pos, tmpstr);
            pos += strlen(tmpstr);
            sts = (item_ptr->func)(
                convwblctx, line, t, newstr + pos, &incr, object);
            if (EVEN(sts)) {
              strncpy(error_line, line, *error_line_size);
              *error_line_num = convwblctx->line_count,
              convwbl_delete_ctx(convwblctx);
              return sts;
            }
            pos += incr;
            write_from = t;
            item_ptr = convwblctx->items;
            while (item_ptr->item[0] != 0) {
              item_ptr->hit = 0;
              item_ptr->hit_count = 0;
              item_ptr++;
            }
            hit = 0;
          }
        } else
          t++;
      }
      strcpy(newstr + pos, write_from);
      pos += strlen(write_from);
      newstr[pos] = 0;
      if (pos > bufsize - 100) {
        convwbl_delete_ctx(convwblctx);
        return GSX__CONVWBLBUFSIZE;
      }
    }
    if (end_of_text)
      break;
  }

  *outsize = pos;
  convwbl_delete_ctx(convwblctx);
  return GSX__SUCCESS;
}

main(int argc, char* argv[])
{
  int sts;
  char* ssts;
  int size;
  char object_var[] = "Z800022";
  char newstr[1000000];
  char str[1000000];
  FILE* infile;
  FILE* outfile;
  char error_line[80];
  int error_num;
  char filename[80];
  int pos;
  char line[400];
  int arg_classdef_idx;
  int outsize;
  FILE* idxfile;

  if (argc >= 2)
    strcpy(filename, argv[1]);
  else {
    printf("\n");
    printf("Usage:\n");
    printf("	Argument 1: filename of wb_load-file.\n");
    printf("	Argument 2: Start index for $ClassDef objekts or\n");
    printf("	            if \"storedidx\" fetch from classdef_idx.dat\n");
    printf("	Argument 3: if \"replace\" old index will be replaced\n");
    printf("\n");
    exit(0);
  }

  if (argc >= 3) {
    if (!strcmp(argv[2], "storedidx")) {
      idxfile = fopen("classdef_idx.dat", "r");
      if (idxfile == 0) {
        printf("-- Index file not found\n");
        arg_classdef_idx = 1;
      }
      fscanf(idxfile, "%d", &arg_classdef_idx);
      fclose(idxfile);
    } else if (sscanf(argv[2], "%d", &arg_classdef_idx) != 1) {
      printf("** WbloadConvert Error, Syntax error in argument 2\n");
      exit(0);
    }
  } else
    arg_classdef_idx = 1;
  classdef_idx = arg_classdef_idx;

  noclassdef = 0;
  replace_idx = 1;
  if (argc >= 4) {
    if (!strcmp(argv[3], "noreplace"))
      replace_idx = 0;
    else if (!strcmp(argv[3], "NOREPLACE"))
      replace_idx = 0;
    else if (!strcmp(argv[3], "noclassdef"))
      noclassdef = 1;
    else if (!strcmp(argv[3], "NOCLASSDEF"))
      noclassdef = 1;
    else {
      printf("** WbloadConvert Error, Syntax error in argument 3\n");
      exit(0);
    }
  }

  printf("-- Processing %s	 Startindex $ClassDef: %d\n", filename,
      arg_classdef_idx);

  str[0] = 0;
  pos = 0;
  infile = fopen(filename, "r");
  ssts = fgets(line, sizeof(line), infile);
  while (ssts != NULL) {
    strcpy(str + pos, line);
    pos += strlen(line);
    ssts = fgets(line, sizeof(line), infile);
  }
  fclose(infile);

  size = sizeof(error_line);
  sts = convwbl_convert(str, newstr, object_var, sizeof(newstr), error_line,
      &size, &error_num, &outsize);
  if (EVEN(sts)) {
    printf("Error in line %d,\n  %s\n", error_num, error_line);
    printf("sts : %d\n", sts);
    exit(0);
  }
  if (idx_count) {
    outfile = fopen(filename, "w");
    fwrite(newstr, outsize, 1, outfile);
    fgetname(outfile, filename);
    fclose(outfile);
    printf("-- File %s created\n	Endindex $ClassDef %d, %d changes\n",
        filename, classdef_idx, idx_count);
  } else
    printf("-- No changes in file '%s'\n", filename);

  idxfile = fopen("classdef_idx.dat", "r+");
  if (!idxfile) {
    idxfile = fopen("classdef_idx.dat", "w");
    if (!idxfile) {
      printf("** Unable to store index i indexfile\n");
      exit(0);
    }
  }
  fprintf(idxfile, "%d", classdef_idx + 1);
  fclose(idxfile);
  exit(sts);
}
