/*
    Creates lists from input like "RT_CLASS1;RT_CLASS3;SNMP" aka tokenlists in
   GSDML
*/
#ifndef _COW_PN_GSDML_CLASSES_HPP_
#define _COW_PN_GSDML_CLASSES_HPP_
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include "co_pugixml.hpp"

class pn_gsdml;

namespace GSDML
{

void parse_octet_string(std::string const& string_data, unsigned char* output_data, std::vector<unsigned char> &output_vector);

typedef enum
{
  ValueDataType_,
  ValueDataType_Bit,
  ValueDataType_BitArea,
  ValueDataType_Integer8,
  ValueDataType_Integer16,
  ValueDataType_Integer32,
  ValueDataType_Integer64,
  ValueDataType_Unsigned8,
  ValueDataType_Unsigned16,
  ValueDataType_Unsigned32,
  ValueDataType_Unsigned64,
  ValueDataType_Float32,
  ValueDataType_Float64,
  ValueDataType_Date,
  ValueDataType_TimeOfDayWithDate,
  ValueDataType_TimeOfDayWithoutDate,
  ValueDataType_TimeDiffWithDate,
  ValueDataType_TimeDiffWithoutDate,
  ValueDataType_NetworkTime,
  ValueDataType_NetworkTimeDiff,
  ValueDataType_VisibleString,
  ValueDataType_OctetString,
  ValueDataType__
} eValueDataType;

typedef enum
{
  SubmoduleItemType_,
  SubmoduleItemType_Virtual,
  SubmoduleItemType_Port,
  SubmoduleItemType_Interface,
  SubmoduleItemType_Submodule,
  SubmoduleItemType__
} eSubmoduleItemType;

typedef enum
{
  ModuleItemType_,
  ModuleItemType_DAP,
  ModuleItemType_Module,
  ModuleItemType__
} eModuleItemType;

template <typename T> T reverse_type(const T in_type)
{
  T return_value;
  char* type_to_convert = (char*)&in_type;
  char* return_type = (char*)&return_value;
  switch (sizeof(T))
  {
  case 2:
    return_type[0] = type_to_convert[1];
    return_type[1] = type_to_convert[0];
    break;
  case 4:
    return_type[0] = type_to_convert[3];
    return_type[1] = type_to_convert[2];
    return_type[2] = type_to_convert[1];
    return_type[3] = type_to_convert[0];
    break;
  case 8:
    return_type[0] = type_to_convert[7];
    return_type[1] = type_to_convert[6];
    return_type[2] = type_to_convert[5];
    return_type[3] = type_to_convert[4];
    return_type[4] = type_to_convert[3];
    return_type[5] = type_to_convert[2];
    return_type[6] = type_to_convert[1];
    return_type[7] = type_to_convert[0];
    break;
  default:
    return_value = in_type;
  }
  return return_value;
}

class Node
{
public:
  virtual ~Node() {}
  void setParent(Node* parent) { m_Parent = parent; };
  Node* getParent() const { return m_Parent; };

protected:
  Node* m_Parent;
};

/*
  ValueList is for GSDML data in the format: 1 3..6 8 10 12 or 1..10 or 1 2 3

  To iterate over all possible values of a list like this:
  for (auto& item : list.getList())
    for (auto i = item.first; i <= item.second; i++)
      whatever;

  or, only the prefix ++ operator can be used no other overload exists:
  for (auto it = listname.begin(); it != listname.end(); ++it)
    ; // do something with it...
*/
template <typename T> class ValueList
{
private:
  std::vector<std::pair<T, T>> listItems;
  bool m_empty;
  std::string m_original_string;

public:
  class iterator
  {
  public:
    iterator(ValueList<T>& parent, bool is_end = false) : m_parent(parent)
    {
      if (is_end)
        m_value = m_parent.max();
      else
        m_value = m_parent.min();
    }
    ~iterator() = default;
    bool operator!=(iterator const& rhs) { return m_value <= rhs.m_value; }
    iterator& operator++()
    {
      while (!m_parent.inList(++m_value))
      {
        if (m_value >= m_parent.max())
        {
          m_value = m_parent.max();
          (m_value)++;
          break;
        }
      }
      return *this;
    }
    T value() { return m_value; }

  private:
    T m_value;
    ValueList<T>& m_parent;
  };

  ValueList(char const* input) : m_empty(true)
  {
    m_original_string = std::string(input);
    // Lets parse it...
    std::istringstream buf(input, std::ios_base::in);

    std::string value;
    std::string rangeDelimeter("..");
    while (buf >> value)
    {
      m_empty = false;
      std::size_t pos;
      if ((pos = value.find(rangeDelimeter)) != std::string::npos)
      {
        value.replace(pos, 2, " ");
        std::istringstream tokenStream(value, std::ios_base::in);
        T val1, val2;
        tokenStream >> val1 >> val2;
        listItems.push_back(std::pair<T, T>(val1, val2));
      }
      else
      {
        std::istringstream tokenStream(value, std::ios_base::in);
        T val;
        tokenStream >> val;
        listItems.push_back(std::pair<T, T>(val, val));
      }
    }
  }

  bool empty() const { return m_empty; }

  bool inList(T value) const
  {
    for (auto& range : listItems)
      if (value >= range.first && value <= range.second)
        return true;
    return false;
  }

  void print()
  {
    for (auto& value : listItems)
      for (auto i = value.first; i <= value.second; i++)
        std::cout << i << std::endl;
  }

  T min() const
  {
    T min = listItems[0].first;
    for (auto& range : listItems)
      if (min >= range.first) // Spec says that ranges like x..y must have the lesser item to the left of ".."
        min = range.first;
    return min;
  }

  T max() const
  {
    T max = listItems[0].second;
    for (auto& range : listItems)
      if (max <=
          range.second) // Spec says that ranges like x..y must have the lesser item to the left of ".."
        max = range.second;
    return max;
  }

  std::string& as_string() { return m_original_string; }

  ValueList<T>::iterator begin() { return iterator(*this); }

  ValueList<T>::iterator end() { return iterator(*this, true); }

  std::vector<std::pair<T, T>>& getList() { return listItems; }
};

/*
  TokenList is for GSDML data in the format: token1;token2
*/
class TokenList
{
public:
  TokenList(char const* input);
  ~TokenList() = default;
  bool inList(std::string const& value) const;
  void print();
  std::vector<std::string>& getList();

private:
  std::vector<std::string> tokenItems;
};

class Const : public Node
{
public:
  Const(pugi::xml_node&&, Node*, pn_gsdml*);
  Const(Const&&) = default;
  Const(Const const&) = default;
  ~Const() = default;

  ushort _ByteOffset;
  std::vector<unsigned char> _data;
};

class BitDataItem
{
public:
  BitDataItem(pugi::xml_node&&, pn_gsdml*);
  BitDataItem(BitDataItem&&) = default;

  // Attributes
  ushort _BitOffset;
  std::shared_ptr<std::string> _Text;
};

class DataItem
{
public:
  DataItem(pugi::xml_node&&, pn_gsdml*);
  DataItem(DataItem&&) = default;
  // Attributes
  eValueDataType _DataType;
  std::string _DataTypeString;
  std::string _Id;
  ushort _Length;
  bool _UseAsBits;
  std::shared_ptr<std::string> _Text;
  std::string _TextId; // We save this string as is aswell. It's super nice to have as part of a channel name

  // Elements
  std::vector<BitDataItem> _BitDataItem;
};

class ExtChannelDiagItem
{
public:
  ExtChannelDiagItem(pugi::xml_node&&, pn_gsdml*);
  ExtChannelDiagItem(ExtChannelDiagItem&&) = default;
  // ushort _ErrorType; implicit in the map
  std::shared_ptr<std::string> _Name;
  std::shared_ptr<std::string> _Help;
  std::vector<DataItem> _ExtChannelAddValue;
};

class ChannelDiagItem
{
public:
  ChannelDiagItem(pugi::xml_node&&, pn_gsdml*);
  ChannelDiagItem(ChannelDiagItem&&) = default;
  // ushort _ErrorType; implicit in the map
  std::shared_ptr<std::string> _Name;
  std::shared_ptr<std::string> _Help;
  std::unordered_map<ushort, ExtChannelDiagItem> _ExtChannelDiagList;
};

class ModuleInfo
{
public:
  ModuleInfo(pugi::xml_node&&, pn_gsdml*);
  ModuleInfo() = default;
  ModuleInfo(ModuleInfo&&) = default;

  std::shared_ptr<std::string> _Name;
  std::shared_ptr<std::string> _InfoText;
  std::string _VendorName;
  std::string _OrderNumber;
  std::string _SoftwareRelease;
  std::string _HardwareRelease;
  std::shared_ptr<std::string> _CategoryItemText;
  std::shared_ptr<std::string> _CategoryItemInfoText;
  std::shared_ptr<std::string> _SubCategoryItemText;
  std::shared_ptr<std::string> _SubCategoryItemInfoText;
};

class SubslotItem
{
public:
  SubslotItem(pugi::xml_node&&, pn_gsdml*);
  SubslotItem(SubslotItem&&) = default;
  ushort _SubslotNumber;
  std::shared_ptr<std::string> _Text;
};

class CertificationInfo
{
public:
  CertificationInfo(pugi::xml_node&&, pn_gsdml*);
  CertificationInfo(CertificationInfo&&) = default;
  std::string _ConformanceClass;
  std::string _ApplicationClass;
  std::string _NetloadClass;
};

class IOConfigData
{
public:
  IOConfigData(pugi::xml_node&&, pn_gsdml*);
  IOConfigData(IOConfigData&&) = default;
  ushort _MaxApplicationInputLength;
  ushort _MaxApplicationOutputLength;
  ushort _MaxApplicationDataLength;
  ushort _MaxInputLength;
  ushort _MaxOutputLength;
  ushort _MaxDataLength;
  bool _ApplicationLengthIncludesIOxS;
};

class Assign
{
public:
  Assign(pugi::xml_node&&, pn_gsdml*);
  Assign(Assign&&) = default;
  uint _Content;
  std::shared_ptr<std::string> _Text;
};

class ValueItem
{
public:
  ValueItem(pugi::xml_node&& valueItem, pn_gsdml* gsdml);
  ValueItem(ValueItem&&) = default;

  std::shared_ptr<std::string> _Help;
  std::vector<Assign> _Assignments;
};

class Ref : public Node
{
public:
  Ref(pugi::xml_node&&, Node* parent, pn_gsdml*);
  Ref(Ref&&) = default;

  // Attributes
  eValueDataType _DataType; // "Bit"
  ushort _ByteOffset;
  ushort _BitOffset;
  ushort _BitLength;
  double _DefaultValue;
  std::string _DefaultValueString;  // This is an extra field for storing the original value unparsed. For
                                    // instance when datatype is "VisibleString"
  ValueList<double> _AllowedValues; // Using double here works for all available data types...use more memory
                                    // though but it's fine...
  ushort _Length;
  bool _Changeable;
  bool _Visible;
  std::shared_ptr<std::string> _Text;
  std::shared_ptr<ValueItem> _ValueItem;
  std::string _ID; // i.e. "Diag:Load voltage". This is used to distinguish some values when we're not using
                   // menus. Otherwise this is implicitly stored as the reference for the map container
};

class MenuItem : public Node
{
public:
  MenuItem(pugi::xml_node&&, Node* parent, pn_gsdml*);
  MenuItem(MenuItem&&) = default;
  void linkMenuRefs(pugi::xml_node&, std::unordered_map<std::string, std::shared_ptr<MenuItem>>&);

  // ID is key in the parent map...

  // Elements
  std::shared_ptr<std::string> _Name;
  std::vector<std::shared_ptr<Ref>> _ParameterRefs;
  std::vector<std::shared_ptr<MenuItem>> _MenuRefs;
};

class ParameterRecordDataItem : public Node
{
public:
  ParameterRecordDataItem(pugi::xml_node&&, pn_gsdml*);
  ParameterRecordDataItem(ParameterRecordDataItem&&);
  ~ParameterRecordDataItem();
  unsigned char* getData() const;

  // Attributes
  ushort _Index;
  ushort _Length;
  ushort _TransferSequence;

  // Elements
  std::shared_ptr<std::string> _Name;
  std::vector<GSDML::Const> _Const;
  std::map<std::string, std::shared_ptr<Ref>> _Ref;
  std::unordered_map<std::string, std::shared_ptr<MenuItem>> _MenuLookupList; // For lookups only
  std::vector<std::shared_ptr<MenuItem>>
      _MenuList; // For the menu to be iterated over in the order they appear

private:
  unsigned char* _data;
};

/*
The <Input> and <Output> elements have the optional attribute Consistency, which
can be “Item consistency” (default if not given) or “All items consistency”.
*/
class Input
{
public:
  Input(pugi::xml_node&&, pn_gsdml* gsdml);
  Input() = default;
  Input(Input&&) = default;

  // TODO parse Consistency?? Something we need to use?

  // Elements
  std::vector<DataItem> _DataItem;
};

class Output
{
public:
  Output(pugi::xml_node&&, pn_gsdml* gsdml);
  Output() = default;
  Output(Output&&) = default;

  // TODO parse Consistency??

  // Elements
  std::vector<DataItem> _DataItem;
};

class IOData
{
public:
  IOData(pugi::xml_node&&, pn_gsdml* gsdml);
  IOData() {}
  IOData(IOData&&) = default;
  Input _Input;
  Output _Output;

  // Convienience method
  bool has_data() const { return _Input._DataItem.size() || _Output._DataItem.size(); }
};

class MAUTypeItem
{
public:
  MAUTypeItem(pugi::xml_node&&, pn_gsdml* gsdml);
  MAUTypeItem(MAUTypeItem&&) = default;
  // Attributes
  uint _Value;
  bool _AdjustSupported;
};
class SubmoduleItem
{
public:
  SubmoduleItem(pugi::xml_node&&, pn_gsdml*, eSubmoduleItemType = SubmoduleItemType_Submodule);
  SubmoduleItem(eSubmoduleItemType = SubmoduleItemType_Submodule);
  SubmoduleItem(SubmoduleItem&&) = default;
  virtual ~SubmoduleItem() = default;

  uint _SubmoduleIdentNumber;
  uint _SubslotNumber; // Some SubmoduleItems are fixed and thus have a SubslotNumber specified, some don't.
                       // But we keep it here for consistency
  GSDML::ValueList<uint> _FixedInSubslots;
  GSDML::ValueList<uint> _Writeable_IM_Records;
  bool _MayIssueProcessAlarm;
  std::shared_ptr<std::string> _Text; // Some SubmoduleItems have a TextId attribute and some don't...
  std::string _ID;

  // Elements
  IOData _IOData;
  std::vector<ParameterRecordDataItem> _RecordDataList;
  ModuleInfo _ModuleInfo;

  // Type tag...
  eSubmoduleItemType _SubmoduleItemType;
};

class VirtualSubmoduleItem : public SubmoduleItem
{
public:
  VirtualSubmoduleItem(pugi::xml_node&&, pn_gsdml* gsdml);
  VirtualSubmoduleItem(VirtualSubmoduleItem&&) = default;

  // No more known attributes specific to VirtualSubmoduleItem

  // No more known elements specific to VirtualSubmoduleItem
};

class PortSubmoduleItem : public SubmoduleItem
{
public:
  PortSubmoduleItem(pugi::xml_node&&, pn_gsdml* gsdml);
  PortSubmoduleItem(PortSubmoduleItem&&) = default;

  // Attributes
  uint _MaxPortRxDelay;
  uint _MaxPortTxDelay;
  std::string _LinkStateDiagnosisCapability; // i.e "Up+Down"
  bool _PortDeactivationSupported;
  uint _MAUTypes;
  bool _SupportsRingportConfig;
  bool _IsDefaultRingport;
  GSDML::ValueList<float> _Writeable_IM_Records; // i.e "1 2 3"
  bool _CheckMAUTypeSupported;
  GSDML::ValueList<float> _FiberOpticTypes; // i.e "2 3"
  bool _PowerBudgetControlSupported;
  bool _ShortPreamble100MBitSupported;
  bool _CheckMAUTypeDifferenceSupported;

  // Elements specific to PortSubmoduleItem
  std::vector<MAUTypeItem> _MAUTypeList;
};

class RT_Class3Properties
{
public:
  RT_Class3Properties(pugi::xml_node&&, pn_gsdml* gsdml);
  RT_Class3Properties() : _StartupMode("Legacy") {}
  RT_Class3Properties(RT_Class3Properties&&) = default;

  // Attributes
  TokenList _StartupMode; // i.e "Legacy;Advanced"
  std::string _ForwardingMode;
  uint _MaxBridgeDelay;
  uint _MaxNumberIR_FrameData;
};

class TimingProperties
{
public:
  TimingProperties(pugi::xml_node&&, pn_gsdml* gsdml);
  TimingProperties() : _SendClock("8 16 32 64 128"), _ReductionRatioPow2("1 2 4 8 16 32 64 128 256 512") {}
  TimingProperties(TimingProperties&&) = default;

  // Attributes
  ValueList<uint> _SendClock;          // i.e "8 16 32 64 128"
  ValueList<uint> _ReductionRatioPow2; // i.e "1 2 4 8 16 32 64 128 256 512"
};

class RT_Class3TimingProperties
{
public:
  RT_Class3TimingProperties(pugi::xml_node&&, pn_gsdml* gsdml);
  RT_Class3TimingProperties()
      : _SendClock("8 12 16 20 24 28 32 36 40 44 48 52 56 60 64 68 72 76 80 84 88 92 96 100 104 108 112 116 "
                   "120 124 128"),
        _ReductionRatioPow2("1 2 4 8 16")
  {
  }
  RT_Class3TimingProperties(RT_Class3TimingProperties&&) = default;

  // Attributes
  ValueList<uint> _SendClock; // i.e "8 12 16 20 24 28 32 36 40 44 48 52 56 60 64 68 72 76 80 84 88 92 96 100
                              // 104 108 112 116 120 124 128"
  ValueList<uint> _ReductionRatioPow2; // i.e "1 2 4 8 16"
};

class ApplicationRelations
{
public:
  ApplicationRelations(pugi::xml_node&&, pn_gsdml* gsdml);
  ApplicationRelations() : _StartupMode("Legacy") {}
  ApplicationRelations(ApplicationRelations&&) = default;

  // Attributes
  uint _NumberOfAR;
  TokenList _StartupMode; // i.e "Legacy;Advanced"

  // Elements
  TimingProperties _TimingProperties;
  RT_Class3TimingProperties _RT_Class3TimingProperties;
};

class SynchronisationMode
{
public:
  SynchronisationMode(pugi::xml_node&&, pn_gsdml* gsdml);
  SynchronisationMode() : _SupportedSyncProtocols("") {}
  SynchronisationMode(SynchronisationMode&&) = default;

  // Attributes
  std::string _SupportedRole; // i.e "SyncSlave"
  uint _MaxLocalJitter;
  uint _T_PLL_MAX;
  TokenList _SupportedSyncProtocols; // i.e "PTCP" Not really a list but since the name of the attribute is in
                                     // plural one can just guess...
};

class MediaRedundancy
{
public:
  MediaRedundancy(pugi::xml_node&&, pn_gsdml* gsdml);
  MediaRedundancy() = default;
  MediaRedundancy(MediaRedundancy&&) = default;

  std::string _SupportedRole; // i.e "Client"
  bool _AdditionalProtocolsSupported;
};

class InterfaceSubmoduleItem : public SubmoduleItem
{
public:
  InterfaceSubmoduleItem(pugi::xml_node&&, pn_gsdml* gsdml);
  InterfaceSubmoduleItem();
  InterfaceSubmoduleItem(InterfaceSubmoduleItem&&) = default;
  // Attributes
  TokenList _SupportedRT_Classes; // i.e "RT_CLASS_1;RT_CLASS_3"
  TokenList _SupportedProtocols;  // i.e "SNMP;LLDP"
  bool _DCP_HelloSupported;
  bool _DelayMeasurementSupported;

  RT_Class3Properties _RT_Class3Properties;
  SynchronisationMode _SynchronisationMode;
  ApplicationRelations _ApplicationRelations;
  MediaRedundancy _MediaRedundancy;
};

class SubmoduleItemRef
{
public:
  SubmoduleItemRef(pugi::xml_node&&, pn_gsdml* gsdml);
  SubmoduleItemRef(SubmoduleItemRef&&) = default;
  std::shared_ptr<SubmoduleItem> _SubmoduleItemTarget;
  GSDML::ValueList<uint> _AllowedInSubslots; // Duh ... allowed in
  GSDML::ValueList<uint> _UsedInSubslots;    // Default if none selected
  GSDML::ValueList<uint> _FixedInSubslots;   // Must be in
};

class ModuleItem
{
public:
  ModuleItem(pugi::xml_node&&, pn_gsdml* gsdml, eModuleItemType = ModuleItemType_Module);
  ModuleItem(ModuleItem&&) = default;
  virtual ~ModuleItem() = default;

  // Attributes
  uint _ModuleIdentNumber;
  ValueList<uint> _PhysicalSubslots;
  std::string _ID;

  // Elements
  ModuleInfo _ModuleInfo;
  std::map<std::string, std::shared_ptr<SubmoduleItem>> _VirtualSubmoduleList;
  std::map<std::string, std::shared_ptr<SubmoduleItemRef>> _UseableSubmodules;

  // Type tag...
  eModuleItemType _ModuleItemType;
};

class ModuleItemRef
{
public:
  ModuleItemRef(pugi::xml_node&&, pn_gsdml* gsdml);
  ModuleItemRef(ModuleItemRef&&) = default;
  std::shared_ptr<ModuleItem> _ModuleItemTarget;
  GSDML::ValueList<uint> _AllowedInSlots;
  GSDML::ValueList<uint> _FixedInSlots;
};

class DeviceIdentity
{
public:
  DeviceIdentity(pugi::xml_node&&, pn_gsdml* gsdml);
  DeviceIdentity(DeviceIdentity&&) = default;

  // Attributes
  uint _VendorID;
  uint _DeviceID;

  // Simple Elements
  std::shared_ptr<std::string> _InfoText;
  std::string _VendorName;
};

class DeviceFunction
{
public:
  DeviceFunction(pugi::xml_node&&, pn_gsdml* gsdml);
  DeviceFunction(DeviceFunction&&) = default;

  // No Attributes

  // "Simple" Elements
  std::string _MainFamily;
  std::string _ProductFamily;
};

class DeviceAccessPointItem : public ModuleItem
{
public:
  DeviceAccessPointItem(pugi::xml_node&&, pn_gsdml* gsdml);
  DeviceAccessPointItem(DeviceAccessPointItem&&) = default;
  // Attributes
  std::string _PNIO_Version;
  std::string _RequiredSchemaVersion;
  GSDML::ValueList<uint> _PhysicalSlots;
  // _ModuleIdentNumber; Inherited
  ushort _MinDeviceInterval;
  std::string _ImplementationType;
  std::string _DNS_CompatibleName;
  GSDML::ValueList<uint> _FixedInSlots;
  ushort _ObjectUUID_LocalIndex;
  bool _MultipleWriteSupported;
  ushort _MaxSupportedRecordSize;
  bool _ParameterizationSpeedupSupported;
  ushort _PowerOnToCommReady;
  bool _DeviceAccessSupported;
  bool _SharedDeviceSupported;
  bool _CheckDeviceID_Allowed;
  bool _NameOfStationNotTransferable;
  ushort _NumberOfDeviceAccessAR;
  ushort _NumberOfSubmodules;
  bool _LLDP_NoD_Supported;
  // ValueList<uint> _PhysicalSubslots;  Inherited
  uint _ResetToFactoryModes;

  // Sub elements
  // ModuleInfo _ModuleInfo; Inherited
  CertificationInfo _CertificationInfo;
  std::vector<SubslotItem> _SubslotList;
  IOConfigData _IOConfigData;
  std::map<std::string, std::shared_ptr<ModuleItemRef>> _UseableModules;
  // std::map<std::string, std::shared_ptr<SubmoduleItem>> _VirtualSubmoduleList; Inherited
  std::map<std::string, std::shared_ptr<SubmoduleItem>> _SystemDefinedSubmoduleList;
  // std::map<std::string, std::shared_ptr<SubmoduleItemRef>> _UseableSubmodules; Inherited
};

} // namespace GSDML

#endif