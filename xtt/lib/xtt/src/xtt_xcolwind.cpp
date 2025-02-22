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

/* xtt_xcolwind.cpp -- Collect window */

#include <stdlib.h>

#include "co_cdh.h"
#include "co_dcli.h"
#include "co_string.h"
#include "co_time.h"

#include "rt_xatt_msg.h"

#include "xtt_xcolwind.h"
#include "xtt_xattnav.h"
#include "xtt_item.h"

void XColWind::message_cb(void* xcolwind, char severity, const char* message)
{
  ((XColWind*)xcolwind)->message(severity, message);
}

void XColWind::change_value_cb(void* xcolwind)
{
  ((XColWind*)xcolwind)->change_value(1);
}

//
//  Callbackfunctions from menu entries
//
void XColWind::activate_open()
{
  pwr_tCmd cmd = "collect open";

  if (command_cb)
    (command_cb)(parent_ctx, cmd);
}

void XColWind::activate_save()
{
  pwr_tFileName fname;
  ItemCollect* item;
  brow_tObject* object_list;
  int object_cnt;
  int i;
  int width, height;
  double scantime, zoomfactor;

  if (streq(filename, ""))
    activate_saveas();

  dcli_translate_filename(fname, filename);
  std::ofstream fp(fname);
  if (!fp) {
    char tmp[280];
    snprintf(tmp, sizeof(tmp), "Unable to open file \"%s\"", fname);
    message('E', tmp);
    return;
  }

  get_window_size(&width, &height);
  scantime = double(xattnav->get_scantime()) / 1000;
  xattnav->get_zoom(&zoomfactor);

  fp << "#\n"
     << "# Xtt collection file\n"
     << "#\n";

  brow_GetObjectList(xattnav->brow->ctx, &object_list, &object_cnt);
  for (i = 0; i < object_cnt; i++) {
    brow_GetUserData(object_list[i], (void**)&item);

    switch (item->type) {
    case xnav_eItemType_Collect:
      if (i == object_cnt - 1) {
        if (i == 0)
          fp << "collect /name=" << item->name << " /newwindow /last"
             << " /width=" << width << " /height=" << height
             << " /zoomfactor=" << zoomfactor << " /scantime=" << scantime
             << " /title=\"" << title << "\"\n";
        else
          fp << "collect /name=" << item->name << " /addwindow /last\n";
      } else {
        if (i == 0)
          fp << "collect /name=" << item->name << " /newwindow"
             << " /width=" << width << " /height=" << height
             << " /zoomfactor=" << zoomfactor << " /scantime=" << scantime
             << " /title=\"" << title << "\"\n";
        else
          fp << "collect /name=" << item->name << " /addwindow\n";
      }
      break;
    default:;
    }
  }

  fp.close();
}

int XColWind::init_cb(void* ctx)
{
  XColWind* xcolwind = (XColWind*)ctx;
  int sts;

  switch (xcolwind->type) {
  case xcolwind_eType_Collect:
    sts = xcolwind->collect_add(xcolwind->objar_list);
    free(xcolwind->objar_list);
    xcolwind->objar_list = 0;
    break;
  case xcolwind_eType_CollectSignals:
  case xcolwind_eType_CollectIOSignals:
    sts = xcolwind->collect_signals(xcolwind->objar_list);
    break;
  default:;
  }
  return sts;
}

void XColWind::file_selected_cb(void* ctx, void* data, char* text)
{
  XColWind* xcolwind = (XColWind*)ctx;

  xcolwind->set_filename(text);
  xcolwind->set_title(text);
  xcolwind->activate_save();
}

void XColWind::activate_saveas()
{
  wow->CreateInputDialog(
      this, "Save as", "Enter filename", file_selected_cb, 0, 40, 0, 0);
}

void XColWind::activate_display_object()
{
  pwr_tAttrRef aref;
  int sts;

  sts = xattnav->get_select(&aref);
  if (EVEN(sts))
    return;

  if (call_method_cb) {
    (call_method_cb)(parent_ctx, "$Object-RtNavigator",
        "$Object-RtNavigatorFilter", aref, xmenu_eItemType_Object,
        xmenu_mUtility_AttrEditor, NULL);
  }
}

void XColWind::activate_show_cross()
{
  pwr_tAttrRef aref;
  int sts;

  sts = xattnav->get_select(&aref);
  if (EVEN(sts))
    return;

  if (call_method_cb) {
    (call_method_cb)(parent_ctx, "$Object-OpenCrossref",
        "$Object-OpenCrossrefFilter", aref, xmenu_eItemType_Object,
        xmenu_mUtility_AttrEditor, NULL);
  }
}

void XColWind::activate_open_classgraph()
{
  pwr_tAttrRef aref;
  int sts;

  sts = xattnav->get_select(&aref);
  if (EVEN(sts))
    return;

  if (call_method_cb) {
    (call_method_cb)(parent_ctx, "$Object-OpenObjectGraph",
        "$Object-OpenObjectGraphFilter", aref, xmenu_eItemType_Object,
        xmenu_mUtility_AttrEditor, NULL);
  }
}

void XColWind::activate_open_plc()
{
  pwr_tAttrRef aref;
  int sts;

  sts = xattnav->get_select(&aref);
  if (EVEN(sts))
    return;

  if (call_method_cb) {
    (call_method_cb)(parent_ctx, "$Object-OpenTrace", "$Object-OpenTraceFilter",
        aref, xmenu_eItemType_Object, xmenu_mUtility_AttrEditor, NULL);
  }
}

void XColWind::activate_collect_insert()
{
  pwr_tAttrRef aref, oaref;
  int is_attr;
  int sts;
  pwr_tOName attr;

  if (get_select_cb) {
    sts = (get_select_cb)(parent_ctx, &aref, &is_attr);
    if (EVEN(sts))
      return;

    if (!aref.Flags.b.Object && !aref.Flags.b.ObjectAttr) {
      collect_insert(&aref);
    } else {
      sts = XNav::get_trace_attr(&aref, attr);
      if (EVEN(sts))
        return;

      sts = gdh_ArefANameToAref(&aref, attr, &oaref);
      if (EVEN(sts))
        return;

      collect_insert(&oaref);
    }
  }
}

void XColWind::activate_collect_delete()
{
  brow_tNode* node_list;
  int node_count;

  brow_GetSelectedNodes(xattnav->brow->ctx, &node_list, &node_count);
  if (node_count > 0)
    brow_DeleteNode(xattnav->brow->ctx, node_list[0]);
}

void XColWind::activate_moveup()
{
  brow_tNode* node_list;
  int node_count;

  brow_GetSelectedNodes(xattnav->brow->ctx, &node_list, &node_count);
  if (node_count == 1) {
    brow_MoveUp(xattnav->brow->ctx, node_list[0]);
    brow_Redraw(xattnav->brow->ctx, 0);
  }
}

void XColWind::activate_movedown()
{
  brow_tNode* node_list;
  int node_count;

  brow_GetSelectedNodes(xattnav->brow->ctx, &node_list, &node_count);
  if (node_count == 1) {
    brow_MoveDown(xattnav->brow->ctx, node_list[0]);
    brow_Redraw(xattnav->brow->ctx, 0);
  }
}

void XColWind::activate_print()
{
  if (!CoWow::PrintDialogIsDisabled())
    print();
}

void XColWind::activate_help()
{
  // Not yet implemented
}

int XColWind::open_changevalue(char* name)
{
  int sts;

  sts = ((XAttNav*)xattnav)->select_by_name(name);
  if (EVEN(sts))
    return sts;

  change_value(0);
  return XATT__SUCCESS;
}

void XColWind::swap(int mode)
{
  xattnav->swap(mode);
}

XColWind::~XColWind()
{
}

XColWind::XColWind(void* xa_parent_ctx, pwr_sAttrRef* xa_objar_list,
    char* xa_title, int xa_advanced_user, xcolwind_eType xa_type, int* xa_sts)
    : parent_ctx(xa_parent_ctx), objar_list(xa_objar_list), input_open(0),
      input_multiline(0), close_cb(0), redraw_cb(0), popup_menu_cb(0),
      call_method_cb(0), command_cb(0), get_select_cb(0), client_data(0),
      type(xa_type)
{
  if (xa_title && !streq(xa_title, "")) {
    strcpy(title, xa_title);
    set_filename(title);
  } else {
    if (type == xcolwind_eType_CollectSignals
        || type == xcolwind_eType_CollectIOSignals) {
      pwr_tAName aname;
      pwr_tStatus sts;

      sts = gdh_AttrrefToName(objar_list, aname, sizeof(aname), cdh_mNName);
      if (EVEN(sts)) {
        *xa_sts = sts;
        return;
      }
      memset(title, 0, sizeof(title));
      strcpy(title, "Signals ");
      strncat(title, aname, sizeof(title) - strlen(title) - 1);
      strcpy(filename, "");
    } else {
      strcpy(title, "Collection");
      strcpy(filename, "");
    }
  }
  *xa_sts = XATT__SUCCESS;
}

void XColWind::set_filename(char* name)
{
  if (!strchr(name, '/')) {
    strcpy(filename, "$pwrp_load/");
    strcat(filename, name);
  } else
    strcpy(filename, name);

  if (!strchr(name, '.'))
    strcat(filename, ".rtt_col");
}

void XColWind::collect_insert(pwr_tAttrRef* aref)
{
  pwr_tAttrRef* areflist = (pwr_tAttrRef*)calloc(2, sizeof(pwr_tAttrRef));
  areflist[0] = *aref;

  collect_add(areflist);
  free(areflist);
}

void XColWind::xcolwind_popup_menu_cb(void* ctx, pwr_sAttrRef attrref,
    unsigned long item_type, unsigned long utility, char* arg, int x, int y)
{
  if (((XColWind*)ctx)->popup_menu_cb)
    (((XColWind*)ctx)->popup_menu_cb)(
        ((XColWind*)ctx)->parent_ctx, attrref, item_type, utility, arg, x, y);
}

int XColWind::xcolwind_is_authorized_cb(void* ctx, unsigned int access)
{
  XColWind* xcolwind = (XColWind*)ctx;

  if (xcolwind->is_authorized_cb)
    return (xcolwind->is_authorized_cb)(xcolwind->parent_ctx, access);
  return 0;
}

void XColWind::activate_zoomin()
{
  double zoom_factor;

  xattnav->get_zoom(&zoom_factor);
  if (zoom_factor > 40)
    return;

  xattnav->zoom(1.18);
}

void XColWind::activate_zoomout()
{
  double zoom_factor;

  xattnav->get_zoom(&zoom_factor);
  if (zoom_factor < 15)
    return;

  xattnav->zoom(1.0 / 1.18);
}

void XColWind::set_scantime(int t)
{
  xattnav->set_scantime(t);
}

void XColWind::zoom(double zoom_factor)
{
  brow_ZoomAbsolute(xattnav->brow->ctx, zoom_factor);
}

int XColWind::collect_add(pwr_tAttrRef* areflist)
{
  ItemCollect* item;
  int sts;
  pwr_tAName attr;
  char* s;
  pwr_tTypeId a_type_id;
  unsigned int a_size;
  unsigned int a_offset;
  unsigned int a_dim;
  unsigned int a_flags = 0;
  pwr_tTid a_tid;
  pwr_tAName name;
  pwr_tAttrRef* arp;

  if (!areflist)
    return XATT__SUCCESS;

  brow_SetNodraw(xattnav->brow->ctx);

  for (arp = areflist; cdh_ObjidIsNotNull(arp->Objid); arp++) {
    sts = gdh_AttrrefToName(arp, name, sizeof(name), cdh_mNName);
    if (EVEN(sts))
      return sts;

    if (!arp->Flags.b.Object && !arp->Flags.b.ObjectAttr) {
      if ((s = strchr(name, '.')) == 0)
        return 0;
      strcpy(attr, s + 1);

      sts = gdh_GetAttributeCharAttrref(
          arp, &a_type_id, &a_size, &a_offset, &a_dim);
      if (EVEN(sts))
        return sts;

      sts = gdh_GetAttributeFlags(arp, &a_flags);
      if (EVEN(sts))
        return sts;

      sts = gdh_GetAttrRefTid(arp, &a_tid);
      if (EVEN(sts))
        return sts;
    } else
      continue;

    item = new ItemCollect(xattnav->brow, arp->Objid, attr, NULL,
        flow_eDest_IntoLast, a_type_id, a_tid, a_size, a_flags, 0);
  }

  brow_ResetNodraw(xattnav->brow->ctx);
  brow_Redraw(xattnav->brow->ctx, 0);
  return XATT__SUCCESS;
}

int XColWind::collect_signals(pwr_tAttrRef* arp)
{
  int sts;
  pwr_tAName name;

  brow_SetNodraw(xattnav->brow->ctx);

  sts = gdh_AttrrefToName(arp, name, sizeof(name), cdh_mNName);
  if (EVEN(sts))
    return sts;

  NavCrr* navcrr = new NavCrr(this, 0);
  navcrr->insert_cb = signal_insert_cb;
  navcrr->name_to_objid_cb = name_to_objid_cb;
  navcrr->get_volume_cb = 0;

  sts = navcrr->get_signals(0, name);

  delete navcrr;

  brow_ResetNodraw(xattnav->brow->ctx);
  brow_Redraw(xattnav->brow->ctx, 0);
  return XATT__SUCCESS;
}

int XColWind::name_to_objid_cb(void* ctx, char* name, pwr_tObjid* objid)
{
  return gdh_NameToObjid(name, objid);
}

void XColWind::signal_insert_cb(void* ctx, void* parent_node,
    navc_eItemType item_type, char* text1, char* text2, int write)
{
  XColWind* xcolwind = (XColWind*)ctx;
  XNavBrow* brow = xcolwind->xattnav->brow;
  pwr_tAttrRef aref;
  pwr_tTypeId a_type_id;
  unsigned int a_size;
  unsigned int a_offset;
  unsigned int a_dim;
  pwr_tTid a_tid;
  unsigned int a_flags = 0;
  pwr_sAttrRef ar;
  pwr_tStatus sts;
  ItemCollect* item;
  char* s;
  pwr_tOName attr, trace_attr;
  pwr_tAName name;
  pwr_tCid cid;

  sts = gdh_NameToAttrref(pwr_cNObjid, text1, &aref);
  if (EVEN(sts))
    return;

  if (xcolwind->type == xcolwind_eType_CollectIOSignals) {
    sts = gdh_GetAttrRefTid(&aref, &cid);
    if (EVEN(sts))
      return;

    switch (cid) {
    case pwr_cClass_Di:
    case pwr_cClass_Do:
    case pwr_cClass_Ai:
    case pwr_cClass_Ao:
    case pwr_cClass_Ii:
    case pwr_cClass_Io:
    case pwr_cClass_Ei:
    case pwr_cClass_Eo:
    case pwr_cClass_Si:
    case pwr_cClass_So:
    case pwr_cClass_Co:
      break;
    default:
      return;
    }
  }

  sts = XNav::get_trace_attr(&aref, trace_attr);
  if (EVEN(sts))
    return;

  if ((s = strchr(text1, '.'))) {
    strcpy(attr, s + 1);
    strcat(attr, ".");
    strcat(attr, trace_attr);
  } else
    strcpy(attr, trace_attr);

  strcpy(name, text1);
  strcat(name, ".");
  strcat(name, trace_attr);

  sts = gdh_NameToAttrref(pwr_cNObjid, name, &ar);
  if (EVEN(sts))
    return;

  sts = gdh_GetAttributeCharAttrref(
      &ar, &a_type_id, &a_size, &a_offset, &a_dim);
  if (EVEN(sts))
    return;

  sts = gdh_GetAttributeFlags(&ar, &a_flags);
  if (EVEN(sts))
    return;

  sts = gdh_GetAttrRefTid(&ar, &a_tid);
  if (EVEN(sts))
    return;

  item = new ItemCollect(brow, aref.Objid, attr, NULL, flow_eDest_IntoLast,
      a_type_id, a_tid, a_size, a_flags, 0);
}
