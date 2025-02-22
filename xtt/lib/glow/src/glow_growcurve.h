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

#ifndef glow_growcurve_h
#define glow_growcurve_h

#include "glow_growtrend.h"

/*! \file glow_growcurve.h
    \brief Contains the GrowCurve class. */
/*! \addtogroup Glow */
/*@{*/

//! Class for drawing curves in a trend window.
/*! A GrowTrend object is an object that draws a number of trend curves, where
  the x-axis is the
  time dimension, and the y-axis displayes the trend value of a parameter. New
  values are added as time
  passes. The curves are drawn filled or unfilled.
*/

class GrowCurve : public GrowTrend {
public:
  //! Constuctor
  /*!
    \param glow_ctx 	The glow context.
    \param data		Initial data for the curves.
    \param name		Name (max 31 char).
    \param x		x coordinate for position.
    \param y		y coordinate for position.
    \param w		Width.
    \param h		Height.
    \param border_d_type Border color.
    \param line_w	Linewidth of border.
    \param display_lev	Displaylevel when this object is visible.
    \param fill_rect	Rectangle is filled.
    \param display_border Border is visible.
    \param fill_d_type	Fill color.
    \param nodraw	Don't draw the object now.
  */
  GrowCurve(GrowCtx* glow_ctx, const char* name, glow_sCurveData* data,
      double x = 0, double y = 0, double w = 0, double h = 0,
      glow_eDrawType border_d_type = glow_eDrawType_Line, int line_w = 1,
      glow_mDisplayLevel display_lev = glow_mDisplayLevel_1, int fill_rect = 0,
      int display_border = 1, glow_eDrawType fill_d_type = glow_eDrawType_Line,
      int nodraw = 0);
  virtual ~GrowCurve();

  glow_eCurveType type; //!< Type of curve.
  int split_digsquare; //!< Split digtal curves on different lines

  void set_type(glow_eCurveType t);
  void set_digital_split(int set)
  {
    split_digsquare = set;
  }
  void get_digital_split(int* set)
  {
    *set = split_digsquare;
  }

  //! Configure the curves
  /*!
    \param data		Configuration data for the curves.

    Calculate position of the points of the curves and create a polyline for
    each curve.
  */
  void configure_curves(glow_sCurveData* data);

  //! Add a new value to the specified curve
  /*!
    \param data		Data for the new values of the curves.

    Add the new value first in all the curves, and shift the other values one
    step forward.
  */
  void add_points(glow_sCurveData* data, unsigned int* no_of_points);
};

/*@}*/
#endif
