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

//
// Search for DeviceID and VendorID in gsdml files and extract ProductFamly and
// TextInfo.
// The content should be written in $pwr_exe/profinet_deviceid.dat which is used
// by the
// Proview Viewer to show info for a device.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <chrono>

#include "co_dcli.h"
#include "co_string.h"
#include "co_pugixml.hpp"

void generate_vendors(char* filename, pugi::xml_node* output);
void parse_gsdml(char* filename, pugi::xml_node* output);

int main(int argc, char* argv[])
{
  pwr_tFileName search_criteria, found_file, vendor_file;
  pwr_tStatus sts;
  pugi::xml_document output;
  pugi::xml_node pn_devices = output.append_child("ProfinetDevices");

  const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  const std::time_t generation_time = std::chrono::system_clock::to_time_t(now);

  std::ostringstream comment;
  comment << std::endl
          << "This document was generated by the pn_get_deviceid utility. It is read by the Profinet Viewer "
             "to map vendor/device IDs to readable names and also provides descriptions as to what type it "
             "is and what the device does."
          << std::endl
          << "Generated on " << std::put_time(std::localtime(&generation_time), "%F %T") << std::endl;
  output.prepend_child(pugi::node_comment).set_value(comment.str().c_str());

  if (argc < 4 || streq(argv[1], "--help"))
  {
    std::cout << std::endl
              << "Usage: pn_get_deviceid {file pattern} {manufacturer file provided by profibus foundation} "
                 "{output xml file}"
              << std::endl;
    std::cout << "Example: \n\t pn_get_deviceid \"/data1/gsdml/GSDML*xml\" ~/path/to/Man_ID_Table.xml "
                 "~/profinet_devices.xml"
              << std::endl;
    std::cout << std::endl
              << "Note: You do need the quotation marks. Otherwise the shell will expand the wildcard and "
                 "thus overwrite whatever file is expanded as the third argument passed to this utility."
              << std::endl
              << "Note2: The Man_ID_Table.xml may be downloaded from "
                 "https://www.profibus.com/IM/Man_ID_Table.xml if you are a member."
              << std::endl;
    exit(0);
  }

  dcli_translate_filename(vendor_file, argv[2]);
  std::cout << "Processing vendor file: " << vendor_file << std::endl;
  generate_vendors(argv[2], &pn_devices);

  dcli_translate_filename(search_criteria, argv[1]);
  sts = dcli_search_file(search_criteria, found_file, DCLI_DIR_SEARCH_INIT);
  while (ODD(sts))
  {
    std::cout << "Processing file: " << found_file << std::endl;
    parse_gsdml(found_file, &pn_devices);

    sts = dcli_search_file(search_criteria, found_file, DCLI_DIR_SEARCH_NEXT);
  }
  dcli_search_file(search_criteria, found_file, DCLI_DIR_SEARCH_END);

  output.save_file(argv[3]);
}

void generate_vendors(char* filename, pugi::xml_node* output)
{
  pwr_tFileName fname;

  dcli_translate_filename(fname, filename);

  pugi::xml_document input_doc;
  pugi::xml_parse_result result = input_doc.load_file(fname);

  if (result.status == pugi::status_ok)
  {
    pugi::xml_node Vendor;
    // Add Vendor
    for (pugi::xml_node& Manufacturer : input_doc.child("Man_ID_Table").children("Manufacturer"))
    {
      Vendor = output->append_child("Vendor");
      Vendor.append_attribute("ID") =
          Manufacturer.attribute("ID").value(); // These are already in decimal form
      Vendor.append_attribute("Name") =
          Manufacturer.child("ManufacturerInfo").child("ManufacturerName").child_value();
    }
  }
  else
  {
    std::cerr << "Unable to open/parse Profibus Vendor file " << fname << std::endl;
    std::cerr << "Did you specify the correct name? Usually it's Man_ID_Table.xml" << std::endl;
    return;
  }
}

void parse_gsdml(char* filename, pugi::xml_node* output)
{
  pwr_tFileName fname;

  dcli_translate_filename(fname, filename);

  pugi::xml_document input_doc;
  pugi::xml_parse_result result = input_doc.load_file(fname);

  if (result.status == pugi::status_ok)
  {
    // Fetch references to certain nodes for quick reference/readability
    pugi::xml_node DeviceIdentity =
        input_doc.select_node("/ISO15745Profile/ProfileBody/DeviceIdentity").node();
    pugi::xml_node Family =
        input_doc.select_node("/ISO15745Profile/ProfileBody/DeviceFunction/Family").node();
    pugi::xml_node TextList = input_doc
                                  .select_node("/ISO15745Profile/ProfileBody/ApplicationProcess/"
                                               "ExternalTextList/PrimaryLanguage")
                                  .node();

    // Create a variable set to populate for use with xpath expressions
    pugi::xpath_variable_set vars;
    vars.add("TextId", pugi::xpath_type_string);
    vars.add("VendorID", pugi::xpath_type_number);
    vars.add("DeviceID", pugi::xpath_type_number);
    vars.set("VendorID", DeviceIdentity.attribute("VendorID").as_double());
    vars.set("DeviceID", DeviceIdentity.attribute("DeviceID").as_double());
    vars.set("InfoTextId", DeviceIdentity.child("InfoText").attribute("TextId").value());

    // Extract the device description
    pugi::xml_node device_identity_infotext =
        TextList.select_node("Text[@TextId = string($InfoTextId)]", &vars).node();

    // Add Vendor
    pugi::xml_node Vendor = output->select_node("Vendor[@ID = number($VendorID)]", &vars).node();
    if (!Vendor)
    {
      Vendor = output->append_child("Vendor");
      Vendor.append_attribute("ID") = DeviceIdentity.attribute("VendorID").as_double();
      Vendor.append_attribute("Name") = DeviceIdentity.child("VendorName").attribute("Value").value();
    }

    // Add Device
    pugi::xml_node Device = Vendor.select_node("Device[@ID = number($DeviceID)]", &vars).node();
    if (!Device)
    {
      Device = Vendor.append_child("Device");
      Device.append_attribute("ID") = DeviceIdentity.attribute("DeviceID").as_double();
      Device.append_attribute("Description") = device_identity_infotext.attribute("Value").value();
      Device.append_attribute("Type") = Family.attribute("MainFamily").value();
      Device.append_attribute("Name") = Family.attribute("ProductFamily").value();
    }
  }
  else
  {
    std::cerr << "Unable to open/parse file " << fname << std::endl;
    std::cerr << "Correct any errors in the file and try again" << std::endl;
    return;
  }
}
