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

#ifndef ge_attrnav_h
#define ge_attrnav_h

/* xtt_attrnav.h -- Simple navigator */

#include "flow_browapi.h"

#include "glow_growapi.h"

#include "ge_cmn.h"

/*! \file ge_attrnav.h
    \brief Contains the AttrNav class and related classes. */
/*! \addtogroup Ge */
/*@{*/

#define attrnav_cVersion "X3.0b"
#define ATTRNAV_BROW_MAX 25

typedef enum {
  attr_mRefresh_Objects = 1 << 0,
  attr_mRefresh_Select = 1 << 1
} attr_mRefresh;

typedef enum {
  attr_eList_Objects,
  attr_eList_Group,
  attr_eList_Layer,
  attr_eList_Select
} attr_eList;

typedef enum { 
  attr_eType_Attributes, 
  attr_eType_ObjectTree,
  attr_eType_Layers
} attr_eType;

typedef enum {
  attr_eFilterType_No,
  attr_eFilterType_Name,
  attr_eFilterType_Class
} attr_eFilterType;

typedef enum {
  attrnav_eItemType_Local,
  attrnav_eItemType_Enum,
  attrnav_eItemType_Mask,
  attrnav_eItemType_Object
} attrnav_eItemType;

typedef enum {
  attrnav_mOpen_All = ~0,
  attrnav_mOpen_Children = 1 << 0,
  attrnav_mOpen_Attributes = 1 << 1,
  attrnav_mOpen_Crossref = 1 << 2
} attrnav_mOpen;

typedef enum {
  attrnav_eType_GdbNodeFlags = 10000,
  attrnav_eType_Empty = 10001,
  attrnav_eType_FixStr = 10002,
  attrnav_eType_ShortTime = 10003
} attrnav_eType;

typedef struct {
  unsigned int num;
  char name[40];
} attrnav_sEnumElement;

typedef struct {
  unsigned int num;
  attrnav_sEnumElement* elements;
} attrnav_sEnum;

class GeDyn;
class Graph;
class AItemObject;

//! Class for handling of brow.
class AttrNavBrow {
public:
  AttrNavBrow(BrowCtx* brow_ctx, void* xn) : ctx(brow_ctx), attrnav(xn){}
  ~AttrNavBrow();

  BrowCtx* ctx;
  void* attrnav;
  brow_tNodeClass nc_object;
  brow_tNodeClass nc_attr;
  brow_tNodeClass nc_attr_multiline;
  brow_tNodeClass nc_table;
  brow_tNodeClass nc_header;
  brow_tNodeClass nc_table_header;
  brow_tNodeClass nc_enum;
  flow_sAnnotPixmap* pixmap_leaf;
  flow_sAnnotPixmap* pixmap_map;
  flow_sAnnotPixmap* pixmap_openmap;
  flow_sAnnotPixmap* pixmap_attr;
  flow_sAnnotPixmap* pixmap_attrarray;
  flow_sAnnotPixmap* pixmap_openattr;
  flow_sAnnotPixmap* pixmap_view;
  flow_sAnnotPixmap* pixmap_hide;

  void free_pixmaps();
  void allocate_pixmaps();
  void create_nodeclasses();
  void brow_setup();
};

//! The navigation area of the attribute editor.
class AttrNav {
public:
  AttrNav(void* xn_parent_ctx, attr_eType xn_type, const char* xn_name,
      attr_sItem* xn_itemlist, int xn_item_cnt,
      void (*xn_get_object_list_cb)(void*, unsigned int, grow_tObject**, int*, 
      grow_tObject*, int), pwr_tStatus* status);
  virtual ~AttrNav();

  void* parent_ctx;
  attr_eType type;
  char name[80];
  AttrNavBrow* brow;
  attr_sItem* itemlist;
  int item_cnt;
  int trace_started;
  Graph* graph;
  brow_tObject last_selected;
  grow_tObject last_selected_id;
  attr_eFilterType filter_type;
  char filter_pattern[80];
  int layer_highlighted;
  void (*message_cb)(void*, int, char, const char*);
  void (*change_value_cb)(void*);
  int (*get_subgraph_info_cb)(void*, char*, attr_sItem**, int*);
  int (*get_dyn_info_cb)(void*, GeDyn*, attr_sItem**, int*);
  int (*reconfigure_attr_cb)(void*);
  int (*get_plant_select_cb)(void*, char*, int);
  int (*get_current_colors_cb)(
      void*, glow_eDrawType*, glow_eDrawType*, glow_eDrawType*);
  int (*get_current_color_tone_cb)(void*, glow_eDrawType*);
  void (*get_object_list_cb)(
      void*, unsigned int type, grow_tObject**, int*, grow_tObject*, int);
  int (*set_inputfocus_cb)(void*);
  int (*traverse_inputfocus_cb)(void*);

  int set_attr_value(char* value_str, grow_tObject* id, void** client_data);
  int check_attr_value(int* multiline, int* size, char** value);
  void message(int popup, char sev, const char* text);
  void force_trace_scan();
  int object_attr();
  int object_tree();
  void refresh_objects(unsigned int rtype);
  void object_open_check(
      AItemObject* item, grow_tObject* open_list, int* open_type, int open_cnt);
  brow_tObject gobject_to_bobject(grow_tObject gobject);
  void filter(int type, char* pattern);
  void find_object(char* object);
  void clear();
  virtual void set_inputfocus()
  {
  }
  virtual void trace_start()
  {
  }
  static int init_brow_cb(FlowCtx* fctx, void* client_data);
  static int string_to_mask(int type_id, char* str, pwr_tMask* mask);
  static int string_to_enum(int type_id, char* str, pwr_tEnum* enumval);
  static int enum_to_string(
      int type_id, pwr_tEnum enumval, char* str, int strsize);
  static void object_type_to_str(
      glow_eObjectType object_type, char* object_type_str);
};

//! Item for a normal attribute.
class AItemLocal {
public:
  AItemLocal(AttrNav* attrnav, const char* item_name, const char* attr,
      int attr_type, int attr_size, double attr_min_limit,
      double attr_max_limit, void* attr_value_p, int attr_multiline,
      int attr_noedit, int attr_mask,
      int (*attr_input_validation_cb)(void* ctx, void* value),
      void* attr_validation_ctx, brow_tNode dest, flow_eDest dest_code);
  attrnav_eItemType type;
  brow_tNode node;
  char name[120];
  void* value_p;
  char old_value[80];
  int first_scan;
  int type_id;
  int size;
  double min_limit;
  double max_limit;
  int multiline;
  int noedit;
  int mask;
  int (*input_validation_cb)(void* ctx, void* value);
  void* validation_ctx;
  int parent;
  int subgraph;

  int open_children(AttrNav* attrnav, double x, double y);
  int close(AttrNav* attrnav, double x, double y);
};

//! Item for an enum attribute.
class AItemEnum {
public:
  AItemEnum(AttrNav* attrnav, char* item_name, int item_num, int item_type_id,
      void* attr_value_p, brow_tNode dest, flow_eDest dest_code);
  attrnav_eItemType type;
  brow_tNode node;
  char name[40];
  int num;
  int type_id;
  void* value_p;
  int old_value;
  int first_scan;
};

//! Item for a mask attribute.
class AItemMask {
public:
  AItemMask(AttrNav* attrnav, char* item_name, unsigned int item_mask,
      int item_type_id, void* attr_value_p, brow_tNode dest,
      flow_eDest dest_code);
  attrnav_eItemType type;
  brow_tNode node;
  char name[40];
  unsigned int mask;
  int type_id;
  void* value_p;
  int old_value;
  int first_scan;
};

//! Item for an object.
class AItemObject {
public:
  AItemObject(AttrNav* attrnav, char* item_name,
      glow_eObjectType item_object_type, grow_tObject item_id,
      char* item_subgraph, brow_tNode dest, flow_eDest dest_code);
  attrnav_eItemType type;
  brow_tNode node;
  char name[80];
  char subgraph[80];
  glow_eObjectType object_type;
  grow_tObject id;
  void* attr_client_data;

  int open_children(AttrNav* attrnav, double x, double y);
  int open_attributes(AttrNav* attrnav, double x, double y);
  int close(AttrNav* attrnav, double x, double y);
  int is_layer();
};

/*@}*/
#endif
