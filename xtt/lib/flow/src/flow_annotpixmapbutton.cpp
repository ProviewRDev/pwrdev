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

#include "flow_annotpixmapbutton.h"
#include "flow_node.h"
#include "flow_draw.h"
#include "flow_browctx.h"
#include "flow_msg.h"

int FlowAnnotPixmapButton::event_handler(
    void* pos, flow_eEvent event, int x, int y, void* node)
{
  if (!((FlowNode*)node)->annotpixmapv[number])
    return 0;

  int sts;
  int idx
      = int(ctx->zoom_factor / ctx->base_zoom_factor * (pixmap_size + 4) - 4);
  if (idx < 0)
    return 0;
  idx = MIN(idx, DRAW_TYPE_SIZE - 1);

  if (p.z_x + ((FlowPoint*)pos)->z_x - ctx->offset_x < x
      && x < p.z_x + ((FlowPoint*)pos)->z_x - ctx->offset_x
              + ((FlowNode*)node)->annotpixmapv[number]->pixmap_data[idx].width
      && p.z_y + ((FlowPoint*)pos)->z_y - ctx->offset_y < y
      && y < p.z_y + ((FlowPoint*)pos)->z_y - ctx->offset_y
              + ((FlowNode*)node)
                    ->annotpixmapv[number]
                    ->pixmap_data[idx]
                    .height) {
    switch (event) {
    case flow_eEvent_MB1Click:
      //        std::cout << "Event handler: Hit in pixmapbutton\n";
      // Call backcall function
      sts = ctx->pixmapbutton_cb((FlowArrayElem*)node, number);
      if (sts == FLOW__DESTROYED)
        return sts;
      return FLOW__NO_PROPAGATE;
    default:;
    }
    return 1;
  } else
    return 0;
}
