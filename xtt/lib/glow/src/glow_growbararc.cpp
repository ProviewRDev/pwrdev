/*
 * ProviewR   Open Source Process Control.
 * Copyright (C) 2005-2019 SSAB EMEA AB.
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

#include <math.h>
#include <stdlib.h>

#include <iostream>

#include "co_string.h"
#include "co_time.h"

#include "glow_growbararc.h"
#include "glow_grownode.h"
#include "glow_draw.h"

#if defined OS_OPENBSD
#define nearbyint rint
#endif

GrowBarArc::GrowBarArc(GrowCtx* glow_ctx, const char* name, double x1,
    double y1, double x2, double y2, int ang1, int ang2, double width,
    int border, glow_eDrawType border_d_type, int line_w, 
    glow_eDrawType fill_d_type, int nodraw)
    : GrowArc(glow_ctx, name, x1, y1, x2, y2, ang1, ang2, border_d_type, line_w,
      1, border, 0, fill_d_type, 1), max_value(100), min_value(0), bar_width(width), bar_value(35),
      bar_drawtype(glow_eDrawType_Color147),
      bar_bordercolor(glow_eDrawType_Color145), bar_borderwidth(1), bar_direction(0)
{
  if (!nodraw)
    draw(&ctx->mw, (GlowTransform*)NULL, highlight, hot, NULL, NULL);
}

GrowBarArc::~GrowBarArc()
{
  if (ctx->nodraw)
    return;
  erase(&ctx->mw);
  erase(&ctx->navw);
}

void GrowBarArc::save(std::ofstream& fp, glow_eSaveMode mode)
{
  fp << int(glow_eSave_GrowBarArc) << '\n';
  fp << int(glow_eSave_GrowBarArc_max_value) << FSPACE << max_value << '\n';
  fp << int(glow_eSave_GrowBarArc_min_value) << FSPACE << min_value << '\n';
  fp << int(glow_eSave_GrowBarArc_bar_width) << FSPACE << bar_width << '\n';
  fp << int(glow_eSave_GrowBarArc_bar_direction) << FSPACE << bar_direction << '\n';
  fp << int(glow_eSave_GrowBarArc_bar_value) << FSPACE << bar_value << '\n';
  fp << int(glow_eSave_GrowBarArc_bar_drawtype) << FSPACE << int(bar_drawtype)
     << '\n';
  fp << int(glow_eSave_GrowBarArc_bar_bordercolor) << FSPACE
     << int(bar_bordercolor) << '\n';
  fp << int(glow_eSave_GrowBarArc_bar_borderwidth) << FSPACE << bar_borderwidth
     << '\n';
  fp << int(glow_eSave_GrowBarArc_arc_part) << '\n';
  GrowArc::save(fp, mode);
  if (user_data && ctx->userdata_save_callback) {
    fp << int(glow_eSave_GrowBarArc_userdata_cb) << '\n';
    (ctx->userdata_save_callback)(&fp, this, glow_eUserdataCbType_Node);
  }
  fp << int(glow_eSave_End) << '\n';
}

void GrowBarArc::open(std::ifstream& fp)
{
  int type = 0;
  int end_found = 0;
  char dummy[40];
  int tmp;

  for (;;) {
    if (!fp.good()) {
      fp.clear();
      fp.getline(dummy, sizeof(dummy));
      printf("** Read error GrowBarArc: \"%d %s\"\n", type, dummy);
    }

    fp >> type;
    switch (type) {
    case glow_eSave_GrowBarArc:
      break;
    case glow_eSave_GrowBarArc_max_value:
      fp >> max_value;
      break;
    case glow_eSave_GrowBarArc_min_value:
      fp >> min_value;
      break;
    case glow_eSave_GrowBarArc_bar_width:
      fp >> bar_width;
      break;
    case glow_eSave_GrowBarArc_bar_direction:
      fp >> bar_direction;
      break;
    case glow_eSave_GrowBarArc_bar_value:
      fp >> bar_value;
      break;
    case glow_eSave_GrowBarArc_bar_bordercolor:
      fp >> tmp;
      bar_bordercolor = (glow_eDrawType)tmp;
      break;
    case glow_eSave_GrowBarArc_bar_borderwidth:
      fp >> bar_borderwidth;
      break;
    case glow_eSave_GrowBarArc_bar_drawtype:
      fp >> tmp;
      bar_drawtype = (glow_eDrawType)tmp;
      break;
    case glow_eSave_GrowBarArc_arc_part:
      GrowArc::open(fp);
      break;
    case glow_eSave_GrowBarArc_userdata_cb:
      if (ctx->userdata_open_callback)
        (ctx->userdata_open_callback)(&fp, this, glow_eUserdataCbType_Node);
      break;
    case glow_eSave_End:
      end_found = 1;
      break;
    default:
      std::cout << "GrowBarArc:open syntax error\n";
      fp.getline(dummy, sizeof(dummy));
    }
    if (end_found)
      break;
  }
}

void GrowBarArc::draw(GlowWind* w, int ll_x, int ll_y, int ur_x, int ur_y)
{
  int tmp;

  if (ll_x > ur_x) {
    /* Shift */
    tmp = ll_x;
    ll_x = ur_x;
    ur_x = tmp;
  }
  if (ll_y > ur_y) {
    /* Shift */
    tmp = ll_y;
    ll_y = ur_y;
    ur_y = tmp;
  }

  if (x_right * w->zoom_factor_x - w->offset_x >= ll_x
      && x_left * w->zoom_factor_x - w->offset_x <= ur_x
      && y_high * w->zoom_factor_y - w->offset_y >= ll_y
      && y_low * w->zoom_factor_y - w->offset_y <= ur_y) {
    draw(w, (GlowTransform*)NULL, highlight, hot, NULL, NULL);
  }
}

void GrowBarArc::draw(GlowWind* w, int* ll_x, int* ll_y, int* ur_x, int* ur_y)
{
  int tmp;
  int obj_ur_x = int(x_right * w->zoom_factor_x) - w->offset_x;
  int obj_ll_x = int(x_left * w->zoom_factor_x) - w->offset_x;
  int obj_ur_y = int(y_high * w->zoom_factor_y) - w->offset_y;
  int obj_ll_y = int(y_low * w->zoom_factor_y) - w->offset_y;

  if (*ll_x > *ur_x) {
    /* Shift */
    tmp = *ll_x;
    *ll_x = *ur_x;
    *ur_x = tmp;
  }
  if (*ll_y > *ur_y) {
    /* Shift */
    tmp = *ll_y;
    *ll_y = *ur_y;
    *ur_y = tmp;
  }

  if (obj_ur_x >= *ll_x && obj_ll_x <= *ur_x && obj_ur_y >= *ll_y
      && obj_ll_y <= *ur_y) {
    draw(w, (GlowTransform*)NULL, highlight, hot, NULL, NULL);

    // Increase the redraw area
    if (obj_ur_x > *ur_x)
      *ur_x = obj_ur_x;
    if (obj_ur_y > *ur_y)
      *ur_y = obj_ur_y;
    if (obj_ll_x < *ll_x)
      *ll_x = obj_ll_x;
    if (obj_ll_y < *ll_y)
      *ll_y = obj_ll_y;
  }
}

void GrowBarArc::set_highlight(int on)
{
  highlight = on;
  draw();
}

void GrowBarArc::draw(GlowWind* w, GlowTransform* t, int highlight, int hot,
    void* node, void* colornode)
{
  if (ctx->nodraw)
    return;
  if (w == &ctx->navw) {
    if (ctx->no_nav)
      return;
    hot = 0;
  }
  int idx;
  double rotation;
  double ang;
  glow_eDrawType drawtype;
  glow_eDrawType bg_drawtype;
  double yscale;
  int width = int(bar_width * w->zoom_factor_x);
  double value = MAX(min_value, MIN(bar_value, max_value));

  if (node && ((GrowNode*)node)->line_width)
    idx = int(
        w->zoom_factor_y / w->base_zoom_factor * ((GrowNode*)node)->line_width
        - 1);
  else
    idx = int(w->zoom_factor_y / w->base_zoom_factor * line_width - 1);
  idx += hot;

  idx = MAX(0, idx);
  idx = MIN(idx, DRAW_TYPE_SIZE - 1);
  int x1, y1, x2, y2, ll_x, ll_y, ur_x, ur_y;

  if (!t) {
    x1 = int(trf.x(ll.x, ll.y) * w->zoom_factor_x) - w->offset_x;
    y1 = int(trf.y(ll.x, ll.y) * w->zoom_factor_y) - w->offset_y;
    x2 = int(trf.x(ur.x, ur.y) * w->zoom_factor_x) - w->offset_x;
    y2 = int(trf.y(ur.x, ur.y) * w->zoom_factor_y) - w->offset_y;
    rotation = (trf.rot() / 360 - floor(trf.rot() / 360)) * 360;
  } else {
    x1 = int(trf.x(t, ll.x, ll.y) * w->zoom_factor_x) - w->offset_x;
    y1 = int(trf.y(t, ll.x, ll.y) * w->zoom_factor_y) - w->offset_y;
    x2 = int(trf.x(t, ur.x, ur.y) * w->zoom_factor_x) - w->offset_x;
    y2 = int(trf.y(t, ur.x, ur.y) * w->zoom_factor_y) - w->offset_y;
    rotation = (trf.rot(t) / 360 - floor(trf.rot(t) / 360)) * 360;
  }

  ll_x = MIN(x1, x2);
  ur_x = MAX(x1, x2);
  ll_y = MIN(y1, y2);
  ur_y = MAX(y1, y2);
  yscale = double(ur_y - ll_y) / (ur_x - ll_x);
  if ( width > ur_x - ll_x)
    width = ur_x - ll_x;
  drawtype = ctx->get_drawtype(fill_drawtype, glow_eDrawType_FillHighlight,
      highlight, (GrowNode*)colornode, 0);

  if ( background_drawtype == glow_eDrawType_No)
    bg_drawtype = ctx->background_color;
  else
    bg_drawtype = background_drawtype;

  // Draw circle background
  ctx->gdraw->fill_arc(w, ll_x, ll_y, ur_x - ll_x, ur_y - ll_y,
		  0, 360, bg_drawtype, 0);

  // Draw bar background
  ctx->gdraw->fill_arc(w, ll_x, ll_y, ur_x - ll_x, ur_y - ll_y,
      angle1 - (int)rotation, angle2, drawtype, 0);

  // Draw bar 
  if ( bar_direction == 0)
    ang = angle1 - (int)rotation;
  else
    ang = angle1 + angle2 * (max_value - value) / (max_value - min_value) - (int)rotation;

  if ( gradient == glow_eGradient_No)
    ctx->gdraw->fill_arc(w, ll_x, ll_y, ur_x - ll_x, ur_y - ll_y,
			 ang, angle2 * (value - min_value) / (max_value - min_value), 
			 bar_drawtype, 0);
  else {
    glow_eDrawType f1, f2;

    if (gradient_contrast >= 0) {
      f2 = GlowColor::shift_drawtype(
          bar_drawtype, -gradient_contrast / 2, 0);
      f1 = GlowColor::shift_drawtype(
          bar_drawtype, int(float(gradient_contrast) / 2 + 0.6), 0);
    } else {
      f2 = GlowColor::shift_drawtype(
          bar_drawtype, -int(float(gradient_contrast) / 2 - 0.6), 0);
      f1 = GlowColor::shift_drawtype(
          bar_drawtype, gradient_contrast / 2, 0);
    }
    ctx->gdraw->gradient_fill_arc(w, ll_x, ll_y, ur_x - ll_x, ur_y - ll_y,
        ang, angle2 * (value - min_value) / (max_value - min_value), 
        bar_drawtype, f1, f2, gradient);
  }

  // Draw inner circle background
  ctx->gdraw->fill_arc(w, ll_x + width, ll_y + yscale * width, 
		       ur_x - ll_x - 2 * width, 
		       ur_y - ll_y - yscale * 2 * width,
		       0, 360, bg_drawtype, 0);

  if ( bar_direction == 0)
    ang = M_PI*(angle1 - rotation + angle2 * (value - min_value) / (max_value - min_value)) / 180;
  else
    ang = M_PI*(angle1 - rotation + angle2 * (max_value - value) / (max_value - min_value)) / 180;

  ctx->gdraw->line(w, (ur_x + ll_x)/2 + (ur_x - ll_x)/2 * cos(ang), 
		   (ur_y + ll_y)/2 - (ur_y - ll_y)/2 * sin(ang), 
		   (ur_x + ll_x)/2 + ((ur_x - ll_x)/2 - width) * cos(ang), 
		   (ur_y + ll_y)/2 - yscale * ((ur_x - ll_x)/2 - width) * sin(ang), 
		   bar_bordercolor, bar_borderwidth, 0);

  if (border) {
    glow_eDrawType bordercolor = ctx->get_drawtype(draw_type, glow_eDrawType_LineHighlight,
        highlight, (GrowNode*)colornode, 0);

    ctx->gdraw->arc(w, ll_x, ll_y, ur_x - ll_x, ur_y - ll_y,
		    angle1 - (int)rotation, angle2, 
		    bordercolor, idx, 0);

    ctx->gdraw->arc(w, ll_x + width, ll_y + yscale * width, ur_x - ll_x - 2 * width, 
		    ur_y - ll_y - yscale * 2 * width,
		    angle1 - (int)rotation, angle2, 
		    bordercolor, idx, 0);

    ang = M_PI*(angle1 - rotation) / 180;
    ctx->gdraw->line(w, (ur_x + ll_x)/2 + (ur_x - ll_x)/2 * cos(ang), 
		     (ur_y + ll_y)/2 - (ur_y - ll_y)/2 * sin(ang), 
		     (ur_x + ll_x)/2 + ((ur_x - ll_x)/2 - width) * cos(ang), 
		     (ur_y + ll_y)/2 - yscale * ((ur_x - ll_x)/2 - width) * sin(ang), 
		     bordercolor, idx, 0);

    ang = M_PI*(angle1 + angle2 - rotation) / 180;
    ctx->gdraw->line(w, (ur_x + ll_x)/2 + (ur_x - ll_x)/2 * cos(ang), 
		     (ur_y + ll_y)/2 - (ur_y - ll_y)/2 * sin(ang), 
		     (ur_x + ll_x)/2 + ((ur_x - ll_x)/2 - width) * cos(ang), 
		     (ur_y + ll_y)/2 - yscale * ((ur_x - ll_x)/2 - width) * sin(ang), 
		     bordercolor, idx, 0);

  }
}

void GrowBarArc::erase(GlowWind* w, GlowTransform* t, int hot, void* node)
{
  if (ctx->nodraw)
    return;
  if (w == &ctx->navw) {
    if (ctx->no_nav)
      return;
    hot = 0;
  }
  int x1, y1, x2, y2, ll_x, ll_y, ur_x, ur_y;
  double rotation;
  int idx;
  if (hot && ctx->environment != glow_eEnv_Development
      && ctx->hot_indication != glow_eHotIndication_LineWidth)
    hot = 0;

  if (node && ((GrowNode*)node)->line_width)
    idx = int(
        w->zoom_factor_y / w->base_zoom_factor * ((GrowNode*)node)->line_width
        - 1);
  else
    idx = int(w->zoom_factor_y / w->base_zoom_factor * line_width - 1);
  idx += hot;
  idx = MAX(0, idx);
  idx = MIN(idx, DRAW_TYPE_SIZE - 1);

  if (!t) {
    x1 = int(trf.x(ll.x, ll.y) * w->zoom_factor_x) - w->offset_x;
    y1 = int(trf.y(ll.x, ll.y) * w->zoom_factor_y) - w->offset_y;
    x2 = int(trf.x(ur.x, ur.y) * w->zoom_factor_x) - w->offset_x;
    y2 = int(trf.y(ur.x, ur.y) * w->zoom_factor_y) - w->offset_y;
    rotation = int(trf.rot());
  } else {
    x1 = int(trf.x(t, ll.x, ll.y) * w->zoom_factor_x) - w->offset_x;
    y1 = int(trf.y(t, ll.x, ll.y) * w->zoom_factor_y) - w->offset_y;
    x2 = int(trf.x(t, ur.x, ur.y) * w->zoom_factor_x) - w->offset_x;
    y2 = int(trf.y(t, ur.x, ur.y) * w->zoom_factor_y) - w->offset_y;
    rotation = int(trf.rot(t));
  }

  ll_x = MIN(x1, x2);
  ur_x = MAX(x1, x2);
  ll_y = MIN(y1, y2);
  ur_y = MAX(y1, y2);

  w->set_draw_buffer_only();
  ctx->gdraw->arc_erase(w, ll_x, ll_y, ur_x - ll_x, ur_y - ll_y,
      angle1 - (int)rotation, angle2, idx);
  ctx->gdraw->fill_rect(
      w, ll_x, ll_y, ur_x - ll_x, ur_y - ll_y, glow_eDrawType_LineErase);
  w->reset_draw_buffer_only();
}

void GrowBarArc::draw()
{
  ctx->draw(&ctx->mw,
      x_left * ctx->mw.zoom_factor_x - ctx->mw.offset_x - DRAW_MP,
      y_low * ctx->mw.zoom_factor_y - ctx->mw.offset_y - DRAW_MP,
      x_right * ctx->mw.zoom_factor_x - ctx->mw.offset_x + DRAW_MP,
      y_high * ctx->mw.zoom_factor_y - ctx->mw.offset_y + DRAW_MP);
  ctx->draw(&ctx->navw,
      x_left * ctx->navw.zoom_factor_x - ctx->navw.offset_x - 1,
      y_low * ctx->navw.zoom_factor_y - ctx->navw.offset_y - 1,
      x_right * ctx->navw.zoom_factor_x - ctx->navw.offset_x + 1,
      y_high * ctx->navw.zoom_factor_y - ctx->navw.offset_y + 1);
}

void GrowBarArc::align(double x, double y, glow_eAlignDirection direction)
{
  double dx, dy;

  erase(&ctx->mw);
  erase(&ctx->navw);
  ctx->set_defered_redraw();
  draw();
  switch (direction) {
  case glow_eAlignDirection_CenterVert:
    dx = x - (x_right + x_left) / 2;
    dy = 0;
    break;
  case glow_eAlignDirection_CenterHoriz:
    dx = 0;
    dy = y - (y_high + y_low) / 2;
    break;
  case glow_eAlignDirection_CenterCenter:
    dx = x - (x_right + x_left) / 2;
    dy = y - (y_high + y_low) / 2;
    break;
  case glow_eAlignDirection_Right:
    dx = x - x_right;
    dy = 0;
    break;
  case glow_eAlignDirection_Left:
    dx = x - x_left;
    dy = 0;
    break;
  case glow_eAlignDirection_Up:
    dx = 0;
    dy = y - y_high;
    break;
  case glow_eAlignDirection_Down:
    dx = 0;
    dy = y - y_low;
    break;
  }
  trf.move(dx, dy);
  x_right += dx;
  x_left += dx;
  y_high += dy;
  y_low += dy;

  draw();
  ctx->redraw_defered();
}

void GrowBarArc::get_range(double *min, double *max)
{
  *max = max_value;
  *min = min_value;
}

void GrowBarArc::set_range(double min, double max)
{
  max_value = max;
  min_value = min;
  draw();
}

void GrowBarArc::export_javabean(GlowTransform* t, void* node,
    glow_eExportPass pass, int* shape_cnt, int node_cnt, int in_nc,
    std::ofstream& fp)
{
#if 0
  double x1, y1, x2, y2, ll_x, ll_y, ur_x, ur_y;
  double rotation;
  int bold;
  int idx = int(
      ctx->mw.zoom_factor_y / ctx->mw.base_zoom_factor * (text_size + 4) - 4);
  idx = MIN(idx, DRAW_TYPE_SIZE - 1);

  bold = (text_drawtype == glow_eDrawType_TextHelveticaBold);

  if (!t) {
    x1 = trf.x(ll.x, ll.y) * ctx->mw.zoom_factor_x - ctx->mw.offset_x;
    y1 = trf.y(ll.x, ll.y) * ctx->mw.zoom_factor_y - ctx->mw.offset_y;
    x2 = trf.x(ur.x, ur.y) * ctx->mw.zoom_factor_x - ctx->mw.offset_x;
    y2 = trf.y(ur.x, ur.y) * ctx->mw.zoom_factor_y - ctx->mw.offset_y;
  } else {
    x1 = trf.x(t, ll.x, ll.y) * ctx->mw.zoom_factor_x - ctx->mw.offset_x;
    y1 = trf.y(t, ll.x, ll.y) * ctx->mw.zoom_factor_y - ctx->mw.offset_y;
    x2 = trf.x(t, ur.x, ur.y) * ctx->mw.zoom_factor_x - ctx->mw.offset_x;
    y2 = trf.y(t, ur.x, ur.y) * ctx->mw.zoom_factor_y - ctx->mw.offset_y;
  }

  ll_x = MIN(x1, x2);
  ur_x = MAX(x1, x2);
  ll_y = MIN(y1, y2);
  ur_y = MAX(y1, y2);

  if (t)
    rotation = (trf.rot(t) / 360 - floor(trf.rot(t) / 360)) * 360;
  else
    rotation = (trf.rot() / 360 - floor(trf.rot() / 360)) * 360;

  ((GrowCtx*)ctx)
      ->export_jbean->bararc(ll_x, ll_y, ur_x, ur_y, angle1, angle2, draw_type,
          text_color_drawtype, min_value, max_value, lines, longquotient,
          valuequotient, linelength, line_width, rotation, bold, idx, format,
          pass, shape_cnt, node_cnt, fp);
#endif
}

int GrowBarArc::trace_scan()
{
  if (!trace.p)
    return 1;

  if (ctx->trace_scan_func)
    ctx->trace_scan_func((void*)this, trace.p);
  return 1;
}

int GrowBarArc::trace_init()
{
  int sts;

  sts = ctx->trace_connect_func((void*)this, &trace);
  return sts;
}

void GrowBarArc::trace_close()
{
  if (trace.p)
    ctx->trace_disconnect_func((void*)this);
}

void GrowBarArc::set_bar_info(glow_sBarInfo* info)
{
  max_value = info->max_value;
  min_value = info->min_value;
  bar_drawtype = info->bar_drawtype;
  bar_bordercolor = info->bar_bordercolor;
  bar_borderwidth = info->bar_borderwidth;
}

void GrowBarArc::get_bar_info(glow_sBarInfo* info)
{
  info->max_value = max_value;
  info->min_value = min_value;
  info->bar_drawtype = bar_drawtype;
  info->bar_bordercolor = bar_bordercolor;
  info->bar_borderwidth = bar_borderwidth;
}

void GrowBarArc::convert(glow_eConvert version)
{
}