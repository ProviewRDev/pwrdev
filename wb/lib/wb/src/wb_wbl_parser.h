/* 
 * Proview   Open Source Process Control.
 * Copyright (C) 2005-2017 SSAB EMEA AB.
 *
 * This file is part of Proview.
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
 * along with Proview. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking Proview statically or dynamically with other modules is
 * making a combined work based on Proview. Thus, the terms and 
 * conditions of the GNU General Public License cover the whole 
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * Proview give you permission to, from the build function in the
 * Proview Configurator, combine Proview with modules generated by the
 * Proview PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every 
 * copy of the combined work is accompanied by a complete copy of 
 * the source code of Proview (the version used to produce the 
 * combined work), being distributed under the terms of the GNU 
 * General Public License plus this exception.
 **/

#ifndef wb_wbl_parser_h
#define wb_wbl_parser_h

#include <stdlib.h>
#include <string.h>
#include "wb_error.h"

typedef enum {
  wbl_mState_InDocBlock 	= 1 << 0,
  wbl_mState_InVolume		= 1 << 1,
  wbl_mState_InSObject		= 1 << 2,
  wbl_mState_InObject		= 1 << 3,
  wbl_mState_InBody		= 1 << 4,
  wbl_mState_InBuffer		= 1 << 5
} wbl_mState;

typedef enum {
  wbl_eToken_DocBlock,
  wbl_eToken_Volume,
  wbl_eToken_SObject,
  wbl_eToken_Object,
  wbl_eToken_Body,
  wbl_eToken_Attr,
  wbl_eToken_Name,
  wbl_eToken_Value,
  wbl_eToken_Index,
  wbl_eToken_String,
  wbl_eToken_Char,
  wbl_eToken_Operator,
  wbl_eToken_Operator_eq,
  wbl_eToken_Operator_oreq,
  wbl_eToken_Date,
  wbl_eToken_Buffer
} wbl_eToken;

class wbl_ast_node {
 public:
  wbl_ast_node() : size(0), line_number(0), fws(0), fch(0), lch(0), fth(0), text(0) {}
  ~wbl_ast_node() { 
    if ( text) free( text);
    wbl_ast_node *next;
    for ( wbl_ast_node *n = fch; n;) {
      next = n->fws;
      delete n;
      n = next;
    }
  }
  void setType( int type) { token = type;}
  unsigned int getType() { return token;}
  char *getText() { return text;}
  void setText( char *t) {
    free( text);
    text = (char *)malloc( strlen(t)+1);
    strcpy( text, t);
  }
  //wbl_ast_node *getFirstChild() { return fch;}
  //wbl_ast_node *getNextSibling() { return fws;}

  int token;
  int size;
  unsigned int line_number;
  wbl_ast_node *fws;
  wbl_ast_node *fch;
  wbl_ast_node *lch;
  wbl_ast_node *fth;
  char *text;
};

class wb_wbl_parser {
 public:
  wb_wbl_parser();
  ~wb_wbl_parser();
  void parse( const char *filename);
  void print();
  void print_node( wbl_ast_node *n, int level);
  void print_error( wb_error_str& e);
  wbl_ast_node *new_ast_node( wbl_eToken token, char *text, unsigned int len,
			      unsigned int line, int allocated);
  int next_token( ifstream& is, char *line, const char *start_delim, 
		  const char *end_delim, wbl_eToken type, char **start, 
		  unsigned int *len, int *allocated);
  int read_docblock( ifstream& is, char *line,
		     char **start, unsigned int *len);
  void ast_node_insert_lch( wbl_ast_node *fth, wbl_ast_node *n);
  wbl_ast_node *get_root_ast() { return m_tree;}
  unsigned int get_line() { return m_line_cnt;}
  void print_line();
  int check_operator( wbl_ast_node *n);

 private:
  pwr_tFileName m_fname;
  unsigned int m_line_cnt;
  unsigned int m_state;
  wbl_ast_node *m_tree;
  wbl_ast_node *m_current;
  int m_object_level;
  int m_buffer_level;
};



#endif

