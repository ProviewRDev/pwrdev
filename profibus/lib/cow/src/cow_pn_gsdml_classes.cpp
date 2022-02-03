#include "cow_pn_gsdml_classes.h"
#include "cow_pn_gsdml.h"
#include "rt_pb_msg.h"
#include "pwr.h"

#include <cstring>
#include <algorithm>
#include <iomanip>
#include <arpa/inet.h>

namespace GSDML
{



// Do we need this?

// int string_to_value(GSDML::eType type, unsigned int size,
//                               const char* str, void* buf)
// {
//   switch (type)
//   {
//   case gsdml_eType_Integer:
//     sscanf(str, "%d", (int*)buf);
//     break;
//   case gsdml_eType_Boolean:
//     if (streq(str, "true"))
//       *(gsdml_tBoolean*)buf = 1;
//     else
//       *(gsdml_tBoolean*)buf = 0;
//     break;
//   case gsdml_eType_Unsigned8:
//     sscanf(str, "%hhu", (unsigned char*)buf);
//     break;
//   case gsdml_eType_Unsigned16:
//     sscanf(str, "%hu", (unsigned short*)buf);
//     break;
//   case gsdml_eType_Unsigned32:
//     sscanf(str, "%u", (unsigned int*)buf);
//     break;
//   case gsdml_eType_Unsigned16hex:
//     sscanf(str, "%hx", (unsigned short*)buf);
//     break;
//   case gsdml_eType_Unsigned32hex:
//     sscanf(str, "%x", (unsigned int*)buf);
//     break;
//   case gsdml_eType_String:
//   case gsdml_eType_NormalizedString:
//   case gsdml_eType_Token:
//   case gsdml_eType_TokenList:
//   case gsdml_eType_ValueList:
//   case gsdml_eType_SignedOrFloatValueList:
//   case gsdml_eType_Id:
//   case gsdml_eType_IdT:
//   case gsdml_eType_Enum:
//     if (strlen(str) >= size)
//     {
//       error_message_line("Attribute size to small, value cut off");
//     }
//     strncpy((char*)buf, str, size);
//     break;
//   case gsdml_eType_RefId:
//     if (strlen(str) >= size)
//     {
//       error_message_line("Attribute size to small, value cut off");
//     }
//     strncpy(((gsdml_tRefId*)buf)->ref, str, size);
//     break;
//   case gsdml_eType_RefIdT:
//     if (strlen(str) >= size)
//     {
//       error_message_line("Attribute size to small, value cut off");
//     }
//     strncpy(((gsdml_tRefIdT*)buf)->ref, str, size);
//     break;
//   case gsdml_eType_AllocatedString:
//     *(gsdml_tAllocatedString*)buf = (char*)malloc(strlen(str) + 1);
//     strcpy(*(char**)buf, str);
//     break;
//   case gsdml_eType_:
//   case gsdml_eType__:
//     break;
//   }
//   return 1;
// }





//
// Get the length of an IO type specified in a data item
//
int get_datavalue_length(GSDML::eValueDataType datatype, int strlength,
                                   unsigned int* len)
{
  switch (datatype)
  {
  case GSDML::ValueDataType_Integer8:
  case GSDML::ValueDataType_Unsigned8:
    *len = 1;
    break;
  case GSDML::ValueDataType_Integer16:
  case GSDML::ValueDataType_Unsigned16:
    *len = 2;
    break;
  case GSDML::ValueDataType_Integer32:
  case GSDML::ValueDataType_Unsigned32:
  case GSDML::ValueDataType_Float32:
    *len = 4;
    break;
  case GSDML::ValueDataType_Integer64:
  case GSDML::ValueDataType_Unsigned64:
  case GSDML::ValueDataType_Float64:
    *len = 8;
    break;
  case GSDML::ValueDataType_VisibleString:
  case GSDML::ValueDataType_OctetString:
    *len = strlength; // TODO check if we can create data for this
    break;
  case GSDML::ValueDataType_Date:
  case GSDML::ValueDataType_TimeOfDayWithDate:
  case GSDML::ValueDataType_TimeOfDayWithoutDate:
  case GSDML::ValueDataType_TimeDiffWithDate:
  case GSDML::ValueDataType_TimeDiffWithoutDate:
  case GSDML::ValueDataType_NetworkTime:
  case GSDML::ValueDataType_NetworkTimeDiff:
    // TODO
    return PB__NYI;
  default:
    return PB__NYI;
  }
  return PB__SUCCESS;
}

// int datavalue_to_string(eValueDataType datatype, void* value,
//                                   unsigned int size, char* str,
//                                   unsigned int strsize)
// {
//   switch (datatype)
//   {
//   case GSDML::ValueDataType_Integer8:
//     snprintf(str, strsize, "%hhd", *(char*)value);
//     break;
//   case GSDML::ValueDataType_Unsigned8:
//     snprintf(str, strsize, "%hhu", *(unsigned char*)value);
//     break;
//   case GSDML::ValueDataType_Integer16:
//   {
//     short v;

//     memcpy(&v, value, sizeof(v));

//     snprintf(str, strsize, "%hd", v);

//     break;
//   }
//   case GSDML::ValueDataType_Unsigned16:
//   {
//     unsigned short v;

//     memcpy(&v, value, sizeof(v));

//     snprintf(str, strsize, "%hu", v);

//     break;
//   }
//   case GSDML::ValueDataType_Integer32:
//   {
//     int v;

//     memcpy(&v, value, sizeof(v));

//     snprintf(str, strsize, "%d", v);

//     break;
//   }
//   case GSDML::ValueDataType_Unsigned32:
//   {
//     unsigned int v;
//     v = ntohl(*(pwr_tUInt32*)value);
//     //memcpy(&v, value, sizeof(v));

//     snprintf(str, strsize, "%u", v);
//     break;
//   }
//   case GSDML::ValueDataType_Float32:
//     snprintf(str, strsize, "%g", *(float*)value);
//     break;
//   case GSDML::ValueDataType_Integer64:
//   {
//     pwr_tInt64 v;
// #if (pwr_dHost_byteOrder == pwr_dLittleEndian)
//     memcpy(&v, value, sizeof(v));
// #elif (pwr_dHost_byteOrder == pwr_dBigEndian)
//     memcpy(&v, &data[item[i].ref->Reference_Offset], sizeof(v));
// #endif
//     snprintf(str, strsize, pwr_dFormatInt64, v);
//     break;
//   }
//   case GSDML::ValueDataType_Unsigned64:
//   {
//     pwr_tUInt64 v;
// #if (pwr_dHost_byteOrder == pwr_dLittleEndian)
//     memcpy(&v, value, sizeof(v));
// #elif (pwr_dHost_byteOrder == pwr_dBigEndian)
//     memcpy(&v, &data[item[i].ref->Reference_Offset], sizeof(v));
// #endif
//     snprintf(str, strsize, pwr_dFormatUInt64, v);
//     break;
//   }
//   case GSDML::ValueDataType_Float64:
//     snprintf(str, strsize, "%lg", *(double*)value);
//     break;
//   case GSDML::ValueDataType_VisibleString:
//     strncpy(str, (char*)value, strsize);
//     break;
//   case GSDML::ValueDataType_OctetString:
//   {
//     unsigned int len = 0;
//     for (unsigned int i = 0; i < size; i++)
//     {
//       if (i == size - 1)
//       {
//         if (len + 4 >= strsize)
//           break;
//         len += sprintf(&str[i * 5], "0x%02hhx", *(((unsigned char*)value) + i));
//       }
//       else
//       {
//         if (len + 5 >= strsize)
//           break;
//         len +=
//             sprintf(&str[i * 5], "0x%02hhx,", *(((unsigned char*)value) + i));
//       }
//     }
//     break;
//   }
//   case GSDML::ValueDataType_Date:
//   case GSDML::ValueDataType_TimeOfDayWithDate:
//   case GSDML::ValueDataType_TimeOfDayWithoutDate:
//   case GSDML::ValueDataType_TimeDiffWithDate:
//   case GSDML::ValueDataType_TimeDiffWithoutDate:
//   case GSDML::ValueDataType_NetworkTime:
//   case GSDML::ValueDataType_NetworkTimeDiff:
//     sprintf(str, "Not implemented data type");
//     return 0;
//   default:
//     return 0;
//   }
//   return 1;
// }

// int string_to_datavalue(GSDML::eValueDataType datatype, void* value, unsigned int size, const char* str)
// {
//   switch (datatype)
//   {
//   case GSDML::ValueDataType_Integer8:
//     if (sscanf(str, "%hhd", (char*)value) != 1)
//       return PB__SYNTAX;
//     break;
//   case GSDML::ValueDataType_Unsigned8:
//     if (sscanf(str, "%hhu", (unsigned char*)value) != 1)
//       return PB__SYNTAX;
//     break;
//   case GSDML::ValueDataType_Integer16:
//   {
//     short v;
//     if (sscanf(str, "%hd", &v) != 1)
//       return PB__SYNTAX;

//     memcpy(value, &v, sizeof(v));

//     unsigned char b[2];
//     memcpy(b, &v, sizeof(b));
//     break;
//   }
//   case GSDML::ValueDataType_Unsigned16:
//   {
//     unsigned short v;
//     if (sscanf(str, "%hu", &v) != 1)
//       return PB__SYNTAX;

//     memcpy(value, &v, sizeof(v));

//     unsigned char b[2];
//     memcpy(b, &v, sizeof(b));
//     break;
//   }
//   case GSDML::ValueDataType_Integer32:
//   {
//     int v;
//     if (sscanf(str, "%d", &v) != 1)
//       return PB__SYNTAX;

//     memcpy(value, &v, sizeof(v));

//     unsigned char b[4];
//     memcpy(b, &v, sizeof(b));
//     break;
//   }
//   case GSDML::ValueDataType_Unsigned32:
//   {
//     unsigned int v;
//     std::istringstream input(str, std::ios_base::in);
//     // if (sscanf(str, "%u", &v) != 1)
//     //   return PB__SYNTAX;
//     input >> v;
//     v = htonl(v);
//     memcpy(value, &v, sizeof(v));
//     break;
//   }
//   case GSDML::ValueDataType_Float32:
//   {
//     if (sscanf(str, "%g", (float*)value) != 1)
//       return PB__SYNTAX;

//     float* tmp = (float*)value;

//     unsigned char b[4];
//     memcpy(b, tmp, sizeof(b));
//     break;
//   }
//   case GSDML::ValueDataType_Integer64:
//   {
//     pwr_tInt64 v;
//     if (sscanf(str, pwr_dFormatInt64, &v) != 1)
//       return PB__SYNTAX;

//     memcpy(value, &v, sizeof(v));

//     unsigned char b[8];
//     memcpy(b, &v, sizeof(b));
//     break;
//   }
//   case GSDML::ValueDataType_Unsigned64:
//   {
//     pwr_tUInt64 v;
//     if (sscanf(str, pwr_dFormatUInt64, &v) != 1)
//       return PB__SYNTAX;

//     memcpy(value, &v, sizeof(v));

//     unsigned char b[8];
//     memcpy(b, &v, sizeof(b));
//     break;
//   }
//   case GSDML::ValueDataType_Float64:
//   {
//     if (sscanf(str, "%lg", (double*)value) != 1)
//       return PB__SYNTAX;

//     unsigned char b[8];
//     memcpy(b, &value, sizeof(b));
//     break;
//   }
//   case GSDML::ValueDataType_VisibleString:
//     strncpy((char*)value, str, size);
//     break;
//   case GSDML::ValueDataType_OctetString:
//   {
//     unsigned int len;
//     for (unsigned int i = 0; i < size; i++)
//     {
//       len = sscanf(&str[i * 5], "0x%2hhx",
//                    (unsigned char*)((unsigned char*)value + i));
//       if (len != 1)
//         break;
//     }
//     break;
//   }
//   case GSDML::ValueDataType_Date:
//   case GSDML::ValueDataType_TimeOfDayWithDate:
//   case GSDML::ValueDataType_TimeOfDayWithoutDate:
//   case GSDML::ValueDataType_TimeDiffWithDate:
//   case GSDML::ValueDataType_TimeDiffWithoutDate:
//   case GSDML::ValueDataType_NetworkTime:
//   case GSDML::ValueDataType_NetworkTimeDiff:
//     return PB__NYI;
//   default:
//     return PB__NYI;
//   }
//   return 1;
// }

int string_to_value_datatype(char const* str, eValueDataType* type)
{
  if (strcmp(str, "Bit") == 0)
    *type = ValueDataType_Bit;
  else if (strcmp(str, "BitArea") == 0)
    *type = ValueDataType_BitArea;
  else if (strcmp(str, "Integer8") == 0)
    *type = ValueDataType_Integer8;
  else if (strcmp(str, "Integer16") == 0)
    *type = ValueDataType_Integer16;
  else if (strcmp(str, "Integer32") == 0)
    *type = ValueDataType_Integer32;
  else if (strcmp(str, "Integer64") == 0)
    *type = ValueDataType_Integer64;
  else if (strcmp(str, "Unsigned8") == 0)
    *type = ValueDataType_Unsigned8;
  else if (strcmp(str, "Unsigned16") == 0)
    *type = ValueDataType_Unsigned16;
  else if (strcmp(str, "Unsigned32") == 0)
    *type = ValueDataType_Unsigned32;
  else if (strcmp(str, "Unsigned64") == 0)
    *type = ValueDataType_Unsigned64;
  else if (strcmp(str, "Float32") == 0)
    *type = ValueDataType_Float32;
  else if (strcmp(str, "Float64") == 0)
    *type = ValueDataType_Float64;
  else if (strcmp(str, "Date") == 0)
    *type = ValueDataType_Date;
  else if (strcmp(str, "TimeOfDay with date indication") == 0)
    *type = ValueDataType_TimeOfDayWithDate;
  else if (strcmp(str, "TimeOfDay without date indication") == 0)
    *type = ValueDataType_TimeOfDayWithoutDate;
  else if (strcmp(str, "TimeDifference with date indication") == 0)
    *type = ValueDataType_TimeDiffWithDate;
  else if (strcmp(str, "TimeDifference without date indication") == 0)
    *type = ValueDataType_TimeDiffWithoutDate;
  else if (strcmp(str, "NetworkTime") == 0)
    *type = ValueDataType_NetworkTime;
  else if (strcmp(str, "NetworkTimeDifference") == 0)
    *type = ValueDataType_NetworkTimeDiff;
  else if (strcmp(str, "VisibleString") == 0)
    *type = ValueDataType_VisibleString;
  else if (strcmp(str, "OctetString") == 0)
    *type = ValueDataType_OctetString;
  else
  {
    *type = ValueDataType_;
    return 0;
  }
  return 1;
}

TokenList::TokenList(char const* input)
{
   // Lets parse it...
    std::istringstream buf(input, std::ios_base::in);

    std::string token;    
    while (std::getline(buf, token, ';'))
    {
      tokenItems.push_back(token);
    }
}

bool TokenList::inList(std::string const& value) const
{
  auto token = std::find(tokenItems.begin(), tokenItems.end(), value);
  return token != tokenItems.end();
}

void TokenList::print()
{
  for (auto& value : tokenItems)      
        std::cout << value << std::endl;
}

std::vector<std::string>& TokenList::getList()
{
  return tokenItems;
}

Const::Const(pugi::xml_node&& xmlNode, Node* parent, pn_gsdml* gsdml)
    : _ByteOffset(xmlNode.attribute("ByteOffset").as_uint())
{
  setParent(parent);
  // Lets parse it...
  std::istringstream buf(xmlNode.attribute("Data").as_string(),
                         std::ios_base::in);
  // <Const Data="0x01,0x00" ByteOffset="0" />
	// <Const Data="0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00" ByteOffset="2" />  
  // [ 0] [ 1] [ 2] [ 3] [ 4] [ 5] [ 6] [ 7] [ 8] [ 9] [10] [11] [12] [13] [14] [15]
  //                                                                       0x00 0x01
  std::string value;
  unsigned int byte;
// #if (pwr_dHost_byteOrder == pwr_dLittleEndian)  
//   _ByteOffset = static_cast<ParameterRecordDataItem*>(m_Parent)->_Length - 1 - _ByteOffset;
// #endif          
  size_t pos = 0;
  //unsigned char* data = static_cast<ParameterRecordDataItem*>(m_Parent)->getData();
  //unsigned char* constData = (data + _ByteOffset);
  while (getline(buf, value, ','))
  {
    std::istringstream val(value, std::ios_base::in);
    val.seekg(2); // Skip the 0x part
    val >> std::hex >> byte;

    // The data we read is network byte ordered so for little endian machines we add the data in the reveresed order
#if (pwr_dHost_byteOrder == pwr_dLittleEndian)
    //*(constData - pos) = byte;
    //_data.push_back((constData - pos++));
    _data.insert(_data.begin(), byte);
#else
    //*(constData + pos) = byte;
    //_data.insert(_data.begin(), (constData + pos++));
    _data.push_back(byte);    
#endif        
  }
}

ModuleInfo::ModuleInfo(pugi::xml_node&& moduleInfo, pn_gsdml* gsdml)
    : _Name(gsdml->_get_TextId(
          moduleInfo.child("Name").attribute("TextId").as_string())),
      _InfoText(gsdml->_get_TextId(
          moduleInfo.child("InfoText").attribute("TextId").as_string())),
      _VendorName(
          moduleInfo.child("VendorName").attribute("Value").as_string()),
      _OrderNumber(
          moduleInfo.child("OrderNumber").attribute("Value").as_string()),
      _SoftwareRelease(
          moduleInfo.child("SoftwareRelease").attribute("Value").as_string()),
      _HardwareRelease(
          moduleInfo.child("HardwareRelease").attribute("Value").as_string()),
      _CategoryItemText(gsdml->_get_CategoryTextRef(
          moduleInfo.attribute("CategoryRef").value())),
      _CategoryItemInfoText(gsdml->_get_CategoryInfoTextRef(
          moduleInfo.attribute("CategoryRef").value())),
      _SubCategoryItemText(gsdml->_get_CategoryTextRef(
          moduleInfo.attribute("SubCategory1Ref").value())),
      _SubCategoryItemInfoText(gsdml->_get_CategoryInfoTextRef(
          moduleInfo.attribute("SubCategory1Ref").value()))
{
}

BitDataItem::BitDataItem(pugi::xml_node&& bitDataItem, pn_gsdml* gsdml)
    : _BitOffset(bitDataItem.attribute("BitOffset").as_uint()),
      _Text(gsdml->_get_TextId(bitDataItem.attribute("TextId").value()))
{
}

DataItem::DataItem(pugi::xml_node&& dataItem, pn_gsdml* gsdml)
    : _Id(dataItem.attribute("Id").as_string()),
      _Length(dataItem.attribute("Lendth").as_uint()),
      _UseAsBits(dataItem.attribute("UseAsBits").as_bool()),
      _Text(gsdml->_get_TextId(
          std::move(std::string(dataItem.attribute("TextId").value()))))
{
  string_to_value_datatype(dataItem.attribute("DataType").as_string(),
                           &_DataType);
  for (pugi::xml_node& bitDataItem : dataItem.children("BitDataItem"))
  {
    _BitDataItem.push_back(BitDataItem(std::move(bitDataItem), gsdml));
  }
}

Input::Input(pugi::xml_node&& input, pn_gsdml* gsdml)
{
  for (pugi::xml_node& dataItem : input.children("DataItem"))
  {
    _DataItem.push_back(DataItem(std::move(dataItem), gsdml));
  }
}

Output::Output(pugi::xml_node&& output, pn_gsdml* gsdml)
{
  for (pugi::xml_node& dataItem : output.children("DataItem"))
  {
    _DataItem.push_back(DataItem(std::move(dataItem), gsdml));
  }
}

IOData::IOData(pugi::xml_node&& ioData, pn_gsdml* gsdml)
    : _Input(ioData.child("Input"), gsdml),
      _Output(ioData.child("Output"), gsdml)
{
}

Assign::Assign(pugi::xml_node&& assign, pn_gsdml* gsdml)
    : _Content(assign.attribute("Content").as_int()),
      _Text(gsdml->_get_TextId(
          std::move(std::string(assign.attribute("TextId").as_string()))))
{
}

MenuItem::MenuItem(pugi::xml_node&& menuItem, Node* parent,
                   pn_gsdml* gsdml)
    : _Name(gsdml->_get_TextId(
          menuItem.child("Name").attribute("TextId").as_string()))
{
  setParent(parent);
  for (pugi::xml_node& parameterRef : menuItem.children("ParameterRef"))
  {    
    ParameterRecordDataItem* prdi = static_cast<ParameterRecordDataItem*>(m_Parent);
    _ParameterRefs.push_back(prdi->_Ref[std::move(
        std::string(parameterRef.attribute("ParameterTarget").as_string()))]);
    // _ParameterRefs.push_back(refs[std::move(
    //     std::string(parameterRef.attribute("ParameterTarget").as_string()))]);
  }
}

void MenuItem::linkMenuRefs(
    pugi::xml_node& menuItem,
    std::unordered_map<std::string, std::shared_ptr<MenuItem>>& menuList)
{
  for (pugi::xml_node& menuRef : menuItem.children("MenuRef"))
  {
    _MenuRefs.push_back(
        menuList[std::string(menuRef.attribute("MenuTarget").as_string())]);
  }
}

Ref::Ref(pugi::xml_node&& ref, Node* parent, pn_gsdml* gsdml)
    : //_DataType(ref.attribute("DataType").as_string()),
      _ByteOffset(ref.attribute("ByteOffset").as_uint()),
      _BitOffset(ref.attribute("BitOffset").as_uint()),
      _BitLength(ref.attribute("BitLength").as_uint()),
      _DefaultValue(ref.attribute("DefaultValue").as_float()),
      _AllowedValues(
          ValueList<float>(ref.attribute("AllowedValues").as_string())),
      _Length(ref.attribute("Length").as_uint()),
      _Changeable(ref.attribute("Changeable").empty() ? true : ref.attribute("Changeable").as_bool()), // This defaults to true if missing according to ISO 15745-4:2003/Amd.1:2006(E)
      _Visible(ref.attribute("Visible").empty() ? true : ref.attribute("Visible").as_bool()), // This defaults to true if missing according to ISO 15745-4:2003/Amd.1:2006(E)
      _Text(gsdml->_get_TextId(ref.attribute("TextId").value()))
{
  setParent(parent);
  string_to_value_datatype(ref.attribute("DataType").as_string(), &_DataType);
  if (ref.attribute("ValueItemTarget"))
  {
    _ValueItem = gsdml->getValueMap()[std::move(
        std::string(ref.attribute("ValueItemTarget").as_string()))];
  }


  // Test iterator TODO REMOVE :D
  // if (!_AllowedValues.empty())
  // {
  //   std::cout << "NEW ALLOWED VALUES:" << std::endl;
  //   for (auto it = _AllowedValues.begin(); it != _AllowedValues.end(); ++it)
  //     std::cout << it.value() << std::endl;
  //   std::cout << "END" << std::endl << std::endl << std::endl;
  // }
}

ParameterRecordDataItem::ParameterRecordDataItem(
    pugi::xml_node&& parameterRecordDataItem, pn_gsdml* gsdml)
    : _Index(parameterRecordDataItem.attribute("Index").as_uint()),
      _Length(parameterRecordDataItem.attribute("Length").as_uint()),
      _TransferSequence(
          parameterRecordDataItem.attribute("TransferSequence").as_uint()),
      _Name(gsdml->_get_TextId(
          parameterRecordDataItem.child("Name").attribute("TextId").value()))
{
  _data = new unsigned char[_Length]();

  for (pugi::xml_node& iConst : parameterRecordDataItem.children("Const"))
  {
    _Const.push_back(GSDML::Const(std::move(iConst), this, gsdml));
  }

  // populated our data
  size_t index = 0;
  for (auto it = _Const.begin(); it != _Const.end(); ++it)  
  {    
    for (auto const& data : it->_data)
    {
      _data[index++] = data;
    }    
  }

  for (pugi::xml_node& ref : parameterRecordDataItem.children("Ref"))
  {
    std::string ID;
    
    // Index using either ValueItemTarget, ID or TextID, order of preference ID > ValueItemTarget > TextId
    if (!ref.attribute("ID").empty())
    {
      ID = ref.attribute("ID").as_string();
    }
    else if (!ref.attribute("ValueItemTarget").empty())
    {
      ID = ref.attribute("ValueItemTarget").as_string();
    }
    else
    {
      ID = ref.attribute("TextId").as_string();
    }

    _Ref.emplace(
        std::make_pair(std::move(ID),
                       std::make_shared<Ref>(Ref(std::move(ref), this, gsdml))));
  }

  // The menuitems need to be parsed "breadth first" since MenuRef's would
  // otherwise reference not already "compiled" menus... We can parse everything
  // except the MenuRef's...
  for (pugi::xml_node& menuItem :
       parameterRecordDataItem.child("MenuList").children("MenuItem"))
  {
    std::shared_ptr<MenuItem> mi(
        std::make_shared<MenuItem>(std::move(menuItem), this, gsdml));
    _MenuLookupList.emplace(std::make_pair(
        std::move(std::string(menuItem.attribute("ID").as_string())), mi));
    _MenuList.push_back(mi);
  }

  // Now link the MenuRefs
  for (pugi::xml_node& menuItem :
       parameterRecordDataItem.child("MenuList").children("MenuItem"))
  {
    _MenuLookupList[menuItem.attribute("ID").as_string()]->linkMenuRefs(
        menuItem, _MenuLookupList);
  }
}

ParameterRecordDataItem::ParameterRecordDataItem(ParameterRecordDataItem&& o)
    : _Index(o._Index), _Length(o._Length),
      _TransferSequence(o._TransferSequence), _Name(std::move(o._Name)),
      _Const(std::move(o._Const)), _Ref(std::move(o._Ref)),
      _MenuLookupList(std::move(o._MenuLookupList)),
      _MenuList(std::move(o._MenuList)), _data(o._data)
{
  o._data = nullptr;
}

unsigned char* ParameterRecordDataItem::getData() const { return _data; }

ParameterRecordDataItem::~ParameterRecordDataItem()
{
  if (_data)
  {
    delete[] _data;
    _data = nullptr;
  }
}

SubslotItem::SubslotItem(pugi::xml_node&& SubslotItem, pn_gsdml* gsdml)
    : _SubslotNumber(SubslotItem.attribute("SubslotNumber").as_int()),
      _Text(gsdml->_get_TextId(SubslotItem.attribute("TextId").value()))
{
}

ValueItem::ValueItem(pugi::xml_node&& valueItem, pn_gsdml* gsdml)
    : _Help(gsdml->_get_TextId(valueItem.child("Help").attribute("TextId").as_string()))
{
  for (pugi::xml_node& assign :
       valueItem.child("Assignments").children("Assign"))
    _Assignments.push_back(Assign(std::move(assign), gsdml));
}

CertificationInfo::CertificationInfo(pugi::xml_node&& CertificationInfo,
                                     pn_gsdml* gsdml)
    : _ConformanceClass(
          CertificationInfo.attribute("ConformanceClass").value()),
      _ApplicationClass(
          CertificationInfo.attribute("ApplicationClass").value()),
      _NetloadClass(CertificationInfo.attribute("NetloadClass").value())
{
}

IOConfigData::IOConfigData(pugi::xml_node&& IOConfigData, pn_gsdml* gsdml)
    : _MaxApplicationInputLength(
          IOConfigData.attribute("MaxApplicationInputLength").as_int()),
      _MaxApplicationOutputLength(
          IOConfigData.attribute("MaxApplicationOutputLength").as_int()),
      _MaxApplicationDataLength(
          IOConfigData.attribute("MaxApplicationDataLength").as_int()),
      _MaxInputLength(IOConfigData.attribute("MaxInputLength").as_int()),
      _MaxOutputLength(IOConfigData.attribute("MaxOutputLength").as_int()),
      _MaxDataLength(IOConfigData.attribute("MaxDataLength").as_int()),
      _ApplicationLengthIncludesIOxS(
          IOConfigData.attribute("ApplicationLengthIncludesIOxS").as_bool())
{
}

MAUTypeItem::MAUTypeItem(pugi::xml_node&& mauTypeItem, pn_gsdml* gsdml)
  : _Value(mauTypeItem.attribute("Value").as_uint()),
    _AdjustSupported(mauTypeItem.attribute("AdjustSupported").as_bool())
{
}

RT_Class3Properties::RT_Class3Properties(pugi::xml_node&& rt_Class3Properties, pn_gsdml* gsdml)
  : _StartupMode(rt_Class3Properties.attribute("StartupMode").as_string()),
    _ForwardingMode(rt_Class3Properties.attribute("ForwardingMode").as_string()),
    _MaxBridgeDelay(rt_Class3Properties.attribute("MaxBridgeDelay").as_uint()),
    _MaxNumberIR_FrameData(rt_Class3Properties.attribute("MaxNumberIR_FrameData").as_uint())
{
}

SynchronisationMode::SynchronisationMode(pugi::xml_node&& synchronisationMode, pn_gsdml* gsdml)
  : _SupportedRole(synchronisationMode.attribute("SupportedRole").as_string()),
    _MaxLocalJitter(synchronisationMode.attribute("MaxLocalJitter").as_uint()),
    _T_PLL_MAX(synchronisationMode.attribute("T_PLL_MAX").as_uint()),
    _SupportedSyncProtocols(synchronisationMode.attribute("SupportedSyncProtocols").as_string())
{
}

TimingProperties::TimingProperties(pugi::xml_node&& timingProperties, pn_gsdml* gsdml)
  : _SendClock(timingProperties.attribute("SendClock").as_string()),
    _ReductionRatioPow2(timingProperties.attribute("ReductionRatioPow2").as_string())
{
}

RT_Class3TimingProperties::RT_Class3TimingProperties(pugi::xml_node&& rt_Class3TimingProperties, pn_gsdml* gsdml)
  : _SendClock(rt_Class3TimingProperties.attribute("SendClock").as_string()),
    _ReductionRatioPow2(rt_Class3TimingProperties.attribute("ReductionRatioPow2").as_string())
{
}

MediaRedundancy::MediaRedundancy(pugi::xml_node&& mediaRedundancy, pn_gsdml* gsdml)
  : _SupportedRole(mediaRedundancy.attribute("SupportedRole").as_string()),
    _AdditionalProtocolsSupported(mediaRedundancy.attribute("AdditionalProtocolsSupported").as_bool())
{
}

ApplicationRelations::ApplicationRelations(pugi::xml_node&& applicationRelations, pn_gsdml* gsdml)
  : _NumberOfAR(applicationRelations.attribute("NumberOfAR").as_uint()),
    _StartupMode(applicationRelations.attribute("StartupMode").as_string()),
    _TimingProperties(applicationRelations.child("TimingProperties"), gsdml),
    _RT_Class3TimingProperties(applicationRelations.child("RT_Class3TimingProperties"), gsdml)
{
}

SubmoduleItem::SubmoduleItem(
    pugi::xml_node&& submoduleItem, pn_gsdml* gsdml, eSubmoduleItemType submoduleType)
    : _SubmoduleIdentNumber(
          submoduleItem.attribute("SubmoduleIdentNumber").as_uint()),
      _SubslotNumber(submoduleItem.attribute("SubslotNumber").as_uint()),
      _FixedInSubslots(
          submoduleItem.attribute("FixedInSubslots").as_string()),
      _Writeable_IM_Records(
          submoduleItem.attribute("Writeable_IM_Records").as_string()),
      _MayIssueProcessAlarm(
          submoduleItem.attribute("MayIssueProcessAlarm").as_bool()),
      _Text(gsdml->_get_TextId(submoduleItem.attribute("TextId").value())),
      _ID(submoduleItem.attribute("ID").as_string()),
      _IOData(submoduleItem.child("IOData"), gsdml),
      _ModuleInfo(submoduleItem.child("ModuleInfo"), gsdml),
      _SubmoduleItemType(submoduleType)
{
  //std::cout << "In module: " << *_ModuleInfo._Name << std::endl;
  for (pugi::xml_node& parameterRecordDataItem : submoduleItem.child("RecordDataList").children("ParameterRecordDataItem"))
    _RecordDataList.push_back(ParameterRecordDataItem(std::move(parameterRecordDataItem), gsdml));
}

InterfaceSubmoduleItem::InterfaceSubmoduleItem(pugi::xml_node&& interfaceSubmoduleItem, pn_gsdml* gsdml)
  : SubmoduleItem(std::move(interfaceSubmoduleItem), gsdml, GSDML::SubmoduleItemType_Interface),
    _SupportedRT_Classes(interfaceSubmoduleItem.attribute("SupportedRT_Classes").as_string()),
    _SupportedProtocols(interfaceSubmoduleItem.attribute("SupportedProtocols").as_string()),
    _DCP_HelloSupported(interfaceSubmoduleItem.attribute("DCP_HelloSupported").as_bool()),
    _DelayMeasurementSupported(interfaceSubmoduleItem.attribute("DelayMeasurementSupported").as_bool()),
    _RT_Class3Properties(interfaceSubmoduleItem.child("RT_Class3Properties"), gsdml),
    _SynchronisationMode(interfaceSubmoduleItem.child("SynchronisationMode"), gsdml),
    _ApplicationRelations(interfaceSubmoduleItem.child("ApplicationRelations"), gsdml),
    _MediaRedundancy(interfaceSubmoduleItem.child("MediaRedundancy"), gsdml)
{
}

PortSubmoduleItem::PortSubmoduleItem(
    pugi::xml_node&& portSubmoduleItem, pn_gsdml* gsdml)
    : SubmoduleItem(std::move(portSubmoduleItem), gsdml, GSDML::SubmoduleItemType_Port),
      _MaxPortRxDelay(portSubmoduleItem.attribute("MaxPortRxDelay").as_uint()),
      _MaxPortTxDelay(portSubmoduleItem.attribute("MaxPortTxDelay").as_uint()),
      _LinkStateDiagnosisCapability(portSubmoduleItem.attribute("LinkStateDiagnosisCapability").as_string()),
      _PortDeactivationSupported(portSubmoduleItem.attribute("PortDeactivationSupported").as_bool()),
      _MAUTypes(portSubmoduleItem.attribute("MAUTypes").as_uint()),
      _SupportsRingportConfig(portSubmoduleItem.attribute("SupportsRingportConfig").as_bool()),
      _IsDefaultRingport(portSubmoduleItem.attribute("IsDefaultRingport").as_bool()),
      _Writeable_IM_Records(portSubmoduleItem.attribute("Writeable_IM_Records").as_string()),
      _CheckMAUTypeSupported(portSubmoduleItem.attribute("CheckMAUTypeSupported").as_bool()),
      _FiberOpticTypes(portSubmoduleItem.attribute("_FiberOpticTypes").as_string()),
      _PowerBudgetControlSupported(portSubmoduleItem.attribute("PowerBudgetControlSupported").as_bool()),
      _ShortPreamble100MBitSupported(portSubmoduleItem.attribute("ShortPreamble100MBitSupported").as_bool()),
      _CheckMAUTypeDifferenceSupported(portSubmoduleItem.attribute("CheckMAUTypeDifferenceSupported").as_bool())      
{  
  for (pugi::xml_node& mauTypeItem : portSubmoduleItem.child("MAUTypeList").children("MAUTypeItem"))
  {
    _MAUTypeList.push_back(MAUTypeItem(std::move(mauTypeItem), gsdml));
  }
}

VirtualSubmoduleItem::VirtualSubmoduleItem(
    pugi::xml_node&& virtualSubmoduleItem, pn_gsdml* gsdml)
    : SubmoduleItem(std::move(virtualSubmoduleItem), gsdml, GSDML::SubmoduleItemType_Virtual)
{  
}

ModuleItem::ModuleItem(pugi::xml_node&& moduleItem, pn_gsdml* gsdml, eModuleItemType moduleType)
    : _ModuleIdentNumber(moduleItem.attribute("ModuleIdentNumber").as_uint()),
      _PhysicalSubslots(moduleItem.attribute("PhysicalSubslots").as_string()),
      _ID(moduleItem.attribute("ID").as_string()),
      _ModuleInfo(moduleItem.child("ModuleInfo"), gsdml),
      _ModuleItemType(moduleType)
{
  for (pugi::xml_node& virtualSubmoduleItem :
       moduleItem.child("VirtualSubmoduleList")
           .children("VirtualSubmoduleItem"))
  {
    std::string ID = virtualSubmoduleItem.attribute("ID").value();
    _VirtualSubmoduleList[ID].reset(
        new VirtualSubmoduleItem(std::move(virtualSubmoduleItem), gsdml));
  }

  for (pugi::xml_node& submoduleItemRef :
       moduleItem.child("UseableSubmodules")
           .children("SubmoduleItemRef"))
  {
    std::string submoduleItemTarget = submoduleItemRef.attribute("SubmoduleItemTarget").value();    
    _UseableSubmodules.emplace(std::make_pair(std::move(submoduleItemTarget), std::make_shared<SubmoduleItemRef>(std::move(submoduleItemRef), gsdml)));    
  }
}

ModuleItemRef::ModuleItemRef(pugi::xml_node&& moduleItemRef, pn_gsdml* gsdml)
    : _ModuleItemTarget(gsdml->getModuleMap()[std::move(std::string(
          moduleItemRef.attribute("ModuleItemTarget").as_string()))]),
      _AllowedInSlots(moduleItemRef.attribute("AllowedInSlots").as_string())
{
}

SubmoduleItemRef::SubmoduleItemRef(pugi::xml_node&& submoduleItemRef, pn_gsdml* gsdml)
    : _SubmoduleItemTarget(gsdml->getSubmoduleMap()[std::move(std::string(submoduleItemRef.attribute("SubmoduleItemTarget").as_string()))]),
      _AllowedInSubslots(submoduleItemRef.attribute("AllowedInSubslots").as_string()),
      _UsedInSubslots(submoduleItemRef.attribute("UsedInSubslots").as_string()),
      _FixedInSubslots(submoduleItemRef.attribute("FixedInSubslots").as_string())
{
}

DeviceIdentity::DeviceIdentity(pugi::xml_node&& deviceIdentity, pn_gsdml* gsdml)
  : _VendorID(deviceIdentity.attribute("VendorID").as_uint()),
    _DeviceID(deviceIdentity.attribute("DeviceID").as_uint()),
    _InfoText(gsdml->_get_TextId(deviceIdentity.child("InfoText").attribute("TextId").value())),
    _VendorName(deviceIdentity.child("VendorName").attribute("Value").as_string())
{  
}

DeviceFunction::DeviceFunction(pugi::xml_node&& deviceFunction, pn_gsdml* gsdml)
  : _MainFamily(deviceFunction.child("Family").attribute("MainFamily").as_string()),
    _ProductFamily(deviceFunction.child("Family").attribute("ProductFamily").as_string())
{
}

DeviceAccessPointItem::DeviceAccessPointItem(
    pugi::xml_node&& deviceAccessPointItem, pn_gsdml* gsdml)
    : ModuleItem(std::forward<pugi::xml_node>(deviceAccessPointItem), gsdml, GSDML::ModuleItemType_DAP),
      _PNIO_Version(
          deviceAccessPointItem.attribute("PNIO_Version").as_string()),
      _RequiredSchemaVersion(
          deviceAccessPointItem.attribute("RequiredSchemaVersion").as_string()),
      _PhysicalSlots(
          deviceAccessPointItem.attribute("PhysicalSlots").as_string()),
      // _ModuleIdentNumber(
      //     deviceAccessPointItem.attribute("ModuleIdentNumber").as_uint()),
      _MinDeviceInterval(
          deviceAccessPointItem.attribute("MinDeviceInterval").as_int()),
      _ImplementationType(
          deviceAccessPointItem.attribute("ImplementationType").as_string()),
      _DNS_CompatibleName(
          deviceAccessPointItem.attribute("DNS_CompatibleName").as_string()),
      _FixedInSlots(
          deviceAccessPointItem.attribute("FixedInSlots").as_string()),
      _ObjectUUID_LocalIndex(
          deviceAccessPointItem.attribute("ObjectUUID_LocalIndex").as_int()),
      _MultipleWriteSupported(
          deviceAccessPointItem.attribute("MultipleWriteSupported").as_bool()),
      _MaxSupportedRecordSize(
          deviceAccessPointItem.attribute("MaxSupportedRecordSize").as_int()),
      _ParameterizationSpeedupSupported(
          deviceAccessPointItem.attribute("ParameterizationSpeedupSupported")
              .as_bool()),
      _PowerOnToCommReady(
          deviceAccessPointItem.attribute("PowerOnToCommReady").as_int()),
      _DeviceAccessSupported(
          deviceAccessPointItem.attribute("DeviceAccessSupported").as_bool()),
      _SharedDeviceSupported(
          deviceAccessPointItem.attribute("SharedDeviceSupported").as_bool()),
      _CheckDeviceID_Allowed(
          deviceAccessPointItem.attribute("CheckDeviceID_Allowed").as_bool()),
      _NameOfStationNotTransferable(
          deviceAccessPointItem.attribute("NameOfStationNotTransferable")
              .as_bool()),
      _NumberOfDeviceAccessAR(
          deviceAccessPointItem.attribute("NumberOfDeviceAccessAR").as_int()),
      _NumberOfSubmodules(
          deviceAccessPointItem.attribute("NumberOfSubmodules").as_int()),
      _LLDP_NoD_Supported(deviceAccessPointItem.attribute("LLDP_NoD_Supported").as_bool()),
      // _PhysicalSubslots(deviceAccessPointItem.attribute("PhysicalSubslots").as_string()),
      _ResetToFactoryModes(deviceAccessPointItem.attribute("ResetToFactoryModes").as_uint()),
      // _ModuleInfo(deviceAccessPointItem.child("ModuleInfo"), gsdml),
      _CertificationInfo(deviceAccessPointItem.child("CertificationInfo"), gsdml),
      _IOConfigData(deviceAccessPointItem.child("IOConfigData"), gsdml)
{
  for (pugi::xml_node& subslotItem :
       deviceAccessPointItem.child("SubslotList").children("SubslotItem"))
  {
    _SubslotList.push_back(SubslotItem(std::move(subslotItem), gsdml));
  }

  for (pugi::xml_node& moduleItemRef :
       deviceAccessPointItem.child("UseableModules").children("ModuleItemRef"))
  {
    _UseableModules[std::string(moduleItemRef.attribute("ModuleItemTarget")
                                    .as_string())]
        .reset(new ModuleItemRef(std::move(moduleItemRef), gsdml));
  }

  // for (pugi::xml_node& virtualSubmoduleItem : deviceAccessPointItem.child("VirtualSubmoduleList").children("VirtualSubmoduleItem"))
  // {
  //   _VirtualSubmoduleList[std::string(virtualSubmoduleItem.attribute("ID").as_string())].reset(new VirtualSubmoduleItem(std::move(virtualSubmoduleItem), gsdml));
  // }

  // for (pugi::xml_node& submoduleItemRef :
  //      deviceAccessPointItem.child("UseableSubmodules").children("SubmoduleItemRef"))
  // {
  //   _UseableSubmodules[std::string(submoduleItemRef.attribute("SubmoduleItemTarget")
  //                                   .as_string())]
  //       .reset(new SubmoduleItemRef(std::move(submoduleItemRef), gsdml));
  // }

  for (pugi::xml_node& systemDefinedSubmodule :
       deviceAccessPointItem.child("SystemDefinedSubmoduleList").children("InterfaceSubmoduleItem"))
  {
    _SystemDefinedSubmoduleList[std::string(systemDefinedSubmodule.attribute("ID").as_string())].reset(new InterfaceSubmoduleItem(std::move(systemDefinedSubmodule), gsdml));
  }

  for (pugi::xml_node& systemDefinedSubmodule :
       deviceAccessPointItem.child("SystemDefinedSubmoduleList").children("PortSubmoduleItem"))
  {
    _SystemDefinedSubmoduleList[std::string(systemDefinedSubmodule.attribute("ID").as_string())].reset(new PortSubmoduleItem(std::move(systemDefinedSubmodule), gsdml));
  }
}

ChannelDiagItem::ChannelDiagItem(pugi::xml_node&& channelDiagItem,
                                 pn_gsdml* gsdml)
    : //_ErrorType(channelDiagItem.attribute("ErrorType").as_uint()),
      _Name(gsdml->_get_TextId(std::move(std::string(
          channelDiagItem.child("Name").attribute("TextId").value())))),
      _Help(gsdml->_get_TextId(std::move(std::string(
          channelDiagItem.child("Help").attribute("TextId").value()))))
{
  for (pugi::xml_node& extChannelDiagItem :
       channelDiagItem.child("ExtChannelDiagList")
           .children("ExtChannelDiagItem"))
    _ExtChannelDiagList.emplace(std::make_pair(
        channelDiagItem.attribute("ErrorType").as_uint(),
        ExtChannelDiagItem(std::move(extChannelDiagItem), gsdml)));
}

ExtChannelDiagItem::ExtChannelDiagItem(pugi::xml_node&& extChannelDiagItem,
                                       pn_gsdml* gsdml)
    : //_ErrorType(extChannelDiagItem.attribute("ErrorType").as_uint()),
      _Name(gsdml->_get_TextId(std::move(std::string(
          extChannelDiagItem.child("Name").attribute("TextId").value())))),
      _Help(gsdml->_get_TextId(std::move(std::string(
          extChannelDiagItem.child("Help").attribute("TextId").value()))))
{
  for (pugi::xml_node& dataItem :
       extChannelDiagItem.child("ExtChannelAddValue").children("DataItem"))
    _ExtChannelAddValue.push_back(DataItem(std::move(dataItem), gsdml));
}

} // namespace GSDML