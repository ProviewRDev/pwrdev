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

#ifndef ge_dashboard_h
#define ge_dashboard_h

/* ge_dashboard.h -- Ge dashboard */

#include "ge_graph.h"

/*! \addtogroup GeDashBoard */
/*@{*/

#define DASH_MAX_ELEM 10

//! Type of dash.
typedef enum {
  ge_eDashType_,
  ge_eDashType_UserDefined,
  ge_eDashType_ObjectGraph,
  ge_eDashType_Bar,
  ge_eDashType_BarArc,
  ge_eDashType_Trend,
  ge_eDashType_Gauge,
  ge_eDashType_Gauge2,
  ge_eDashType_Slider,
  ge_eDashType_Pie,
  ge_eDashType_Indicator,
  ge_eDashType_DigitalTrend
} ge_eDashType;

//! Type of dash attributes.
typedef enum {
  ge_eDashAttr_No,
  ge_eDashAttr_Object,
  ge_eDashAttr_Analog,
  ge_eDashAttr_Digital
} ge_eDashAttr;

typedef enum {
  dash_eSave_Dash = 1,
  dash_eSave_Analog = 2,
  dash_eSave_Digital = 3,
  dash_eSave_Object = 4,
  dash_eSave_End = 999,
  dash_eSave_Dash_type = 1000,
  dash_eSave_Dash_attr_type = 1001,
  dash_eSave_Dash_title = 1002,
  dash_eSave_Dash_script = 1003,
  dash_eSave_Dash_elements = 1004,
  dash_eSave_Dash_time_range = 1005,
  dash_eSave_Dash_direction = 1006,
  dash_eSave_Analog_attribute = 1100,
  dash_eSave_Analog_format = 1101,
  dash_eSave_Analog_min_value = 1102,
  dash_eSave_Analog_max_value = 1103,
  dash_eSave_Analog_text = 1104,
  dash_eSave_Digital_attribute = 1200,
  dash_eSave_Digital_text = 1201,
  dash_eSave_Digital_color = 1202,
  dash_eSave_Digital_flash = 1203,
  dash_eSave_Object_object = 1300,
  dash_eSave_Object_text = 1301,
  dash_eSave_Object_min_value = 1302,
  dash_eSave_Object_max_value = 1303
} dash_eSave;

/*@}*/

class GeDashElem;
class GeDashAnalog;
class GeDashDigital;
class Graph;

class GeDash {
public:
  Graph* graph; //!< Graph.
  int width;
  int height;
  ge_eDashType type;
  int new_type;
  ge_eDashAttr attr_type;
  char title[80];
  double time_range;
  glow_eDirection direction;
  
  char script[80];
  unsigned int elements;
  GeDashElem *elem[DASH_MAX_ELEM];
  ge_eDashType old_type;
  int old_rows;
  int old_columns;
  char old_title[80];
  unsigned int old_elements;
  

  GeDash(Graph *d_graph) : graph(d_graph), type(ge_eDashType_), 
    new_type(0), attr_type(ge_eDashAttr_No), time_range(0), 
    direction(glow_eDirection_Center), elements(0), 
    old_type(ge_eDashType_), old_rows(1), old_columns(1), old_elements(0) {
    memset(elem, 0, sizeof(elem));
    strcpy(title, "");
    strcpy(old_title, "");
    strcpy(script, "");
  }

  //! Copy constructor.
  GeDash(const GeDash& x);

  void merge(grow_tObject o, GeDash *x) {
    if (elements + x->elements > max_elements())
      return;
    for (int i = 0; i < x->elements; i++) {
      elem[elements + i] = x->elem[i];
      x->elem[i] = 0;      
    }
    elements += x->elements;
    old_elements = elements;
    x->elements = 0;
    update(o);
  }

  ge_eDashAttr get_attr_type();
  int max_elements();

  //! Get list of attributes.
  /*!
    \param attrinfo    	List of attribute items.
    \param item_count	Number of items in list.
  */
  void get_attributes(grow_tObject o, attr_sItem* attrinfo, int* item_count);

  void replace_attribute(char* from, char* to, int* cnt, int strict);
  void set_attribute(grow_tObject object, const char* attr_name, int second);

  //! Save dash data to file.
  /*! \param fp		Output file. */
  void save(std::ofstream& fp);

  //! Open dash data from file.
  /*! \param fp		Input file. */
  void open(std::ifstream& fp);

  //! Update after edit.
  void update_elem(grow_tObject o);
  void update(grow_tObject o);

  int connect(grow_tObject o, int idx, char* attr, pwr_tTypeId atype);

  //! Destructor
  ~GeDash();
};

class GeDashElem {
public:
  GeDash *dash;

  GeDashElem(GeDash *d) : dash(d) {}
  GeDashElem(const GeDashElem& x) : dash(x.dash) {}

  //! Get script call string.
  /*!
    \param o    	Dash object.
    \param script	Script string.
  */
  virtual void get_script(grow_tObject o, char *script)
  {
  }

  //! Get list of attributes.
  /*!
    \param attrinfo    	List of attribute items.
    \param item_count	Number of items in list.
  */
  virtual void get_attributes(attr_sItem* attrinfo, int* item_count, char *name,
      unsigned int attr_mask, int num)
  {
  }

  //! Set an 'Attribute' attribute, i.e. a reference to the database attribute.
  /*!
    \param object	Owner object.
    \param attr_name	Database reference.
    \param cnt		Counter to decide if first or second attribute is to be
    set.
  */
  virtual void set_attribute(
      grow_tObject object, const char* attr_name, int second)
  {
  }

  //! Replace an attribute string
  /*!
    \param from		Attribute string to replace.
    \param to		New attribute string.
    \param cnt		Counter of replaced attributes.
    \param strict	If 1 the comparation is case sensitive.
  */
  virtual void replace_attribute(char* from, char* to, int* cnt, int strict)
  {
  }

  //! Save dash element data to file.
  /*! \param fp		Output file. */
  virtual void save(std::ofstream& fp)
  {
  }

  //! Open dash element data from file.
  /*! \param fp		Input file. */
  virtual void open(std::ifstream& fp)
  {
  }

  //! Destructor
  virtual ~GeDashElem()
  {
  }
};

class GeDashAnalog : public GeDashElem {
public:
  pwr_tAName attribute;
  char text[80];
  char format[80];
  double min_value;
  double max_value;
  
  GeDashAnalog(GeDash *d) : GeDashElem(d), min_value(0), max_value(0) {
    strcpy(attribute, "");
    strcpy(text, "");
    strcpy(format, "");
  }
  GeDashAnalog(const GeDashAnalog& x) : GeDashElem(x), min_value(x.min_value), 
    max_value(x.max_value) {
    strcpy(attribute, x.attribute);
    strcpy(text, x.text);
    strcpy(format, x.format);
  }
  void get_script(grow_tObject o, char *script);
  void get_attributes(attr_sItem* attrinfo, int* item_count, char *name, 
      unsigned int attr_mask, int num);
  void set_attribute(grow_tObject object, const char* attr_name, int second);
  void replace_attribute(char* from, char* to, int* cnt, int strict);
  void save(std::ofstream& fp);
  void open(std::ifstream& fp);
};

class GeDashDigital : public GeDashElem {
public:
  pwr_tAName attribute;
  char text[80];
  glow_eDrawType color;
  int flash;
  
  GeDashDigital(GeDash *d) : GeDashElem(d), color(glow_eDrawType_CustomColor26),
    flash(0) {
    strcpy(attribute, "");
    strcpy(text, "");
  }
  GeDashDigital(const GeDashDigital& x) : GeDashElem(x), color(x.color),
      flash(x.flash) {
    strcpy(attribute, x.attribute);
    strcpy(text, x.text);
  }
  void get_script(grow_tObject o, char *script);
  void get_attributes(attr_sItem* attrinfo, int* item_count, char *name, 
      unsigned int attr_mask, int num);
  void set_attribute(grow_tObject object, const char* attr_name, int second);
  void replace_attribute(char* from, char* to, int* cnt, int strict);
  void save(std::ofstream& fp);
  void open(std::ifstream& fp);
};

class GeDashObject : public GeDashElem {
public:
  pwr_tAName object;
  char text[80];
  double min_value;
  double max_value;
  
  GeDashObject(GeDash *d) : GeDashElem(d), min_value(0), max_value(100) {
    strcpy(object, "");
    strcpy(text, "");
  }
  GeDashObject(const GeDashObject& x) : GeDashElem(x), min_value(x.min_value),
      max_value(x.max_value) {
    strcpy(object, x.object);
    strcpy(text, x.text);
  }
  void get_script(grow_tObject o, char *script);
  void get_attributes(attr_sItem* attrinfo, int* item_count, char *name, 
      unsigned int attr_mask, int num);
  void set_attribute(grow_tObject object, const char* attr_name, int second);
  void replace_attribute(char* from, char* to, int* cnt, int strict);
  void save(std::ofstream& fp);
  void open(std::ifstream& fp);
  char* script_filename();
};

#endif
