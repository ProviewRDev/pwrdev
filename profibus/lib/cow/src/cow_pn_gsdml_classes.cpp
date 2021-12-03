#include "cow_pn_gsdml_classes.h"
#include "cow_pn_gsdml.h"
#include <cstring>

namespace GSDML
{

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

Const::Const(pugi::xml_node&& xmlNode, Node* parent, pn_gsdml* gsdml)
    : _ByteOffset(xmlNode.attribute("ByteOffset").as_uint())
{
  setParent(parent);
  // Lets parse it...
  std::istringstream buf(xmlNode.attribute("Data").as_string(),
                         std::ios_base::in);
  std::string value;
  unsigned int byte;
  size_t size = 0;
  unsigned char* data = static_cast<ParameterRecordDataItem*>(m_Parent)->getData();
  unsigned char* constData = (data + _ByteOffset);
  while (getline(buf, value, ','))
  {
    std::istringstream val(value, std::ios_base::in);
    val.seekg(2); // Skip the 0x part
    val >> std::hex >> byte;
    *(constData + size) = byte;
    _data.insert(_data.begin(), (constData + size++));
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
          moduleInfo.attribute("CategoryRef").value()))
{
}

BitDataItem::BitDataItem(pugi::xml_node&& bitDataItem, pn_gsdml* gsdml)
    : _BitOffset(bitDataItem.attribute("BitOffset").as_uint()),
      _Text(gsdml->_get_TextId(bitDataItem.attribute("TextId").value()))
{
}

DataItem::DataItem(pugi::xml_node&& dataItem, pn_gsdml* gsdml)
    : _Id(dataItem.attribute("Id").as_string()),
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

// MenuItem::MenuItem(pugi::xml_node&& menuItem,
//                    std::map<std::string, std::shared_ptr<Ref>>& refs,
//                    pn_gsdml* gsdml)
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
      _Changeable(ref.attribute("Changeable").as_bool()),
      _Visible(ref.attribute("Visible").as_bool()),
      _Text(gsdml->_get_TextId(ref.attribute("TextId").value()))
{
  setParent(parent);
  string_to_value_datatype(ref.attribute("DataType").as_string(), &_DataType);
  if (ref.attribute("ValueItemTarget"))
  {
    _ValueItem = gsdml->getValueList()[std::move(
        std::string(ref.attribute("ValueItemTarget").as_string()))];
  }
}

// pn_gsdml::Const::Const(pugi::xml_node&& Const, ParameterRecordDataItem&
// parameterRecordDataItem, pn_gsdml* gsdml) :
//   _Data(Const.attribute("Data").as_string()),
//   _ByteOffset(Const.attribute("ByteOffset").as_uint())
// {

// }

ParameterRecordDataItem::ParameterRecordDataItem(
    pugi::xml_node&& parameterRecordDataItem, pn_gsdml* gsdml)
    : _Index(parameterRecordDataItem.attribute("Index").as_uint()),
      _Length(parameterRecordDataItem.attribute("Length").as_uint()),
      _TransferSequence(
          parameterRecordDataItem.attribute("TransferSequence").as_uint()),
      _Name(gsdml->_get_TextId(
          parameterRecordDataItem.child("Name").attribute("TextId").value()))
{
  _data = new unsigned char[_Length];

  for (pugi::xml_node& iConst : parameterRecordDataItem.children("Const"))
  {
    _Const.push_back(GSDML::Const(std::move(iConst), this, gsdml));
  }

  for (pugi::xml_node& ref : parameterRecordDataItem.children("Ref"))
  {
    _Ref.emplace(
        std::make_pair(std::move(std::string(ref.attribute("ID").as_string())),
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

unsigned char* ParameterRecordDataItem::getData() { return _data; }

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
    : _Help(gsdml->getTextIdList()[std::move(std::string(
          valueItem.child("Help").attribute("TextId").as_string()))])
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

VirtualSubmoduleItem::VirtualSubmoduleItem(
    pugi::xml_node&& virtualSubmoduleItem, pn_gsdml* gsdml)
    : _SubmoduleIdentNumber(
          virtualSubmoduleItem.attribute("SubmoduleIdentNumber").as_uint()),
      _FixedInSubslots(
          virtualSubmoduleItem.attribute("FixedInSubslots").as_string()),
      _Writeable_IM_Records(
          virtualSubmoduleItem.attribute("Writeable_IM_Records").as_string()),
      _MayIssueProcessAlarm(
          virtualSubmoduleItem.attribute("MayIssueProcessAlarm").as_bool()),
      _IOData(virtualSubmoduleItem.child("IOData"), gsdml),
      _ModuleInfo(virtualSubmoduleItem.child("ModuleInfo"), gsdml)
{
  for (pugi::xml_node& parameterRecordDataItem :
       virtualSubmoduleItem.child("RecordDataList")
           .children("ParameterRecordDataItem"))
    _RecordDataList.push_back(
        ParameterRecordDataItem(std::move(parameterRecordDataItem), gsdml));
}

ModuleItem::ModuleItem(pugi::xml_node&& moduleItem, pn_gsdml* gsdml)
    : _ModuleIdentNumber(moduleItem.attribute("ModuleIdentNumber").as_uint()),
      _ModuleInfo(moduleItem.child("ModuleInfo"), gsdml)

{
  for (pugi::xml_node& virtualSubmoduleItem :
       moduleItem.child("VirtualSubmoduleList")
           .children("VirtualSubmoduleItem"))
  {
    std::string ID = virtualSubmoduleItem.attribute("ID").value();
    _VirtualSubmoduleList[ID].reset(
        new VirtualSubmoduleItem(std::move(virtualSubmoduleItem), gsdml));
  }
}

ModuleItemRef::ModuleItemRef(pugi::xml_node&& moduleItemRef, pn_gsdml* gsdml)
    : _ModuleItemTarget(gsdml->getModuleList()[std::move(std::string(
          moduleItemRef.attribute("ModuleItemTarget").as_string()))]),
      _AllowedInSlots(moduleItemRef.attribute("AllowedInSlots").as_string())
{
}

DeviceAccessPointItem::DeviceAccessPointItem(
    pugi::xml_node&& deviceAccessPointItem, pn_gsdml* gsdml)
    : _PNIO_Version(
          deviceAccessPointItem.attribute("PNIO_Version").as_string()),
      _RequiredSchemaVersion(
          deviceAccessPointItem.attribute("RequiredSchemaVersion").as_string()),
      _PhysicalSlots(
          deviceAccessPointItem.attribute("PhysicalSlots").as_string()),
      _ModuleIdentNumber(
          deviceAccessPointItem.attribute("ModuleIdentNumber").as_uint()),
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
      _ModuleInfo(deviceAccessPointItem.child("ModuleInfo"), gsdml),
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