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
 */

#ifndef xtt_xattnav_qt_h
#define xtt_xattnav_qt_h

/* wb_xattnav_qt.h -- */

#ifndef xtt_xattnav_h
#include "xtt_xattnav.h"
#endif

#include <QWidget>

class XAttNavQt : public XAttNav {
public:
  XAttNavQt(void* xa_parent_ctx, QWidget* xa_parent_wid, xattnav_eType xa_type,
      const char* xa_name, pwr_sAttrRef* xa_objar, int xa_advanced_user,
      void* xa_userdata, QWidget** w, pwr_tStatus* status);
  ~XAttNavQt();

  QWidget* brow_widget;
  QWidget* form_widget;

  void popup_position(int x_event, int y_event, int* x, int* y);
  void set_inputfocus();
};

#endif