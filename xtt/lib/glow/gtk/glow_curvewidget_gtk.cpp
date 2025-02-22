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

#include "glow_curvewidget_gtk.h"
#include "glow_draw_gtk.h"

typedef struct _CurveWidgetGtk CurveWidgetGtk;
typedef struct _CurveWidgetGtkClass CurveWidgetGtkClass;
typedef struct _CurveWidgetGtkPrivate CurveWidgetGtkPrivate;

typedef struct {
  GtkWidget* curve;
  GtkWidget* form;
  GtkWidget* scroll_h;
  GtkWidget* scroll_v;
  int scroll_h_managed;
  int scroll_v_managed;
} curvewidget_sScroll;

struct _CurveWidgetGtk {
  GtkDrawingArea bin;
  CurveWidgetGtkPrivate *priv;
};

struct _CurveWidgetGtkPrivate {
  GdkWindow *window;
  void* curve_ctx;
  void* draw_ctx;
  int (*init_proc)(GlowCtx* ctx, void* clien_data);
  int is_navigator;
  int is_realized;
  int realize_navigator;
  void* client_data;
  GtkWidget* main_curve_widget;
  GtkWidget* navigator_widget;
  GtkWidget* scroll_h;
  GtkWidget* scroll_v;
  GtkWidget* form;
  int scroll_h_ignore;
  int scroll_v_ignore;
  gdouble scroll_h_value;
  gdouble scroll_v_value;
  int scroll_h_pagesize;
  int scroll_v_pagesize;
  int scroll_h_upper;
  int scroll_v_upper;
  gint scroll_timerid;
  glow_sScroll scroll_data;
  int scroll_configure;
  int destroyed;
  GtkAdjustment* hadjustment;
  GtkAdjustment* vadjustment;
  guint hscroll_policy : 1;
  guint vscroll_policy : 1;
};

enum {
  PROP_0,
  PROP_HADJUSTMENT,
  PROP_VADJUSTMENT,
  PROP_HSCROLL_POLICY,
  PROP_VSCROLL_POLICY
};

struct _CurveWidgetGtkClass {
  GtkDrawingAreaClass parent_class;
};

G_DEFINE_TYPE_WITH_CODE(CurveWidgetGtk, curvewidgetgtk, GTK_TYPE_DRAWING_AREA,
			G_ADD_PRIVATE(CurveWidgetGtk)
			G_IMPLEMENT_INTERFACE(GTK_TYPE_SCROLLABLE, NULL));

static gboolean scroll_callback_cb(void* d);
static void curvewidgetgtk_get_property(GObject *object, guint prop_id, 
				       GValue *value, GParamSpec *pspec);
static void curvewidgetgtk_set_property(GObject *object, guint prop_id, 
				       const GValue *value, GParamSpec *pspec);

static void scroll_callback(glow_sScroll* data)
{
  curvewidget_sScroll* scroll_data = (curvewidget_sScroll*)data->scroll_data;
  CurveWidgetGtkPrivate *curve = (CurveWidgetGtkPrivate *)((CurveWidgetGtk*)scroll_data->curve)->priv;

  if (curve->scroll_timerid)
    g_source_remove(curve->scroll_timerid);

  curve->scroll_timerid = g_timeout_add(200, scroll_callback_cb, scroll_data->curve);
  curve->scroll_data = *data;
}

static gboolean scroll_callback_cb(void* d)
{
  CurveWidgetGtkPrivate *curve = ((CurveWidgetGtk*)d)->priv;
  glow_sScroll* data = &((CurveWidgetGtk*)d)->priv->scroll_data;
  curvewidget_sScroll* scroll_data = (curvewidget_sScroll*)data->scroll_data;
  GtkAdjustment *adj;

  if (data->total_width <= data->window_width) {
    if (data->offset_x == 0)
      data->total_width = data->window_width;
    if (scroll_data->scroll_h_managed) {
      // Remove horizontal scrollbar
    }
  } else {
    if (!scroll_data->scroll_h_managed) {
      // Insert horizontal scrollbar
    }
  }

  if (data->total_height <= data->window_height) {
    if (data->offset_y == 0)
      data->total_height = data->window_height;
    if (scroll_data->scroll_v_managed) {
      // Remove vertical scrollbar
    }
  } else {
    if (!scroll_data->scroll_v_managed) {
      // Insert vertical scrollbar
    }
  }
  if (data->offset_x < 0) {
    data->total_width += -data->offset_x;
    data->offset_x = 0;
  }
  if (data->offset_y < 0) {
    data->total_height += -data->offset_y;
    data->offset_y = 0;
  }
  if (data->total_height < data->window_height + data->offset_y)
    data->total_height = data->window_height + data->offset_y;
  if (data->total_width < data->window_width + data->offset_x)
    data->total_width = data->window_width + data->offset_x;
  if (data->window_width < 1)
    data->window_width = 1;
  if (data->window_height < 1)
    data->window_height = 1;

  if (scroll_data->scroll_v_managed) {
    curve->scroll_v_ignore = 1;
    if (data->window_height != curve->scroll_v_pagesize
        || data->total_height != curve->scroll_v_upper
        || curve->scroll_configure) {
      adj = gtk_range_get_adjustment(GTK_RANGE(scroll_data->scroll_h));
      gtk_adjustment_set_value(adj, data->offset_x);
      gtk_adjustment_set_upper(adj, data->total_width);
      gtk_adjustment_set_page_size(adj, data->window_width);
    } else {
      adj = gtk_range_get_adjustment(GTK_RANGE(scroll_data->scroll_h));
      gtk_adjustment_set_value(adj, data->offset_x);
    }
    curve->scroll_v_value = (gdouble)data->offset_y;
    curve->scroll_h_pagesize = data->window_width;
    curve->scroll_h_upper = data->total_width;
  }
  curve->scroll_configure = 0;
  return FALSE;
}

static void scroll_h_action(GtkWidget* w, gpointer data)
{
  CurveWidgetGtk* curvew = (CurveWidgetGtk*)data;
  if (curvew->priv->scroll_h_ignore) {
    curvew->priv->scroll_h_ignore = 0;
    return;
  }

  CurveCtx* ctx = (CurveCtx*)curvew->priv->curve_ctx;
  gdouble value;
  g_object_get(w, "value", &value, NULL);
  glow_scroll_horizontal(ctx, int(value), 0);
}

static void scroll_v_action(GtkWidget* w, gpointer data)
{
  CurveWidgetGtk* curvew = (CurveWidgetGtk*)data;

  if (curvew->priv->scroll_v_ignore) {
    curvew->priv->scroll_v_ignore = 0;
    return;
  }

  CurveCtx* ctx = (CurveCtx*)curvew->priv->curve_ctx;
  gdouble value;
  g_object_get(w, "value", &value, NULL);
  glow_scroll_vertical(ctx, int(value), 0);
}

static int curve_init_proc(GtkWidget* w, GlowCtx* fctx, void* client_data)
{
  curvewidget_sScroll* scroll_data;
  CurveCtx* ctx;

  ctx = (CurveCtx*)((CurveWidgetGtk*)w)->priv->curve_ctx;

  if (((CurveWidgetGtk*)w)->priv->scroll_h) {
    scroll_data = (curvewidget_sScroll*)malloc(sizeof(curvewidget_sScroll));
    scroll_data->curve = w;
    scroll_data->scroll_h = ((CurveWidgetGtk*)w)->priv->scroll_h;
    scroll_data->scroll_v = ((CurveWidgetGtk*)w)->priv->scroll_v;
    scroll_data->form = ((CurveWidgetGtk*)w)->priv->form;
    scroll_data->scroll_h_managed = 1;
    scroll_data->scroll_v_managed = 1;

    ctx->register_scroll_callback((void*)scroll_data, scroll_callback);
  }
  return (((CurveWidgetGtk*)w)->priv->init_proc)(ctx, client_data);
}

static gboolean curvewidgetgtk_expose(GtkWidget* widget, cairo_t* cr)
{
  CurveWidgetGtk *curve = CURVEWIDGETGTK(widget);

  if (!curve->priv->curve_ctx)
    // Navigator not yet created
    return TRUE;

  ((GlowDrawGtk*)((CurveCtx*)curve->priv->curve_ctx)->gdraw)->
      expose(cr, curve->priv->is_navigator);
  return TRUE;
}

static void curvewidgetgtk_grab_focus(GtkWidget* curve)
{
  if (!((CurveWidgetGtk *)curve)->priv->window)
    return;
  GTK_WIDGET_CLASS(curvewidgetgtk_parent_class)->grab_focus(curve);
  gdk_window_focus(((CurveWidgetGtk *)curve)->priv->window, GDK_CURRENT_TIME);
}

static void curvewidgetgtk_destroy(GtkWidget* widget)
{
  CurveWidgetGtk* curvew = (CurveWidgetGtk*)widget;

  if (!curvew->priv->destroyed) {
    curvew->priv->destroyed = 1;
    if (curvew->priv->scroll_timerid)
      g_source_remove(curvew->priv->scroll_timerid);
    if (curvew->priv->is_navigator) {
      if (curvew->priv->curve_ctx
          && !((CurveWidgetGtk*)curvew->priv->main_curve_widget)->priv->destroyed)
        ((CurveCtx*)curvew->priv->curve_ctx)->no_nav = 1;
    } else
      delete (GlowDrawGtk*)curvew->priv->draw_ctx;
    if (curvew->priv->hadjustment)
      g_object_unref(curvew->priv->hadjustment);
    if (curvew->priv->vadjustment)
      g_object_unref(curvew->priv->vadjustment);
  }
  GTK_WIDGET_CLASS(curvewidgetgtk_parent_class)->destroy(widget);
}

static gboolean curvewidgetgtk_event(GtkWidget* curve, GdkEvent* event)
{
  if (((CurveWidgetGtk*)curve)->priv->destroyed)
    return TRUE;

  if (!((CurveWidgetGtk*)curve)->priv->curve_ctx)
    // Navigator not yet created
    return TRUE;

  if (event->type == GDK_MOTION_NOTIFY) {
    gdk_display_flush(
        ((GlowDrawGtk*)((CurveCtx*)((CurveWidgetGtk*)curve)->priv->curve_ctx)->gdraw)
            ->display);
    GdkEvent* next = gdk_event_peek();
    if (next && next->type == GDK_MOTION_NOTIFY) {
      gdk_event_free(next);
      return TRUE;
    } else if (next)
      gdk_event_free(next);
  } else if (event->type == GDK_CONFIGURE) {
    if (((CurveWidgetGtk*)curve)->priv->scroll_h)
      ((CurveWidgetGtk*)curve)->priv->scroll_configure = 1;
  }

  ((GlowDrawGtk*)((CurveCtx*)((CurveWidgetGtk*)curve)->priv->curve_ctx)->gdraw)
      ->event_handler(*event);
  return TRUE;
}

static void curvewidgetgtk_realize(GtkWidget* widget)
{
  GdkWindowAttr attr;
  gint attr_mask;
  CurveWidgetGtk* curve;
  GtkAllocation allocation;

  g_return_if_fail(widget != NULL);
  g_return_if_fail(IS_CURVEWIDGETGTK(widget));

  curve = CURVEWIDGETGTK(widget);
  gtk_widget_set_realized(widget, TRUE);

  gtk_widget_get_allocation(widget, &allocation);
  attr.x = allocation.x;
  attr.y = allocation.y;
  attr.width = allocation.width;
  attr.height = allocation.height;
  attr.wclass = GDK_INPUT_OUTPUT;
  attr.window_type = GDK_WINDOW_CHILD;
  attr.event_mask = gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK
      | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK
      | GDK_POINTER_MOTION_MASK | GDK_BUTTON_MOTION_MASK
      | GDK_POINTER_MOTION_HINT_MASK | GDK_ENTER_NOTIFY_MASK
      | GDK_LEAVE_NOTIFY_MASK | GDK_STRUCTURE_MASK;
  attr.visual = gtk_widget_get_visual(widget);

  attr_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL;
  curve->priv->window = gdk_window_new(gtk_widget_get_parent_window(widget), &attr, attr_mask);
  gtk_widget_set_window(widget, curve->priv->window);
  gtk_widget_register_window(widget, curve->priv->window);
  //widget->style = gtk_style_attach(widget->style, widget->window);
  //gtk_style_set_background(widget->style, widget->window, GTK_STATE_ACTIVE);

  gtk_widget_set_can_focus(widget, TRUE);

  if (curve->priv->is_navigator) {
    if (!curve->priv->curve_ctx) {
      CurveWidgetGtk* main_curve = (CurveWidgetGtk*)curve->priv->main_curve_widget;

      if (!main_curve->priv->is_realized) {
        main_curve->priv->realize_navigator = 1;
        main_curve->priv->navigator_widget = widget;
      } else {
        curve->priv->curve_ctx = main_curve->priv->curve_ctx;
        curve->priv->draw_ctx = main_curve->priv->draw_ctx;
        ((GlowDrawGtk*)curve->priv->draw_ctx)->init_nav(widget);
      }
    }
  } else {
    if (!curve->priv->curve_ctx) {
      curve->priv->draw_ctx = new GlowDrawGtk(widget, &curve->priv->curve_ctx,
          curve_init_proc, curve->priv->client_data, glow_eCtxType_Curve);
    }
    if (curve->priv->realize_navigator) {
      CurveWidgetGtk* nav_curve = (CurveWidgetGtk*)curve->priv->navigator_widget;
      nav_curve->priv->curve_ctx = curve->priv->curve_ctx;
      nav_curve->priv->draw_ctx = curve->priv->draw_ctx;
      ((GlowDrawGtk*)nav_curve->priv->draw_ctx)->init_nav((GtkWidget*)nav_curve);
    }
  }

  curve->priv->is_realized = 1;
}

static void curvewidgetgtk_class_init(CurveWidgetGtkClass* klass)
{
  GtkWidgetClass* widget_class;
  GObjectClass* gobject_class = G_OBJECT_CLASS(klass);

  widget_class = GTK_WIDGET_CLASS(klass);
  widget_class->realize = curvewidgetgtk_realize;
  widget_class->draw = curvewidgetgtk_expose;
  widget_class->event = curvewidgetgtk_event;
  widget_class->grab_focus = curvewidgetgtk_grab_focus;
  widget_class->destroy = curvewidgetgtk_destroy;
  gobject_class->set_property = curvewidgetgtk_set_property;
  gobject_class->get_property = curvewidgetgtk_get_property;

  // GtkScrollable interface
  g_object_class_override_property(gobject_class, PROP_HADJUSTMENT, "hadjustment");
  g_object_class_override_property(gobject_class, PROP_VADJUSTMENT, "vadjustment");
  g_object_class_override_property(gobject_class, PROP_HSCROLL_POLICY, "hscroll-policy");
  g_object_class_override_property(gobject_class, PROP_VSCROLL_POLICY, "vscroll-policy");
}

static void curvewidgetgtk_init(CurveWidgetGtk* curve)
{
  curve->priv = (CurveWidgetGtkPrivate *)curvewidgetgtk_get_instance_private(curve);
}

GtkWidget* curvewidgetgtk_new(
    int (*init_proc)(GlowCtx* ctx, void* client_data), void* client_data)
{
  CurveWidgetGtk* w;
  w = (CurveWidgetGtk*)g_object_new(CURVEWIDGETGTK_TYPE, NULL);
  w->priv->init_proc = init_proc;
  w->priv->curve_ctx = 0;
  w->priv->is_navigator = 0;
  w->priv->client_data = client_data;
  w->priv->scroll_h = 0;
  w->priv->scroll_v = 0;
  w->priv->hadjustment = NULL;
  w->priv->vadjustment = NULL;
  w->priv->destroyed = 0;
  return (GtkWidget*)w;
}

GtkWidget* scrolledcurvewidgetgtk_new(
    int (*init_proc)(GlowCtx* ctx, void* client_data), void* client_data,
    GtkWidget** curvewidget)
{
  CurveWidgetGtk* w;

  GtkWidget* form = gtk_scrolled_window_new(NULL, NULL);

  w = (CurveWidgetGtk*)g_object_new(CURVEWIDGETGTK_TYPE, NULL);
  w->priv->init_proc = init_proc;
  w->priv->curve_ctx = 0;
  w->priv->is_navigator = 0;
  w->priv->client_data = client_data;
  w->priv->scroll_h = gtk_scrolled_window_get_hscrollbar(GTK_SCROLLED_WINDOW(form));
  w->priv->scroll_v = gtk_scrolled_window_get_vscrollbar(GTK_SCROLLED_WINDOW(form));
  w->priv->scroll_h_ignore = 0;
  w->priv->scroll_v_ignore = 0;
  w->priv->scroll_h_value = 0;
  w->priv->scroll_v_value = 0;
  w->priv->scroll_configure = 0;
  w->priv->destroyed = 0;
  w->priv->form = form;
  w->priv->hadjustment = gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  w->priv->vadjustment = gtk_adjustment_new(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  *curvewidget = GTK_WIDGET(w);

  g_signal_connect(gtk_range_get_adjustment(GTK_RANGE((GtkScrollbar*)w->priv->scroll_h)),
      "value-changed", G_CALLBACK(scroll_h_action), w);
  g_signal_connect(gtk_range_get_adjustment(GTK_RANGE((GtkScrollbar*)w->priv->scroll_v)),
      "value-changed", G_CALLBACK(scroll_v_action), w);

  gtk_container_add(GTK_CONTAINER(form), GTK_WIDGET(w));

  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(form), GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);

  return (GtkWidget*)form;
}

GtkWidget* curvenavwidgetgtk_new(GtkWidget* main_curve)
{
  CurveWidgetGtk* w;
  w = (CurveWidgetGtk*)g_object_new(CURVEWIDGETGTK_TYPE, NULL);
  w->priv->init_proc = 0;
  w->priv->curve_ctx = 0;
  w->priv->is_navigator = 1;
  w->priv->main_curve_widget = main_curve;
  w->priv->client_data = 0;
  w->priv->scroll_h = 0;
  w->priv->scroll_v = 0;
  w->priv->scroll_h_ignore = 0;
  w->priv->scroll_v_ignore = 0;
  w->priv->scroll_h_value = 0;
  w->priv->scroll_v_value = 0;
  w->priv->scroll_configure = 0;
  w->priv->hadjustment = NULL;
  w->priv->vadjustment = NULL;
  w->priv->destroyed = 0;
  return (GtkWidget*)w;
}

static void curvewidgetgtk_set_property(GObject *object, guint prop_id, 
				       const GValue *value, GParamSpec *pspec)
{
  //CurveWidgetGtk *curve = (CurveWidgetGtk *)object;

  switch (prop_id) {
  case PROP_HADJUSTMENT:
    break;
  case PROP_VADJUSTMENT:
    break;
  case PROP_HSCROLL_POLICY:
    break;
  case PROP_VSCROLL_POLICY:
    break;
  }
}

static void curvewidgetgtk_get_property(GObject *object, guint prop_id, 
				       GValue *value, GParamSpec *pspec)
{
  CurveWidgetGtk *curve = (CurveWidgetGtk *)object;

  switch (prop_id) {
  case PROP_HADJUSTMENT:
    g_value_set_object(value, curve->priv->hadjustment);
    break;
  case PROP_VADJUSTMENT:
    g_value_set_object(value, curve->priv->vadjustment);
    break;
  case PROP_HSCROLL_POLICY:
    g_value_set_enum(value, curve->priv->hscroll_policy);
    break;
  case PROP_VSCROLL_POLICY:
    g_value_set_enum(value, curve->priv->vscroll_policy);
    break;
  }
}
