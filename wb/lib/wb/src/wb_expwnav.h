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

#ifndef wb_expwnav_h
#define wb_expwnav_h

/* wb_expwnav.h -- Backupfile display window */

#include "flow_browapi.h"
#include "pwr_baseclasses.h"
#include "wb_bck.h"

typedef enum {
  expw_mOpen_All = ~0,
  expw_mOpen_Children = 1 << 0,
} expw_mOpen;

typedef enum {
  expw_eType_Export,
  expw_eType_Import,
  expw_eType_BuildDirectories
} expw_eType;

typedef enum {
  expwitem_eItemType_Exp,
  expwitem_eItemType_Dir
} expwitem_eItemType;

typedef enum {
  expw_eListType_Dir,
  expw_eListType_Copy,
  expw_eListType_Exec,
} expw_eListType;

class ExpWList {
public:
  int update;

  ExpWList() : update(0)
  {
  }
};

class ExpWCopy : public ExpWList {
public:
  ExpWCopy* next;
  ExpWCopy* prev;
  pwr_tFileName source;
  pwr_tFileName target;
  pwr_tFileConvertEnum conversion;

  ExpWCopy() : next(0), prev(0)
  {
  }
};

class ExpWExec : public ExpWList {
public:
  ExpWExec* next;
  ExpWExec* prev;
  pwr_tCmd command;
  pwr_tFileName dir;

  ExpWExec() : next(0), prev(0)
  {
  }
};

class ExpWMake : public ExpWList {
public:
  ExpWMake* next;
  ExpWMake* prev;
  pwr_tFileName makefile;
  pwr_tFileName dir;

  ExpWMake() : next(0), prev(0)
  {
  }
};

class ExpWDir : public ExpWList {
public:
  ExpWDir* next;
  ExpWDir* prev;
  ExpWCopy* copylist;
  ExpWMake* makelist;
  ExpWExec* execlist;
  int open;
  char name[80];
  pwr_tMask options;
  char description[80];

  ExpWDir()
      : next(0), prev(0), copylist(0), makelist(0), execlist(0), open(0),
        options(0)
  {
  }
  ExpWCopy* copy_insert(char* source, char* target, pwr_tFileConvertEnum conversion, 
			int update);
  ExpWMake* make_insert(char* dir, char* makefile, int update);
  ExpWExec* exec_insert(char* dir, char* command, int update);
};

class WbExpWNavBrow {
public:
  WbExpWNavBrow(BrowCtx* brow_ctx, void* lwnav) : ctx(brow_ctx), expwnav(lwnav)
  {
  }
  ~WbExpWNavBrow();

  BrowCtx* ctx;
  void* expwnav;
  brow_tNodeClass nc_exp;
  brow_tNodeClass nc_eexp;
  flow_sAnnotPixmap* pixmap_export;
  flow_sAnnotPixmap* pixmap_map;
  flow_sAnnotPixmap* pixmap_openmap;
  flow_sAnnotPixmap* pixmap_leaf;

  void free_pixmaps();
  void allocate_pixmaps();
  void create_nodeclasses();
  void brow_setup();
};

class WbExpWNav {
public:
  WbExpWNav(
      void* l_parent_ctx, ldh_tSesContext l_ldhses, int l_type, int l_editmode);
  virtual ~WbExpWNav();

  void* parent_ctx;
  ldh_tSesContext ldhses;
  WbExpWNavBrow* brow;
  int type;
  int editmode;
  int show_all;
  ExpWDir* dirlist;

  virtual void set_input_focus()
  {
  }

  void show();
  void update();
  void show_export_import();
  void show_builddir();
  void redraw(int posit_top);
  void clear();
  void zoom(double zoom_factor);
  void unzoom();
  pwr_tStatus exp();
  void check_all();
  void check_clear();
  void check_reset();
  void set_show_all(int set);
  ExpWDir* dir_find(char* name);
  ExpWDir* dir_insert(char* name, char* options_str, char* description);
  void list_free();

  static int init_brow_cb(FlowCtx* fctx, void* client_data);
  static int brow_cb(FlowCtx* ctx, flow_tEvent event);
};

class ItemExp {
public:
  ItemExp(WbExpWNav* expwnav, char* item_source, char* item_target,
      ExpWList* item_listp, brow_tNode dest, flow_eDest dest_code);
  expwitem_eItemType type;
  WbExpWNav* expwnav;
  brow_tNode node;
  pwr_tFileName source;
  pwr_tFileName target;
  int button_value;
  ExpWList* listp;

  void set(int value);
  void update();
  virtual ~ItemExp();
};

class ItemDir {
public:
  ItemDir(WbExpWNav* expwnav, ExpWDir* item_dir, char* item_name,
      char* item_description, brow_tNode dest, flow_eDest dest_code);
  expwitem_eItemType type;
  WbExpWNav* expwnav;
  ExpWDir* dir;
  brow_tNode node;
  char name[80];
  char description[80];
  int button_value;

  int open_children();
  int close();
  void set(int value);
  void update();
  virtual ~ItemDir();
};

#endif
