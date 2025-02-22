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

#ifndef glow_color_h
#define glow_color_h

#include "glow.h"

/*! \file glow_color.h
    \brief Contains the GlowColor class. */
/*! \addtogroup Glow */
/*@{*/

//! Class for calculations and conversions of colors and color tones.
/*! The class contains functions to create color for the color palette,
    and to convert colors dependent on the current color tone, inverse, shadow
   etc.
*/
class GlowCustomColors;

class GlowColor {
public:
  //! Constructor
  GlowColor()
  {
  }

  //! Conversion from HIS to RGB
  /*! Uses the tranformation matrix:
    [R G B] = [M1 M2 I1] * [ 2/sqrt(6) -1/sqrt(6) -1/sqrt(6) ]
                           [ 0         1/sqrt(2)  -1/sqrt(2) ]
                           [ 1/sqrt(3) 1/sqrt(3)  1/sqrt(3)  ]
    where M1 = S * sin(H); M2 = s * cos(H); I1 = I/sqrt(3)
  */
  static void his_to_rgb(
      double* r, double* g, double* b, double h, double i, double s);

  //! Calculates the rgb values for a drawtype index
  static void rgb_color(
      int idx, double* r, double* g, double* b, GlowCustomColors* customcolors);

  //! Conversion from base drawtype to drawtype dependent on tone, brightness,
  //! highlight etc
  /*! If an object is a member of a node, the current properties of the node,
    such as
    tone, brightness, intensity, colorshift and inverse, controls the drawtype
    of the object.
  */
  static glow_eDrawType get_drawtype(glow_eDrawType local_drawtype,
      glow_eDrawType highlight_drawtype, int highlight, void* node, int fill,
      int highlight_disabled);

  //! Calculation of light and shadow drawtype for 3D effects
  /*!
    \param dt 	The original drawtype
    \param shift	The drawtype shift, 1 gives one step darker, -1 one step
    lighter
    \param node	The node that controls the colors of the object. Can be zero.
  */
  static glow_eDrawType shift_drawtype(
      glow_eDrawType dt, int shift, void* node);

  //! Conversion of colors between different Glow versions
  static glow_eDrawType convert(glow_eConvert version, glow_eDrawType color);

  //! Translates a drawtype to a color name
  /*!
    \param drawtype The color index
    \return The name of the color
  */
  static char* color_to_name(glow_eDrawType drawtype);

  //! Translates a tone to a colortone name
  /*!
    \param drawtype The color tone index
    \return The name of the color tone
  */
  static char* colortone_to_name(glow_eDrawType drawtype);

  static int is_shiftable(glow_eDrawType dt)
  {
    return (dt >= 20 && dt <= glow_eDrawType_Color300);
  }
  static int is_custom(glow_eDrawType dt)
  {
    return (
        dt >= glow_eDrawType_CustomColor1 && dt < glow_eDrawType_CustomColor__);
  }

  static void print_rgb_colors(GlowCustomColors* cc);
};

/*@}*/
#endif
