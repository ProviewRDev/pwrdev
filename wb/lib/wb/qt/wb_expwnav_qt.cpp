/**
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

/* wb_expwnav_qt.cpp -- Backupfile display window */

#include "cow_qt_helpers.h"

#include "flow_browwidget_qt.h"

#include "wb_expwnav_qt.h"

WbExpWNavQt::WbExpWNavQt(void* l_parent_ctx, QWidget* l_parent_wid,
    ldh_tSession l_ldhses, int l_type, int l_editmode, QWidget** w)
    : WbExpWNav(l_parent_ctx, l_ldhses, l_type, l_editmode)
{
  debug_print("creating a scrolledbrowwidgetqt\n");
  form_widget
      = scrolledbrowwidgetqt_new(WbExpWNav::init_brow_cb, this, &brow_widget);

  showNow(brow_widget);

  // Create the root item
  *w = form_widget;
}

WbExpWNavQt::~WbExpWNavQt()
{
  debug_print("WbExpWNavQt::~WbExpWNavQt\n");
  delete brow;
  form_widget->close();
}

void WbExpWNavQt::set_input_focus()
{
  brow_widget->setFocus();
}