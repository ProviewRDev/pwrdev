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

#ifndef glow_growslider_h
#define glow_growslider_h

#include "glow_grownode.h"

/*! \file glow_growslider.h
    \brief Contains the GrowSlider class. */
/*! \addtogroup Glow */
/*@{*/

//! Class for a subgraph object with slider dynamic.
/*! A GrowSlider object is a subgraph object that the user can move between
  along a horizontal or
  vertical line between two limit points. The position of the slider reflects
  the value of a parameter.

  The GrowSlider class contains function for drawing the object, and handle
  events when the
  object is clicked on, moved etc.
*/
class GrowSlider : public GrowNode {
public:
  //! Constuctor
  /*!
    \param glow_ctx 	The glow context.
    \param name		Name (max 31 char).
    \param node_class	Pointer to NodeClass object.
    \param x1		x coordinate for position.
    \param y1		y coordinate for position.
    \param nodraw	Don't draw the object now.
    \param rel_annot_pos Not used.
  */
  GrowSlider(GrowCtx* glow_ctx, const char* name, GlowNodeClass* node_class,
      double x1, double y1, int nodraw = 0, int rel_annot_pos = 0);

  //! Noargs constructor
  GrowSlider(){}

  //! Make this object a copy of another image object.
  /*!
    \param n	Object to copy.
  */
  void copy_from(const GrowSlider& n);

  //! Get the object type
  /*!
    \return The type of the object.
  */
  glow_eObjectType type()
  {
    return glow_eObjectType_GrowSlider;
  }

  //! Save the content of the object to file.
  /*!
    \param fp	Ouput file.
    \param mode	Save as graph or subgraph.
  */
  void save(std::ofstream& fp, glow_eSaveMode mode);

  //! Read the content of the object from file.
  /*!
    \param fp	Input file.
  */
  void open(std::ifstream& fp);

  //! Set range of the slider value.
  /*!
    \param min_val	Min value.
    \param max_val	Max value.
  */
  void set_range(double min_val, double max_val);

  //! Get slider info.
  /*!
    \param dir		Direction of the slider.
    \param max_val	Max value of the slider value.
    \param min_val	Min value of the slider value.
    \param max_position	Coordinate of the max position.
    \param min_position	Coordinate of the min position.
  */
  void get_info(glow_eDirection* dir, double* max_val, double* min_val,
      double* max_position, double* min_position);
  //! Get the zero position of the slider.
  /*!
    \param direction	Direction of the slider.
    \param pos		Position of the zero point.

    The zero point for the slider is the point in the slider object that
    corresponds to the slider value.
    This can be a x coordinate or an y coordinate dependent of the actual
    direction of the slider.
    The zero point for a slider is stored i y0 for the NodeClass of the slider,
    in the nodeclass
    coordinate system.
  */
  void get_origo(glow_eDirection direction, double* pos);

  //! Set slider info.
  /*!
    \param dir		Direction of the slider.
    \param max_val	Max value of the slider value.
    \param min_val	Min value of the slider value.
    \param max_position	Coordinate of the max position.
    \param min_position	Coordinate of the min position.
  */
  void set_info(glow_eDirection dir, double max_val, double min_val,
      double max_position, double min_position);

  //! Export the object as a java bean.
  /*!
    \param t		Transform of parent node. Can be zero.
    \param node		Parent node. Can be zero.
    \param pass		Export pass.
    \param shape_cnt	Current index in a shape std::vector.
    \param node_cnt	Counter used for javabean name. Not used for this kind
    of
    object.
    \param in_nc	Member of a nodeclass. Not used for this kind of object.
    \param fp		Output file.

    The object is transformed to the current zoom factor, and GlowExportJBean is
    used to generate
    java code for the java bean.
  */
  void export_javabean(GlowTransform* t, void* node, glow_eExportPass pass,
      int* shape_cnt, int node_cnt, int in_nc, std::ofstream& fp);

  //! Get slider info with positions in pixel.
  /*!
    \param dir		Direction of the slider.
    \param max_position	Coordinate of the max position.
    \param min_position	Coordinate of the min position.
    \param bg_dyn_type	Dyntype of the background object to the slider.
  */
  void get_info_pixel(glow_eDirection* dir, double* max_position,
      double* min_position, int bg_dyn_type);

  glow_eDirection direction; //!< Direction of the slider.
  double max_value; //!< Max value of the slider value.
  double min_value; //!< Min value of the slider value.
  double max_pos; //!< End position of the slider.
  double min_pos; //!< End position of the slider.
};

/*@}*/
#endif
