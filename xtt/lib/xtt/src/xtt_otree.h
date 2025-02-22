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

#ifndef xtt_otree_h
#define xtt_otree_h

#include "cow_tree.h"

/* xtt_otree.h -- Object tree viewer */

class XttOTree {
protected:
  void* parent_ctx;
  CowTree* cowtree;
  pwr_tStatus (*action_cb)(void*, pwr_tAttrRef*);

public:
  XttOTree(void* xn_parent_ctx, pwr_tAttrRef* xn_itemlist, int xn_item_cnt,
      unsigned int xn_options,
      pwr_tStatus (*xn_action_cb)(void*, pwr_tAttrRef*));
  void pop();
  void (*close_cb)(void*);
  static pwr_tStatus get_object_info(void* ctx, pwr_tAttrRef* aref, char* name,
      int nsize, char* cname, char* descr, int dsize);
  static pwr_tStatus get_node_info(
      void* ctx, char* name, char* descr, int dsize);
  static pwr_tStatus action(void* ctx, pwr_tAttrRef* aref);
  static void close(void* ctx);
  virtual ~XttOTree();
};

#endif
