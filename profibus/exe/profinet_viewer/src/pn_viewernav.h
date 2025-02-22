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

#ifndef pn_viewernav_h
#define pn_viewernav_h

/* pn_viewernav.h -- Profinet viewer */

#include <vector>

#include "pwr_class.h"

#include "flow_browapi.h"

class PnDevice
{
public:
  PnDevice();
  unsigned char ipaddress[4];
  unsigned char macaddress[6];
  char devname[80];
  int vendorid;
  int deviceid;
  bool hide;
};

class ItemDevice;

typedef enum { viewer_eType_Network, viewer_eType_Configuration } viewer_eType;

typedef enum {
  viewer_eFilterType_No,
  viewer_eFilterType_NotMatching
} viewer_eFilterType;

typedef enum {
  viewitem_eItemType_Device,
  viewitem_eItemType_DeviceAttr
} viewitem_eItemType;

class PnViewerNavBrow
{
public:
  PnViewerNavBrow(BrowCtx* brow_ctx, void* lvnav)
      : ctx(brow_ctx), viewernav(lvnav)
  {
  }
  ~PnViewerNavBrow();

  BrowCtx* ctx;
  void* viewernav;
  brow_tNodeClass nc_device;
  brow_tNodeClass nc_attr;
  flow_sAnnotPixmap* pixmap_map;
  flow_sAnnotPixmap* pixmap_openmap;
  flow_sAnnotPixmap* pixmap_attr;
  flow_sAnnotPixmap* pixmap_edit;

  void free_pixmaps();
  void allocate_pixmaps();
  void create_nodeclasses();
  void brow_setup();
};

class PnViewerNav
{
public:
  PnViewerNav(void* l_parent_ctx, viewer_eType l_type);
  virtual ~PnViewerNav();

  viewer_eType type;
  void* parent_ctx;
  PnViewerNavBrow* brow;
  void (*change_value_cb)(void*);
  void (*message_cb)(void*, int, const char*);

  virtual void set_input_focus() {}

  void message(int severity, const char* msg);
  void zoom(double zoom_factor);
  void unzoom();
  void set(std::vector<PnDevice>& dev_vect);
  int set_attr_value(char* value_str);
  virtual int check_attr_value();
  int get_selected_device(ItemDevice** device);
  void vendorid_to_str(unsigned int vendorid, char* vendorid_str, int size);
  void get_device_info(unsigned int vendorid, unsigned int deviceid, char* info,
                       int info_size, char* family, int family_size);

  static int init_brow_cb(FlowCtx* fctx, void* client_data);
  static int brow_cb(FlowCtx* ctx, flow_tEvent event);
};

class ItemDevice
{
public:
  ItemDevice(PnViewerNav* viewernav, const char* item_name,
             unsigned char* item_ipaddress, unsigned char* item_macaddress,
             char* item_devname, int vendorid, int deviceid, brow_tNode dest,
             flow_eDest dest_code);
  virtual ~ItemDevice();

  viewitem_eItemType type;
  PnViewerNav* viewernav;
  brow_tNode node;
  unsigned char ipaddress[4];
  unsigned char macaddress[8];
  char ipaddress_str[20];
  char macaddress_str[20];
  char devname[80];
  int vendorid;
  int deviceid;
  char vendorid_str[80];
  char deviceid_str[20];
  char infotext[200];
  char family[80];

  int open_children(PnViewerNav* viewernav);
  void close(PnViewerNav* viewernav);
};

class ItemDeviceAttr
{
public:
  ItemDeviceAttr(PnViewerNav* viewernav, const char* item_name,
                 pwr_eType item_attr_type, void* item_p, brow_tNode dest,
                 flow_eDest dest_code);
  virtual ~ItemDeviceAttr();

  viewitem_eItemType type;
  char name[80];
  pwr_eType attr_type;
  PnViewerNav* viewernav;
  brow_tNode node;
  void* p;
};

#endif
