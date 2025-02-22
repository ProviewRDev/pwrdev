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

#ifndef glow_growctx_h
#define glow_growctx_h

#include "glow_ctx.h"
#include "glow_exportjbean.h"
#include "glow_customcolors.h"

/*! \file glow_growctx.h
    \brief Contains the GrowCtx class. */
/*! \addtogroup Glow */
/*@{*/

//! Editing modes
typedef enum {
  grow_eMode_Edit, //!< Ordinary edit mode
  grow_eMode_Rect, //!< Create rectangle mode
  grow_eMode_Line, //!< Create line mode
  grow_eMode_PolyLine, //!< Create polyline mode
  grow_eMode_Text, //!< Create text mode
  grow_eMode_Circle, //!< Create circle mode
  grow_eMode_Annot, //!< Create annotation mode
  grow_eMode_ConPoint, //!< Create connection point mode
  grow_eMode_Scale, //!< Scale object mode
  grow_eMode_EditPolyLine, //!< Edit polyline mode
  grow_eMode_RectRounded //!< Create rounded rectangle mode
} grow_eMode;

class GrowGroup;
class GrowDashboard;
class GrowLayer;

//! Extended class for the drawing window.
/*! GlowCtx adds functions to GlowCtx for creation of Grow objects, background
  color and images,
  an extended event handling, and more advanced drawing routines.
*/
class GrowCtx : public GlowCtx {
public:
  //! Constructor
  /*!
    \param ctx_name	Name of context.
    \param zoom_fact	Initial zoom factor.
  */
  GrowCtx(const char* ctx_name, double zoom_fact = 100);

  //! Destructor
  /*! Delete all objects. */
  ~GrowCtx();

  //! Event handler
  /*!
    \param event	Event.
    \param x		x coordinate for event.
    \param y		y coordinate for event.
    \param w		Width for exposure event.
    \param h		Height for exposure event.

    Receives all events in the window. Some event have special handling with
    rubberbanding,
    drawing of selection rectangles etc, others are just passed to backcall
    routines, if such
    routine is registred for the actual event.
  */
  int event_handler(glow_eEvent event, int x, int y, int w, int h);

  int subw_event_handler(glow_eEvent event, int x, int y, int w, int h);

  //! Draw a specified area of the window.
  /*!
    \param ll_x		x coordinate for lower left corner in pixel.
    \param ll_y		y coordinate for lower left corner in pixel.
    \param ur_x		x coordinate for upper right corner in pixel.
    \param ur_y		y coordinate for upper right corner in pixel.

    If draw is defered the area of defered redraw is extended with this new
    area, and nothing more is done.
    Sets a clip of the specified area and draws first all connections in the
    area, and then all other objects in the area. Draws the tooltip text, if 
    sthis is active, and selection rectangle if this is active.
  */
  void draw(GlowWind* w, int ll_x, int ll_y, int ur_x, int ur_y);
  void draw_invalidated(GlowWind* w, int ll_x, int ll_y, int ur_x, int ur_y);

  void erase(GlowWind* w, int ll_x, int ll_y, int ur_x, int ur_y);

  void redraw();

  //! Draw a specified area of the window.
  /*! Interface with double arguments */
  void draw(GlowWind* w, double ll_x, double ll_y, double ur_x, double ur_y)
  {
    draw(w, (int)ll_x, (int)ll_y, (int)ur_x, (int)ur_y);
  }

  //! Draw a specified area of the navigation window.
  /*! The drawing is done with extending drawing area which should be replaced
    with drawing with clip
    to increase performance.
  */
  void nav_draw(GlowWind* w, int ll_x, int ll_y, int ur_x, int ur_y);

  //! Draw a specified area of the navigation window.
  /*! Interface with double arguments */
  void nav_draw(GlowWind* w, double ll_x, double ll_y, double ur_x, double ur_y)
  {
    nav_draw(w, (int)ll_x, (int)ll_y, (int)ur_x, (int)ur_y);
  }

  //! Draw the defered drawing area.
  /*! Redraw the area and deactivate refered drawing. */
  void redraw_defered();

  //! Set edit mode.
  /*! \param grow_mode		Edit mode. */
  void set_mode(grow_eMode grow_mode);

  //! Get the edit mode.
  /*! \return Current edit mode. */
  grow_eMode mode()
  {
    return edit_mode;
  }

  //! Open and load a subgraph with a specified name.
  /*!
    \param name		Name of the subgraph.
    \param mode		Not used.
  */
  int open_subgraph_from_name(const char* name, glow_eSaveMode mode);

  //! Save as a subgraph to file, i.e. as a GlowNodeClass.
  /*!
    \param filename	Name of file to save in.
    \param mode		Save mode, should be glow_eSaveMode_Subgraph.
  */
  int save_subgraph(char* filename, glow_eSaveMode mode);

  //! Save as a flow nodeclass to file.
  /*!
    \param filename	Name of file to save in.
  */
  int export_flow(char* filename);

  //! Save graph as script.
  /*!
    \param filename	Name of file to save in.
  */
  int export_script(char* filename,
		    int (*userdata_cb)(void*, void*, std::ofstream&, char*));

  //! Open and load a subgraph from file.
  /*!
    \param filename	Name of file.
    \param mode		Not used.
  */
  int open_subgraph(char* filename, glow_eSaveMode mode);

  //! Save as graph to file.
  /*!
    \param filename	Name of file.
    \param mode		Save mode, should be glow_eSaveMode_Edit.
  */
  int save(char* filename, glow_eSaveMode mode);

  //! Save the grow part of the context.
  /*!
    \param fp		Output file.
    \param mode		Save mode.
  */
  void save_grow(std::ofstream& fp, glow_eSaveMode mode);

  //! Open the grow part of the context.
  /*! \param fp		Input file. */
  void open_grow(std::ifstream& fp);

  void save_meta(std::ofstream& fp, glow_eSaveMode mode);

  //! Find an object by name.
  /*!
    \param name		Name of object.
    \param element	Pointer to found object.
    \return		Returns 1 if object is found, else 0.
  */
  int find_by_name(const char* name, GlowArrayElem** element);

  //! Find a nodeclass by name.
  /*!
    \param name		Name of object.
    \param element	Pointer to found nodeclass.
    \return		Returns 1 if nodeclass is found, else 0.
  */
  int find_nc_by_name(const char* name, GlowArrayElem** element)
  {
    return a_nc.find_by_name(name, element);
  }

  //! Find a conclass by name.
  /*!
    \param name		Name of object.
    \param element	Pointer to found conclass.
    \return		Returns 1 if conclass is found, else 0.
  */
  int find_cc_by_name(char* name, GlowArrayElem** element)
  {
    return a_cc.find_by_name(name, element);
  }

  //! Get next connection point number for a subgraph
  /*!
    \return Next connection point number.

    Returns the number for next connectionpoint and increments the counter.
  */
  int get_next_conpoint_num()
  {
    return conpoint_num_cnt++;
  }

  //! Get next objectname number.
  /*!
    \return Next objectname number.

    Returns the number for next objectname.
  */
  int get_next_objectname_num()
  {
    return objectname_cnt;
  }

  //! Get and increment next objectname number.
  /*!
    \return Next objectname number.

    Returns the number for next objectname and increments the counter.
  */
  int incr_next_objectname_num()
  {
    return objectname_cnt++;
  }

  //! Set next layername number.
  /*!
    Set the number for next layername.
  */
  void set_next_objectname_num(int num)
  {
    objectname_cnt = num;
  }

  //! Get and increment next layername number.
  /*!
    \return Next layername number.

    Returns the number for next layername and increments the counter.
  */
  int incr_next_layername_num()
  {
    return layername_cnt++;
  }

  //! Clear the window.
  /*!
    \param keep_paste	Keep objects in the paste buffer, and their nodeclasses.

    Remove and deltet all objects in the context.
  */
  void clear_all(int keep_paste);

  //! Set context name.
  /*! \param grow_name	Context name. */
  void set_name(char* grow_name);

  //! Get context name.
  /*! \param grow_name	Context name. */
  void get_name(char* grow_name);

  //! Get list of currently moved objects.
  /*!
    \param list		Returned pointer to the move list.
    \param size		Number of objects in movelist.
  */
  void get_movelist(GlowArrayElem*** list, int* size)
  {
    *list = a_move.a;
    *size = a_move.size();
  }
  //! Reset polyline drawing sequence.
  void polyline_end()
  {
    polyline_not_first = 0;
  }

  //! Execute dynamic code for all objects.
  void exec_dynamic()
  {
    a.exec_dynamic();
  }

  //! Call the registred dynamic callback.
  void dynamic_cb(GlowArrayElem* object, char* code, glow_eDynamicType type);

  //! Set original border color on all selected objects.
  /*! \param drawtype	Original border color. */
  void set_select_original_border_color(glow_eDrawType drawtype)
  {
    a_sel.set_original_border_color(drawtype);
  }

  //! Set original text color on all selected objects.
  /*! \param drawtype	Original text color. */
  void set_select_original_text_color(glow_eDrawType drawtype)
  {
    a_sel.set_original_text_color(drawtype);
  }

  //! Set original fill color on all selected objects.
  /*! \param drawtype	Original fill color. */
  void set_select_original_fill_color(glow_eDrawType drawtype)
  {
    a_sel.set_original_fill_color(drawtype);
  }

  //! Set original color tone on all selected objects.
  /*! \param tone	Original color tone. */
  void set_select_original_color_tone(glow_eDrawTone tone)
  {
    a_sel.set_original_color_tone(tone);
  }

  //! Set original background color on all selected objects.
  /*! \param drawtype	Background color. */
  void set_select_original_background_color(glow_eDrawType drawtype)
  {
    a_sel.set_original_background_color(drawtype);
  }

  //! Set original color lighness on all selected objects.
  /*! \param lightness	Original color lightness. */
  void set_select_original_color_lightness(int lightness)
  {
    a_sel.set_original_color_lightness(lightness);
  }

  //! Increase original color lightness on all selected objects.
  /*! \param lightness	Original color lightness increment. */
  void incr_select_original_color_lightness(int lightness)
  {
    a_sel.incr_original_color_lightness(lightness);
  }

  //! Set original color intensity on all selected objects.
  /*! \param intensity	Original color intensity. */
  void set_select_original_color_intensity(int intensity)
  {
    a_sel.set_original_color_intensity(intensity);
  }

  //! Increase original color intensity on all selected objects.
  /*! \param intensity	Original color intensiity increment. */
  void incr_select_original_color_intensity(int intensity)
  {
    a_sel.incr_original_color_intensity(intensity);
  }

  //! Set original color shift on all selected objects.
  /*! \param shift	Original color shift. */
  void set_select_original_color_shift(int shift)
  {
    a_sel.set_original_color_shift(shift);
  }

  //! Increase original color shift on all selected objects.
  /*! \param shift	Original color shift. */
  void incr_select_original_color_shift(int shift)
  {
    a_sel.incr_original_color_shift(shift);
  }

  //! Set linewidth on all selected objects.
  /*! \param linewidth	Line width. */
  void set_select_linewidth(int linewidth)
  {
    a_sel.set_linewidth(linewidth);
  }

  //! Set or reset fill on all selected objects.
  /*! \param fill	Fill. */
  void set_select_fill(int fill)
  {
    a_sel.set_fill(fill);
  }

  //! Set or reset border on all selected objects.
  /*! \param border	Border. */
  void set_select_border(int border)
  {
    a_sel.set_border(border);
  }

  //! Set or reset shadow on all selected objects.
  /*! \param shadow	Shadow. */
  void set_select_shadow(int shadow)
  {
    a_sel.set_shadow(shadow);
  }

  //! Set gradient type on all selected objects.
  /*! \param gradient	Gradient type. */
  void set_select_gradient(glow_eGradient gradient)
  {
    a_sel.set_gradient(gradient);
  }

  //! Set linetype on all selected objects.
  /*! \param type       Linetype. */
  void set_select_linetype(glow_eLineType type)
  {
    a_sel.set_linetype(type);
  }

  //! Scale selected object.
  /*!
    \param scale_x	Scale factor in x direction.
    \param scale_y	Scale factor in y direction.
    \param type		Type of scaleing.

    Create a transform for the scaling and apply this to all selected objects.
  */
  void scale_select(double scale_x, double scale_y, glow_eScaleType type);

  //! Rotate selected objects.
  /*!
    \param angle	Angle in degrees of rotation.
    \param type		Type of rotation.

    Create a transform for the rotation and apply this to all selected objects.
  */
  void rotate_select(double angle, glow_eRotationPoint type);

  //! Set textsize on all selected objects.
  /*! \param size	Textsize. */
  void set_select_textsize(int size);

  //! Set or reset text bold on all selected objects.
  /*! \param bold	Bold. */
  void set_select_textbold(int bold);

  //! Set text font on all selected objects.
  /*! \param font	Font. */
  void set_select_textfont(glow_eFont font);

  void set_select_scale(double scale_x, double scale_y, glow_eScaleType type);

  //! Set background color of the window.
  /*! \param color	Background color. */
  void set_background(glow_eDrawType color);

  //! Get background color.
  /*! \param color	Returned background color. */
  void get_background(glow_eDrawType* color)
  {
    *color = background_color;
  }

  //! Set background image.
  /*! \param image	Name of image file. */
  void set_background_image(char* image);

  //! Get background image.
  /*! \param image	Name of image file. */
  void get_background_image(char* image);

  //! Get size of background image pixmap.
  /*!
    \param width	Width in pixel.
    \param height	Height in pixel.
  */
  int get_background_image_size(int* width, int* height);

  //! Reset background color.
  void reset_background();

  //! Shift a color.
  /*!
    \param dt		Color.
    \param shift	Amount of shift. Positive gives darker and negative
    ligther
    color.
    \param node		Parent node. Can be zero.
    \return		The shifted color.

    Shift the color to a color darker or lighter then the original, with the
    same color tone and intensity.
    The drawtype is calculated from the tone, intensity, lightness and inverse
    of the parent node.
  */
  glow_eDrawType shift_drawtype(glow_eDrawType dt, int shift, void* node)
  {
    return GlowColor::shift_drawtype(dt, shift, node);
  }

  //! Get color.
  /*!
    \param local_drawtype	Original color of object.
    \param highlight_drawtype	Color the object is draw width when it is
    highlighted.
    \param highlight		Object is highlighted.
    \param node			Parent node that control the color of the
    object.
    \param fill			The object is drawn with fill.

    The drawtype is calculated from the tone, intensity, lightness and inverse
    of the parent node.
  */
  glow_eDrawType get_drawtype(glow_eDrawType local_drawtype,
      glow_eDrawType highlight_drawtype, int highlight, void* node, int fill)
  {
    return GlowColor::get_drawtype(local_drawtype, highlight_drawtype,
        highlight, node, fill, highlight_disabled);
  }

  //! Disable highlight function.
  void disable_highlight()
  {
    highlight_disabled = 1;
  }

  //! Enable hightlight function.
  void enable_highlight()
  {
    highlight_disabled = 0;
  }

  //! Set dynamic code for subgraph.
  void set_dynamic(char* code, int size);

  //! Get dynamic code for subgraph.
  void get_dynamic(char** code, int* size)
  {
    *code = dynamic;
    *size = dynamicsize;
  }

  //! Get list of nodeclasses.
  /*!
    \param list		Pointer to list of nodeclasses.
    \param size		Number of nodeclasses in list.
  */
  void get_nodeclasslist(GlowArrayElem*** list, int* size)
  {
    *list = a_nc.a;
    *size = a_nc.size();
  }

  //! Get list of groups.
  /*!
    \param list		Pointer to list of groups.
    \param size		Number of groups in list.

    The list should be freed by the caller.
  */
  void get_nodegrouplist(GlowArrayElem*** list, int* size);

  //! Pop selected objects.
  /*! Move selected objects to last position in array. */
  void pop_select();

  //! Push selected objects.
  /*! Move selected objects to first position in array. */
  void push_select();

  //! Set default layout.
  /*! Adjust zoom factor to the current size of the window. */
  void set_default_layout();

  //! Set search path for subgraphs and images.
  /*!
    \param path_count		Number of paths in path_vect.
    \param path_vect		Array of paths char[10][80].
  */
  void set_path(int path_count, const char* path_vect);

  //! Check of any of the selected object is a connection object.
  /*! \return 	Returns 1 if a connection is selected, else 0. */
  int any_select_is_con();

  //! Change conclass for all selected connections to the specified conclass.
  /*! \param conclass	Connection class to change to. */
  void change_select_conclass(GlowArrayElem* conclass);

  //! Restrict movement of moved objects.
  /*!
    \param restriction		Type of restriction.
    \param max_limit		Max limit of movement. Used for slider movement.
    \param min_limit		Min limit of movement. Used for slider movement.
    \param object		Object for restrictions. User for slider
    movement.
  */
  void set_move_restrictions(glow_eMoveRestriction restriction,
      double max_limit, double min_limit, GlowArrayElem* object);

  //! Align selected objects.
  /*! \param direction		Alignment direction. */
  void align_select(glow_eAlignDirection direction);

  //! Set equal distance between selected objects.
  /*! \param direction		Direction in which to order objects. */
  void equidistance_select(glow_eAlignDirection direction);

  //! Check if object is a subgraph.
  /*! \return 		Returns 1 if object is a subgraph, else 0. */
  int is_subgraph()
  {
    return subgraph;
  }

  //! Check if object is a slider.
  /*! \return 		Returns 1 if object is a slider, else 0. */
  int is_slider()
  {
    return slider;
  }

  //! Set that graph is modified or not since last save.
  /*! \param mod 	1: graph is modified, 0: graph is not modified. */
  void set_modified(int mod)
  {
    modified = mod;
  }

  //! Get if graph is modified or not since last save.
  /*! \return  	Returns 1 if graph is modified, 0 if graph is not modified. */
  int get_modified()
  {
    return modified;
  }

  //! Measure the extension of a subgraph.
  /*!
    \param pix_x_right		Right border in pixel.
    \param pix_x_left		Left border in pixel.
    \param pix_y_high		High border in pixel.
    \param pix_y_low		Low border in pixel.

    If x0, y0, x1, y1 are set these are used as borders, otherwise the borders
    are calculated
    by meauring all objects i the contexts. The extension are given in pixels
    width the
    current zoom factor.
  */
  void measure_javabean(double* pix_x_right, double* pix_x_left,
      double* pix_y_high, double* pix_y_low);

  //! Convert coordinates to pixel.
  /*!
    \param x		x coordinate.
    \param y		y coordinate.
    \param pix_x	x coordinate in pixel.
    \param pix_y	y coordinate in pixel.
  */
  void to_pixel(double x, double y, double* pix_x, double* pix_y);

  //! Zoom to the java size of the picture.
  /*!
    \param pix_x_right	Right border in pixel.
    \param pix_x_left	Left border in pixel.
    \param pix_y_high	High border in pixel.
    \param pix_y_low	Low border in pixel.

    The java size is determined by java_width. The picture is zoomed to the
    level
    where the border width of the picture equals java_width.
  */
  void set_javaframe(double* pix_x_right, double* pix_x_left,
      double* pix_y_high, double* pix_y_low);

  //! Export context as a javabean.
  /*!
    \param fp		Output file.
    \param components	0: Paint. 1: Declare components. 2: Set component
    attributes.
  */
  void export_javabean(std::ofstream& fp, int components);

  //! Export a nodeclass as javabean.
  /*!
    \param nc		Node class.
    \param fp		Output file.
    \param components	0: Paint. 1: Declare components. 2: Set component
    attributes.
  */
  void export_nodeclass_javabean(
      GlowArrayElem* nc, std::ofstream& fp, int components);

  //! Export annotation fonts in a nodeclass.
  /*!
    \param nc		Nodeclass.
    \param fp		Output file.
    \param components	0: Paint. 1: Declare components. 2: Set component
    attributes.
  */
  void export_nc_javabean_font(
      GlowArrayElem* nc, std::ofstream& fp, int components);

  //! Set java name of the context.
  /*! \param name 	Name of java class for the graph. */
  void set_java_name(const char* name);

  //! Get java name of the context.
  /*! \param name 	Name of java class for the graph. */
  int get_java_name(char* name);

  //! Get the numbers of all annotations in a subgraph context.
  /*!
    \param numbers 	List of numbers.
    \param cnt		Number of numbers in the list.
  */
  void get_annotation_numbers(int** numbers, int* cnt);

  //! Send a hot request callback if such a callback is registred.
  /*!
    \param object	Object that is about to be hot.

    Request from an object that is covered by the cursor, to set itself hot.
  */
  int send_hot_request(GlowArrayElem* object);

  //! Send a menu callback if such a callback is registred.
  /*!
    \param object	Menu object.
    \param item		Activated menu item.
    \param event	Menu event.
    \param x		x coordinate.
    \param y		y coordinate.

    Send a menu callback.
  */
  int send_menu_callback(
      GlowArrayElem* object, int item, glow_eEvent event, double x, double y);

  //! Send a table callback if such a callback is registred.
  /*!
    \param object	Menu object.
    \param event	Menu event.
    \param x		x coordinate.
    \param y		y coordinate.
    \param column      	Activated cell column.
    \param row		Activated cell row.

    Send a table callback.
  */
  int send_table_callback(GlowArrayElem* object, glow_eEvent event, double x,
      double y, int column, int row);

  //! Send a toolbar callback if such a callback is registred.
  /*!
    \param object	Menu object.
    \param event	Menu event.
    \param x		x coordinate.
    \param y		y coordinate.
    \param idx      	Activated tool index.

    Send a toolbar callback.
  */
  int send_toolbar_callback(GlowArrayElem* object, glow_eEvent event, double x,
      double y, int category, int idx);

  //! Get configured scantimes.
  /*!
    \param time		Scantime for slow cycle.
    \param fast_time	Scantime for fast cycle.
    \param animation_time Scantime for animations.
  */
  void get_scantime(double* time, double* fast_time, double* animation_time)
  {
    *time = scantime;
    *fast_time = fast_scantime;
    *animation_time = animation_scantime;
  }

  //! Store the current zoomfactor and offsets.
  /*! The stored geometry is restored with restore_geometry(); */
  void store_geometry();

  //! Restore the stored geometry.
  /*! Restore the zoomfactor and offsets previously stored with store_geometry()
   */
  void restore_geometry();

  //! Get extent of a text
  /*!
    \param text		Text
    \param len		Lenght of text.
    \param draw_type	Text drawtype.
    \param text_size	Size of text.
    \param width	Returned width of text.
    \param height	Returned height of text.
    \param descent	Returnd descent of text.
  */
  void get_text_extent(char* text, int len, glow_eDrawType draw_type,
      int text_size, glow_eFont font, double* width, double* height,
      double* descent);

  //! Get the window size
  /*!
    \param width	Window width.
    \param height	Window height.
    \return		Returns 1 if size is configured, otherwise 0.
  */
  int get_window_size(double* width, double* height);

  //! Get the default window size
  /*!
    \param width	Window width in pixel.
    \param height	Window height in pixel.
    \return		Returns 1 if size is configured, otherwise 0.

    The window size is configured in x0, y0, x1 and y1. From these the window
    size in pixel is calculated.
  */
  int get_default_window_size(int* width, int* height);

  //! Set default layout to the specified values.
  /*!
    \param nx0		x coordinate for left border.
    \param ny0		y coordinate for low border.
    \param nx1		x coordinate for right border.
    \param ny1		y coordinate for high border.
  */
  void set_layout(double nx0, double ny0, double nx1, double ny1)
  {
    x0 = nx0;
    y0 = ny0;
    x1 = nx1;
    y1 = ny1;
  }

  //! Get layout
  /*!
    \param nx0		x coordinate for left border.
    \param ny0		y coordinate for low border.
    \param nx1		x coordinate for right border.
    \param ny1		y coordinate for high border.
  */
  void get_layout(double* nx0, double* ny0, double* nx1, double* ny1)
  {
    *nx0 = x0;
    *ny0 = y0;
    *nx1 = x1;
    *ny1 = y1;
  }

  //! Group the selected objects.
  /*!
    \param group	Created group.
    \param last_group	Name of last group.

    The last group is the group name registred as last group among the majority
    of the
    group members
  */
  int group_select(GlowArrayElem** group, char* last_group);

  //! Ungroup selected groups.
  /*! \return	Always returns 1. */
  int ungroup_select();

  //! Ungroup a groups.
  void ungroup_group(GrowGroup* group);

  //! Get the group the specified object is a member of.
  /*!
    \param object	Object to get group for.
    \param group	Found group for the object.
    \return		Returns 1 if a group is found, 0 if the object isn't a
    memeber of a group.
  */
  int get_object_group(GlowArrayElem* object, GlowArrayElem** group);

  //! Get the limits configured in a background object for a slider.
  /*!
    \param type		Dyntype for the background object.
    \param x		x coordinate for the foreground object.
    \param y		y coordinate for the foreground object.
    \param background  	Found background object.
    \param min		Min limit for background object.
    \param max		Max limit for background object.
    \param direction	Direction if background object.

    The background object is an object with the specified dyntype that covers
    the specified
    coordinates. The limits are stored in y0 and y1 for the nodeclass of the
    objects.
  */
  int get_background_object_limits(glow_eTraceType type, double x, double y,
      GlowArrayElem** background, double* min, double* max,
      glow_eDirection* direction);

  //! Set or reset scaletype to equal scale.
  /*!
    \param equal	Scaleing of objects are equal in x and y direction.

    Scale equal means that when the scale of an objects is changed, the
    proportions in x and y
    direction is kept.
  */
  void set_scale_equal(int equal)
  {
    scale_equal = equal;
  }

  //! Call the translate callback if such a callback is registred.
  /*!
    \param object	Object that send the callback.
    \param text		Text to be translated.
    \param new_text	Text that is translated by the callback function.
    \return		Returns 1 if a callback function is registred, else 0.

    Translation callbacks are sent for all text and annotations when opening a
    graph, if
    translate_on is set.
  */
  int translate_cb(GlowArrayElem* object, char* text, char** new_text);

  //! Flip the selected objects.
  /*! \param dir	Flip direction. */
  void flip_select(glow_eFlipDirection dir);

  //! Conversion between different versions of Glow
  /*!
    \param version	Version to convert to.
  */
  void convert(glow_eConvert version);

  //! Close annotation input for all nodes.
  void close_annotation_input_all();

  //! Remove input focus for all nodes.
  void reset_input_focus_all();

  //! Send an input focus init event.
  void inputfocus_init_event();

  //! Delete all child menues to a menu object.
  /*! \param parent	The parent menu object. */
  void delete_menu_child(GlowArrayElem* parent);

  //! Get owner.
  /*! \param o		Owner string. */
  void get_owner(char* o);

  //! Get owner.
  /*! \param o		Owner string. */
  void set_owner(char* o);

  //! Get next object regarding position. */
  int get_next_object_position(
      GlowArrayElem* object, glow_eDirection dir, GlowArrayElem** next);

  //! Get previous object. */
  int get_previous_object(GlowArrayElem* object, GlowArrayElem** prev)
  {
    return a.get_previous(object, prev);
  }

  //! Get next object. */
  int get_next_object(GlowArrayElem* object, GlowArrayElem** next)
  {
    return a.get_next(object, next);
  }

  //! Get first object. */
  int get_first_object(GlowArrayElem** first)
  {
    return a.get_first(first);
  }

  //! Check if object is visible. */
  int is_visible(GlowArrayElem* element, glow_eVisible type);

  //! Read object from file. */
  void read_object(std::ifstream& fp, GlowArrayElem** o);

  //! Order object. */
  int order_object(GlowArrayElem* o, GlowArrayElem* dest, glow_eDest code)
  {
    return a.move(o, dest, code);
  }

  void measure_window(double* ll_x, double* ll_y, double* ur_x, double* ur_y);

  int key_pressed(int key);
  int signal_send(char* signalname);

  void pop(GlowArrayElem* element)
  {
    layer->pop(element);
  }
  void set_text_coding(glow_eTextCoding coding);
  void set_edit_set_mode(glow_eEditSetMode mode)
  {
    edit_set_mode = mode;
  }
  int set_custom_color(
      glow_eDrawType color, double red, double green, double blue);
  void reset_custom_colors();
  int custom_colors_is_empty();
  int read_customcolor_file(char* name);
  int write_customcolor_file(char* name);
  int check_object_name(char* name);
  void get_dashboard_info(double* cell_width, double* cell_height, int* columns, int* rows);
  void set_color_theme();
  int is_dashboard() 
  {
    return dashboard;
  }
  int layer_active();
  int get_active_layer(GrowLayer **layer);
  int merge_visible_layers();
  int merge_visible_layers_to_bg();
  int merge_all_layers();
  int move_select_to_layer();
  void layer_reset_active_all();
  static void set_default_color_theme(char* theme);

  static int get_dimension(char* filename, int* width, int* heigth);

  char name[40]; //!< Name of the context.
  grow_eMode edit_mode; //!< Current edit mode.
  int conpoint_num_cnt; //!< Counter to get next number for when creating
  //! conpoints in a subgraph.
  int objectname_cnt; //!< Counter to get next number when creating new object
  //! names.
  int layername_cnt; //!< Counter to get next number when creating new layer
  //! names.
  int polyline_not_first; //!< Indicates that the first line is drawn for the
  //! current polyline.
  int polyline_start_x; //!< x coordinate for the start point of the current
  //! polyline.
  int polyline_start_y; //!< y coordinate for the start point of the current
  //! polyline.
  int polyline_last_end_x; //!< x coordinate for the last point of the current
  //! polyline.
  int polyline_last_end_y; //!< y coordinate for the last point of the current
  //! polyline.
  int select_rect_stored_ll_x; //!< Stored lower left x coordinate for selection
  //! rectangle.
  int select_rect_stored_ll_y; //!< Stored lower left y coordinate for selection
  //! rectangle.
  int select_rect_stored_ur_x; //!< Stored upper right x coordinate for
  //! selection rectangle.
  int select_rect_stored_ur_y; //!< Stored upper right y coordinate for
  //! selection rectangle.
  double scale_x; //!< Scalefactor in x direction for scale of selected objects.
  double scale_y; //!< Scalefactor in y direction for scale of selected objects.
  double scale_center_x; //!< x coordinate for scaling point.
  double scale_center_y; //!< y coordinate for scaling point.
  glow_eScaleType scale_type; //!< Type of scaling.
  glow_eDrawType background_color; //!< Background color of the window.
  int highlight_disabled; //!< Highlight is disabled.
  char* dynamic; //!< Dynamic code for a subgraph.
  int dynamicsize; //!< Size of dynamic code.
  int arg_cnt; //!< Number of arguments for dynamic code.
  char argname[20][32]; //!< Name of arguments for dynamic code.
  int argtype[20]; //!< Type of the arguments for dynamic code.
  double x0; //!< Left border of graph.
  double y0; //!< Low border of graph.
  double x1; //!< Right border of graph.
  double y1; //!< High border of graph.
  int path_cnt; //!< Number of paths in searchpath
  char path[10][80]; //!< Search paths where subgraphs and imagefiles are
  //! searched for.
  int dyn_type1; //!< Dyntype for a subgraph mask 1.
  int dyn_type2; //!< Dyntype for a subgraph mask 2.
  int dyn_action_type1; //!< Action type for a subgraph mask 1.
  int dyn_action_type2; //!< Action type for a subgraph mask 2.
  glow_eDrawType dyn_color[4]; //!< Dynamic colors for a subgraph.
  int dyn_attr[4]; //!< Dynamic attributes for a subgraph.
  int no_con_obstacle; //!< Subgraph is not an obstacle for a connection.
  int slider; //!< Subgraph is a slider.
  double slider_cursor_offset; //!< Offset between the cursor and coordinate for
  //! the currently moved slider.
  glow_eMoveRestriction move_restriction; //!< Actual move restrictions.
  double restriction_max_limit; //!< Max limit of move restrictions. Used for
  //! sliders.
  double restriction_min_limit; //!< Min limit of move restrictions. Used for
  //! sliders.
  GlowArrayElem* restriction_object; //!< Object the restrictions are valid for.
  //! Used for sliders.
  int subgraph; //!< Context is a subgraph.
  int modified; //!< Context is modified since last save operation.
  GlowExportJBean* export_jbean; //!< Java export object.
  char java_name[40]; //!< Name of javaclass for context.
  char next_subgraph[40]; //!< Name of subgraph for next page.
  int animation_count; //!< Number of scans this page is displayed in an
  //! animation.
  double scantime; //!< Scantime for slow cycle.
  double fast_scantime; //!< Scantime for fast cycle.
  double animation_scantime; //!< Scantime for animations.
  int stored_offset_x; //!< Stored geometry: offset_x.
  int stored_offset_y; //!< Stored geometry: offset_y.
  double stored_zoom_factor_x; //!< Stored geometry: zoom_factor_x.
  double stored_zoom_factor_y; //!< Stored geometry: zoom_factor_y.
  double stored_grid_size_x; //!< Stored geometry: grid_size_x.
  double stored_grid_size_y; //!< Stored geometry: grid_size_y.
  int stored_grid_on; //!< Stored geometry: grid_on.
  int stored_show_grid; //!< Stored geometry: show_grid.
  int java_width; //!< Width in pixels when the graph is exported as a java
  //! frame.
  int enable_bg_pixmap; //!< Display the background image.
  char background_image[80]; //!< Filename of background image.
  int background_tiled; //!< Background image it tiled, otherwised it is scaled
  //! to cover the window.
  int scale_active; //!< Scale of selected objects is active.
  int initial_position; //!< Initial position for curve contexts (a
  //! glow_eDirection).
  int is_javaapplet; //!< Graph is exported as a java applet.
  int is_javaapplication; //!< Graph is exported as a java frame.
  glow_eCycle cycle; //!< Fast of slow cycle as default for subgraph.
  glow_eMB3Action mb3_action; //!< Action performed when MB3 is clicked.
  int scale_equal; //!< Scaleing of objects are equal in x and y direction.
  int translate_on; //!< Send a translate callback to translate all texts of
  //! text objects and annotations.
  glow_eInputFocusMark
      input_focus_mark; //!< How to mark input focus for a subgraph.
  int background_disabled; //!< Disable drawing of background.
  void (*redraw_callback)(void*); //!< Backcall function before drawing (if ctx
  //! is window component).
  void* redraw_data; //!< Data for redraw callback.
  int has_subwindows; //!< Graph contains subwindow objects (GrowWindow or
  //! GrowFolder)
  int is_subwindow; //!< Is a subwindow context.
  char owner[256]; //!< Owner, used by application
  int anti_aliasing; //!< Use anti-aliasing
  int window_resize; //!< Window resize restrictions, non or aspect ratio.
  glow_eEnv environment; //!< Environment Development or Runtime.
  glow_eTextCoding text_coding; //!< Text coding
  int recursive_trace; //!< Subgraph recursive trace
  glow_eEditSetMode edit_set_mode; //!< Special edit mode
  int dashboard;
  GrowDashboard *dash;
  double dash_cell_width; //!< Width of dashboard cell
  double dash_cell_height; //!< Height of dashboard cell
  int disable_subw_events; //!< Disable eventhandling in subwindows and tables
  int current_color_theme; //!< Current color theme index
  void set_layer_borders(); //!< Set borders of active layer
};

void grow_auto_scrolling(GrowCtx* ctx);

/*@}*/
#endif
