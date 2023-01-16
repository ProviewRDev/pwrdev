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

void parse_octet_string(std::string const& string_data, unsigned char* output_data,
                        std::vector<unsigned char>& output_vector)
{
  // Lets parse it...
  std::istringstream buf(string_data, std::ios_base::in);

  std::string value;
  unsigned int byte;
  size_t pos = 0;
  while (getline(buf, value, ','))
  {
    std::istringstream val(value, std::ios_base::in);
    val.seekg(2); // Skip the 0x part
    val >> std::hex >> byte;
    *(output_data + pos++) = byte;
    output_vector.push_back(byte);
  }
}

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

uint io_datatype_length(GSDML::eValueDataType datatype)
{
  switch (datatype)
  {
  case ValueDataType_Integer8:
  case ValueDataType_Unsigned8:
    return 1;
  case ValueDataType_Integer16:
  case ValueDataType_Unsigned16:
    return 2;
  case ValueDataType_Float32:
  case ValueDataType_Integer32:
  case ValueDataType_Unsigned32:
    return 4;
  case ValueDataType_Float64:
  case ValueDataType_Integer64:
  case ValueDataType_Unsigned64:
    return 8;
  default:;
  }
  return 0;
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

std::vector<std::string>& TokenList::getList() { return tokenItems; }

Const::Const(pugi::xml_node&& xmlNode, Node* parent, pn_gsdml* gsdml)
    : _ByteOffset(xmlNode.attribute("ByteOffset").as_uint())
{
  setParent(parent);
  // Lets parse it...
  parse_octet_string(xmlNode.attribute("Data").as_string(),
                     static_cast<ParameterRecordDataItem*>(m_Parent)->getData() + _ByteOffset, _data);
}

ModuleInfo::ModuleInfo(pugi::xml_node&& moduleInfo, pn_gsdml* gsdml)
    : _Name(gsdml->_get_TextId(moduleInfo.child("Name").attribute("TextId").as_string())),
      _InfoText(gsdml->_get_TextId(moduleInfo.child("InfoText").attribute("TextId").as_string())),
      _VendorName(moduleInfo.child("VendorName").attribute("Value").as_string()),
      _OrderNumber(moduleInfo.child("OrderNumber").attribute("Value").as_string()),
      _SoftwareRelease(moduleInfo.child("SoftwareRelease").attribute("Value").as_string()),
      _HardwareRelease(moduleInfo.child("HardwareRelease").attribute("Value").as_string()),
      _CategoryItemText(gsdml->_get_CategoryTextRef(moduleInfo.attribute("CategoryRef").value())),
      _CategoryItemInfoText(gsdml->_get_CategoryInfoTextRef(moduleInfo.attribute("CategoryRef").value())),
      _SubCategoryItemText(gsdml->_get_CategoryTextRef(moduleInfo.attribute("SubCategory1Ref").value())),
      _SubCategoryItemInfoText(
          gsdml->_get_CategoryInfoTextRef(moduleInfo.attribute("SubCategory1Ref").value()))
{
}

BitDataItem::BitDataItem(pugi::xml_node&& bitDataItem, pn_gsdml* gsdml)
    : _BitOffset(bitDataItem.attribute("BitOffset").as_uint()),
      _Text(gsdml->_get_TextId(bitDataItem.attribute("TextId").value()))
{
}

DataItem::DataItem(pugi::xml_node&& dataItem, pn_gsdml* gsdml)
    : _DataTypeString(dataItem.attribute("DataType").as_string()), _Id(dataItem.attribute("Id").as_string()),
      _Length(dataItem.attribute("Length").as_uint()), _UseAsBits(dataItem.attribute("UseAsBits").as_bool()),
      _Text(gsdml->_get_TextId(std::move(std::string(dataItem.attribute("TextId").value())))),
      _TextId(dataItem.attribute("TextId").as_string())
{
  string_to_value_datatype(dataItem.attribute("DataType").as_string(), &_DataType);

  // If we have no length, deduce the length from the datatype, makes it easier later on to calculate data
  // lengths and the time to do this is neglible
  if (_Length == 0)
    _Length = io_datatype_length(_DataType);

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
    : _Input(ioData.child("Input"), gsdml), _Output(ioData.child("Output"), gsdml)
{
}

Assign::Assign(pugi::xml_node&& assign, pn_gsdml* gsdml)
    : _Content(assign.attribute("Content").as_uint()),
      _Text(gsdml->_get_TextId(std::move(std::string(assign.attribute("TextId").as_string()))))
{
}

MenuItem::MenuItem(pugi::xml_node&& menuItem, Node* parent, pn_gsdml* gsdml)
    : _Name(gsdml->_get_TextId(menuItem.child("Name").attribute("TextId").as_string()))
{
  setParent(parent);
  for (pugi::xml_node& parameterRef : menuItem.children("ParameterRef"))
  {
    ParameterRecordDataItem* prdi = static_cast<ParameterRecordDataItem*>(m_Parent);
    _ParameterRefs.push_back(
        prdi->_Ref[std::move(std::string(parameterRef.attribute("ParameterTarget").as_string()))]);
  }
}

void MenuItem::linkMenuRefs(pugi::xml_node& menuItem,
                            std::unordered_map<std::string, std::shared_ptr<MenuItem>>& menuList)
{
  for (pugi::xml_node& menuRef : menuItem.children("MenuRef"))
  {
    _MenuRefs.push_back(menuList[std::string(menuRef.attribute("MenuTarget").as_string())]);
  }
}

Ref::Ref(pugi::xml_node&& ref, Node* parent, pn_gsdml* gsdml)
    : _ByteOffset(ref.attribute("ByteOffset").as_uint()), _BitOffset(ref.attribute("BitOffset").as_uint()),
      _BitLength(ref.attribute("BitLength").as_uint()),
      _DefaultValue(ref.attribute("DefaultValue").as_double()),
      _DefaultValueString(ref.attribute("DefaultValue").as_string()),
      _AllowedValues(ValueList<double>(ref.attribute("AllowedValues").as_string())),
      _Length(ref.attribute("Length").as_uint()),
      _Changeable(ref.attribute("Changeable").empty()
                      ? true
                      : ref.attribute("Changeable").as_bool()), // This defaults to true if missing according
                                                                // to ISO 15745-4:2003/Amd.1:2006(E)
      _Visible(ref.attribute("Visible").empty()
                   ? true
                   : ref.attribute("Visible").as_bool()), // This defaults to true if missing according to ISO
                                                          // 15745-4:2003/Amd.1:2006(E)
      _Text(gsdml->_get_TextId(ref.attribute("TextId").value())), _ID(ref.attribute("ID").as_string())
{
  setParent(parent);
  string_to_value_datatype(ref.attribute("DataType").as_string(), &_DataType);
  if (ref.attribute("ValueItemTarget"))
  {
    _ValueItem = gsdml->getValueMap()[std::move(std::string(ref.attribute("ValueItemTarget").as_string()))];
  }
}

ParameterRecordDataItem::ParameterRecordDataItem(pugi::xml_node&& parameterRecordDataItem, pn_gsdml* gsdml)
    : _Index(parameterRecordDataItem.attribute("Index").as_uint()),
      _Length(parameterRecordDataItem.attribute("Length").as_uint()),
      _TransferSequence(parameterRecordDataItem.attribute("TransferSequence").as_uint()),
      _Name(gsdml->_get_TextId(parameterRecordDataItem.child("Name").attribute("TextId").value()))
{
  _data = new unsigned char[_Length]();

  for (pugi::xml_node& iConst : parameterRecordDataItem.children("Const"))
  {
    _Const.push_back(GSDML::Const(std::move(iConst), this, gsdml));
  }

  for (pugi::xml_node& ref : parameterRecordDataItem.children("Ref"))
  {
    std::string ID;

    // Index using either ValueItemTarget, ID or TextID, order of preference ID > TextId > ValueItemTarget
    if (!ref.attribute("ID").empty())
    {
      ID = ref.attribute("ID").as_string();
    }
    else if (!ref.attribute("TextId").empty())
    {
      ID = ref.attribute("TextId").as_string();
    }
    else
    {
      ID = ref.attribute("ValueItemTarget").as_string();
    }

    _Ref.emplace(std::make_pair(std::move(ID), std::make_shared<Ref>(Ref(std::move(ref), this, gsdml))));
  }

  // The menuitems need to be parsed "breadth first" since MenuRef's would
  // otherwise reference not already "compiled" menus... We can parse everything
  // except the MenuRef's...
  for (pugi::xml_node& menuItem : parameterRecordDataItem.child("MenuList").children("MenuItem"))
  {
    std::shared_ptr<MenuItem> mi(std::make_shared<MenuItem>(std::move(menuItem), this, gsdml));
    _MenuLookupList.emplace(std::make_pair(std::move(std::string(menuItem.attribute("ID").as_string())), mi));
    _MenuList.push_back(mi);
  }

  // Now link the MenuRefs
  for (pugi::xml_node& menuItem : parameterRecordDataItem.child("MenuList").children("MenuItem"))
  {
    _MenuLookupList[menuItem.attribute("ID").as_string()]->linkMenuRefs(menuItem, _MenuLookupList);
  }
}

ParameterRecordDataItem::ParameterRecordDataItem(ParameterRecordDataItem&& o)
    : _Index(o._Index), _Length(o._Length), _TransferSequence(o._TransferSequence), _Name(std::move(o._Name)),
      _Const(std::move(o._Const)), _Ref(std::move(o._Ref)), _MenuLookupList(std::move(o._MenuLookupList)),
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
  for (pugi::xml_node& assign : valueItem.child("Assignments").children("Assign"))
    _Assignments.push_back(Assign(std::move(assign), gsdml));
}

CertificationInfo::CertificationInfo(pugi::xml_node&& CertificationInfo, pn_gsdml* gsdml)
    : _ConformanceClass(CertificationInfo.attribute("ConformanceClass").value()),
      _ApplicationClass(CertificationInfo.attribute("ApplicationClass").value()),
      _NetloadClass(CertificationInfo.attribute("NetloadClass").value())
{
}

IOConfigData::IOConfigData(pugi::xml_node&& IOConfigData, pn_gsdml* gsdml)
    : _MaxApplicationInputLength(IOConfigData.attribute("MaxApplicationInputLength").as_int()),
      _MaxApplicationOutputLength(IOConfigData.attribute("MaxApplicationOutputLength").as_int()),
      _MaxApplicationDataLength(IOConfigData.attribute("MaxApplicationDataLength").as_int()),
      _MaxInputLength(IOConfigData.attribute("MaxInputLength").as_int()),
      _MaxOutputLength(IOConfigData.attribute("MaxOutputLength").as_int()),
      _MaxDataLength(IOConfigData.attribute("MaxDataLength").as_int()),
      _ApplicationLengthIncludesIOxS(IOConfigData.attribute("ApplicationLengthIncludesIOxS").as_bool())
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
  // Fill in default values if these are not present
  if (_SendClock.empty())
    _SendClock = ValueList<uint>("8 16 32 64 128");

  if (_ReductionRatioPow2.empty())
    _ReductionRatioPow2 = ValueList<uint>("1 2 4 8 16 32 64 128 256 512");
}

RT_Class3TimingProperties::RT_Class3TimingProperties(pugi::xml_node&& rt_Class3TimingProperties,
                                                     pn_gsdml* gsdml)
    : _SendClock(rt_Class3TimingProperties.attribute("SendClock").as_string()),
      _ReductionRatioPow2(rt_Class3TimingProperties.attribute("ReductionRatioPow2").as_string())
{
  // Fill in default values if these are not present. Even though class 3 properties are always there.
  // Until confirmed with the spec leave this as is...
  if (_SendClock.empty())
    _SendClock = ValueList<uint>("8 16 32 64 128");

  if (_ReductionRatioPow2.empty())
    _ReductionRatioPow2 = ValueList<uint>("1 2 4 8 16");
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

SubmoduleItem::SubmoduleItem(pugi::xml_node&& submoduleItem, pn_gsdml* gsdml,
                             eSubmoduleItemType submoduleType)
    : _SubmoduleIdentNumber(submoduleItem.attribute("SubmoduleIdentNumber").as_uint()),
      _SubslotNumber(submoduleItem.attribute("SubslotNumber").as_uint()),
      _FixedInSubslots(submoduleItem.attribute("FixedInSubslots").as_string()),
      _Writeable_IM_Records(submoduleItem.attribute("Writeable_IM_Records").as_string()),
      _MayIssueProcessAlarm(submoduleItem.attribute("MayIssueProcessAlarm").as_bool()),
      _Text(gsdml->_get_TextId(submoduleItem.attribute("TextId").value())),
      _ID(submoduleItem.attribute("ID").as_string()), _API(submoduleItem.attribute("API").as_uint()),
      _IOData(submoduleItem.child("IOData"), gsdml), _ModuleInfo(submoduleItem.child("ModuleInfo"), gsdml),
      _SubmoduleItemType(submoduleType)
{
  for (pugi::xml_node& parameterRecordDataItem :
       submoduleItem.child("RecordDataList").children("ParameterRecordDataItem"))
    _RecordDataList.push_back(ParameterRecordDataItem(std::move(parameterRecordDataItem), gsdml));
}

SubmoduleItem::SubmoduleItem(eSubmoduleItemType submoduleType)
    : _FixedInSubslots(""), _Writeable_IM_Records(""), _SubmoduleItemType(submoduleType)
{
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

InterfaceSubmoduleItem::InterfaceSubmoduleItem()
    : SubmoduleItem(GSDML::SubmoduleItemType_Interface), _SupportedRT_Classes("RT_CLASS_1"),
      _SupportedProtocols("")
{
}

PortSubmoduleItem::PortSubmoduleItem(pugi::xml_node&& portSubmoduleItem, pn_gsdml* gsdml)
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
      _CheckMAUTypeDifferenceSupported(
          portSubmoduleItem.attribute("CheckMAUTypeDifferenceSupported").as_bool())
{
  for (pugi::xml_node& mauTypeItem : portSubmoduleItem.child("MAUTypeList").children("MAUTypeItem"))
  {
    _MAUTypeList.push_back(MAUTypeItem(std::move(mauTypeItem), gsdml));
  }
}

VirtualSubmoduleItem::VirtualSubmoduleItem(pugi::xml_node&& virtualSubmoduleItem, pn_gsdml* gsdml)
    : SubmoduleItem(std::move(virtualSubmoduleItem), gsdml, GSDML::SubmoduleItemType_Virtual)
{
}

ModuleItem::ModuleItem(pugi::xml_node&& moduleItem, pn_gsdml* gsdml, eModuleItemType moduleType)
    : _ModuleIdentNumber(moduleItem.attribute("ModuleIdentNumber").as_uint()),
      _PhysicalSubslots(moduleItem.attribute("PhysicalSubslots").as_string()),
      _ID(moduleItem.attribute("ID").as_string()), _ModuleInfo(moduleItem.child("ModuleInfo"), gsdml),
      _ModuleItemType(moduleType)
{
  for (pugi::xml_node& virtualSubmoduleItem :
       moduleItem.child("VirtualSubmoduleList").children("VirtualSubmoduleItem"))
  {
    std::string ID = virtualSubmoduleItem.attribute("ID").value();
    _VirtualSubmoduleList[ID].reset(new VirtualSubmoduleItem(std::move(virtualSubmoduleItem), gsdml));
  }

  for (pugi::xml_node& submoduleItemRef : moduleItem.child("UseableSubmodules").children("SubmoduleItemRef"))
  {
    std::string submoduleItemTarget = submoduleItemRef.attribute("SubmoduleItemTarget").value();
    _UseableSubmodules.emplace(
        std::make_pair(std::move(submoduleItemTarget),
                       std::make_shared<SubmoduleItemRef>(std::move(submoduleItemRef), gsdml)));
  }
}

ModuleItemRef::ModuleItemRef(pugi::xml_node&& moduleItemRef, pn_gsdml* gsdml)
    : _ModuleItemTarget(gsdml->getModuleMap()[std::move(
          std::string(moduleItemRef.attribute("ModuleItemTarget").as_string()))]),
      _AllowedInSlots(moduleItemRef.attribute("AllowedInSlots").as_string()),
      _FixedInSlots(moduleItemRef.attribute("FixedInSlots").as_string())
{
}

SubmoduleItemRef::SubmoduleItemRef(pugi::xml_node&& submoduleItemRef, pn_gsdml* gsdml)
    : _SubmoduleItemTarget(gsdml->getSubmoduleMap()[std::move(
          std::string(submoduleItemRef.attribute("SubmoduleItemTarget").as_string()))]),
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

DeviceAccessPointItem::DeviceAccessPointItem(pugi::xml_node&& deviceAccessPointItem, pn_gsdml* gsdml)
    : ModuleItem(std::forward<pugi::xml_node>(deviceAccessPointItem), gsdml, GSDML::ModuleItemType_DAP),
      _PNIO_Version(deviceAccessPointItem.attribute("PNIO_Version").as_string()),
      _RequiredSchemaVersion(deviceAccessPointItem.attribute("RequiredSchemaVersion").as_string()),
      _PhysicalSlots(deviceAccessPointItem.attribute("PhysicalSlots").as_string()),
      _MinDeviceInterval(deviceAccessPointItem.attribute("MinDeviceInterval").as_int()),
      _ImplementationType(deviceAccessPointItem.attribute("ImplementationType").as_string()),
      _DNS_CompatibleName(deviceAccessPointItem.attribute("DNS_CompatibleName").as_string()),
      _FixedInSlots(deviceAccessPointItem.attribute("FixedInSlots").as_string()),
      _ObjectUUID_LocalIndex(deviceAccessPointItem.attribute("ObjectUUID_LocalIndex").as_int()),
      _MultipleWriteSupported(deviceAccessPointItem.attribute("MultipleWriteSupported").as_bool()),
      _MaxSupportedRecordSize(deviceAccessPointItem.attribute("MaxSupportedRecordSize").as_int()),
      _ParameterizationSpeedupSupported(
          deviceAccessPointItem.attribute("ParameterizationSpeedupSupported").as_bool()),
      _PowerOnToCommReady(deviceAccessPointItem.attribute("PowerOnToCommReady").as_int()),
      _DeviceAccessSupported(deviceAccessPointItem.attribute("DeviceAccessSupported").as_bool()),
      _SharedDeviceSupported(deviceAccessPointItem.attribute("SharedDeviceSupported").as_bool()),
      _CheckDeviceID_Allowed(deviceAccessPointItem.attribute("CheckDeviceID_Allowed").as_bool()),
      _NameOfStationNotTransferable(
          deviceAccessPointItem.attribute("NameOfStationNotTransferable").as_bool()),
      _NumberOfDeviceAccessAR(deviceAccessPointItem.attribute("NumberOfDeviceAccessAR").as_int()),
      _NumberOfSubmodules(deviceAccessPointItem.attribute("NumberOfSubmodules").as_int()),
      _LLDP_NoD_Supported(deviceAccessPointItem.attribute("LLDP_NoD_Supported").as_bool()),
      _ResetToFactoryModes(deviceAccessPointItem.attribute("ResetToFactoryModes").as_uint()),
      _CertificationInfo(deviceAccessPointItem.child("CertificationInfo"), gsdml),
      _IOConfigData(deviceAccessPointItem.child("IOConfigData"), gsdml)
{
  for (pugi::xml_node& subslotItem : deviceAccessPointItem.child("SubslotList").children("SubslotItem"))
  {
    _SubslotList.push_back(SubslotItem(std::move(subslotItem), gsdml));
  }

  for (pugi::xml_node& moduleItemRef :
       deviceAccessPointItem.child("UseableModules").children("ModuleItemRef"))
  {
    _UseableModules[std::string(moduleItemRef.attribute("ModuleItemTarget").as_string())].reset(
        new ModuleItemRef(std::move(moduleItemRef), gsdml));
  }

  for (pugi::xml_node& systemDefinedSubmodule :
       deviceAccessPointItem.child("SystemDefinedSubmoduleList").children("InterfaceSubmoduleItem"))
  {
    _SystemDefinedSubmoduleList[std::string(systemDefinedSubmodule.attribute("ID").as_string())].reset(
        new InterfaceSubmoduleItem(std::move(systemDefinedSubmodule), gsdml));
  }

  for (pugi::xml_node& systemDefinedSubmodule :
       deviceAccessPointItem.child("SystemDefinedSubmoduleList").children("PortSubmoduleItem"))
  {
    _SystemDefinedSubmoduleList[std::string(systemDefinedSubmodule.attribute("ID").as_string())].reset(
        new PortSubmoduleItem(std::move(systemDefinedSubmodule), gsdml));
  }
}

ChannelDiagItem::ChannelDiagItem(pugi::xml_node&& channelDiagItem, pn_gsdml* gsdml)
    : _Name(gsdml->_get_TextId(
          std::move(std::string(channelDiagItem.child("Name").attribute("TextId").value())))),
      _Help(gsdml->_get_TextId(
          std::move(std::string(channelDiagItem.child("Help").attribute("TextId").value()))))
{
  for (pugi::xml_node& extChannelDiagItem :
       channelDiagItem.child("ExtChannelDiagList").children("ExtChannelDiagItem"))
    _ExtChannelDiagList.emplace(std::make_pair(extChannelDiagItem.attribute("ErrorType").as_uint(),
                                               ExtChannelDiagItem(std::move(extChannelDiagItem), gsdml)));
}

ExtChannelDiagItem::ExtChannelDiagItem(pugi::xml_node&& extChannelDiagItem, pn_gsdml* gsdml)
    : _Name(gsdml->_get_TextId(
          std::move(std::string(extChannelDiagItem.child("Name").attribute("TextId").value())))),
      _Help(gsdml->_get_TextId(
          std::move(std::string(extChannelDiagItem.child("Help").attribute("TextId").value()))))
{
  for (pugi::xml_node& dataItem : extChannelDiagItem.child("ExtChannelAddValue").children("DataItem"))
    _ExtChannelAddValue.push_back(DataItem(std::move(dataItem), gsdml));
}

UnitDiagTypeItem::UnitDiagTypeItem(pugi::xml_node&& unitDiagTypeItem, pn_gsdml* gsdml)
    : _UserStructureIdentifier(unitDiagTypeItem.attribute("UserStructureIdentifier").as_uint()),
      _Name(gsdml->_get_TextId(
          std::move(std::string(unitDiagTypeItem.child("Name").attribute("TextId").value())))),
      _Help(gsdml->_get_TextId(
          std::move(std::string(unitDiagTypeItem.child("Help").attribute("TextId").value()))))
{
  for (pugi::xml_node& ref : unitDiagTypeItem.children("Ref"))
  {  
    _Ref.push_back(std::make_shared<Ref>(Ref(std::move(ref), this, gsdml)));    
  }
}

} // namespace GSDML