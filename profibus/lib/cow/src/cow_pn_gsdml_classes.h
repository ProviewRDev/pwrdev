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
//#include <cstdint>
#include "co_pugixml.hpp"

class pn_gsdml;

namespace GSDML
{
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
  To iterate over all possible values of a list like this:
  for (auto& item : list.getList())
    for (auto i = item.first; i <= item.second; i++)
      whatever;
*/
template <typename T> class ValueList
{
public:
  ValueList(char const* input)
  {
    // Lets parse it...
    std::istringstream buf(input, std::ios_base::in);

    std::string value;
    std::string rangeDelimeter("..");
    while (buf >> value)
    {
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

  bool inList(T value)
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

  std::vector<std::pair<T, T>>& getList() { return listItems; }

private:
  std::vector<std::pair<T, T>> listItems;
};

class TokenList
{
public:
  TokenList(char const* input);
  ~TokenList() {}
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

private:
  std::vector<unsigned char*> _data;
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
  std::string _Id;
  bool _UseAsBits;
  std::shared_ptr<std::string> _Text;

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
  ModuleInfo(ModuleInfo&&) = default;

  std::shared_ptr<std::string> _Name;
  std::shared_ptr<std::string> _InfoText;
  std::string _VendorName;
  std::string _OrderNumber;
  std::string _SoftwareRelease;
  std::string _HardwareRelease;
  std::shared_ptr<std::string> _CategoryItemText;
  std::shared_ptr<std::string> _CategoryItemInfoText;
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
  CertificationInfo(pugi::xml_node&&, pn_gsdml*);
  CertificationInfo(CertificationInfo&&) = default;
  std::string _ConformanceClass;
  std::string _ApplicationClass;
  std::string _NetloadClass;
};

class IOConfigData
{
public:
  IOConfigData(pugi::xml_node&& IOConfigData, pn_gsdml* gsdml);
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
  int _Content;
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
  // ValueItemTarget="Diag:Load voltage"
  eValueDataType _DataType; // "Bit"
  ushort _ByteOffset;
  ushort _BitOffset;
  ushort _BitLength;
  float _DefaultValue; // Using float works for all available data types...
  ValueList<float>
      _AllowedValues; // Using float works for all available data types...
  bool _Changeable;
  bool _Visible;
  std::shared_ptr<std::string> _Text;
  std::shared_ptr<ValueItem> _ValueItem;
  // ID="Diag:Load voltage"
};

class MenuItem : public Node
{
public:
  MenuItem(pugi::xml_node&&, Node* parent, pn_gsdml*);
  MenuItem(MenuItem&&) = default;
  void
  linkMenuRefs(pugi::xml_node&,
               std::unordered_map<std::string, std::shared_ptr<MenuItem>>&);

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
  unsigned char* getData();

  // Attributes
  ushort _Index;
  ushort _Length;
  ushort _TransferSequence;

  // Elements
  std::shared_ptr<std::string> _Name;
  std::vector<GSDML::Const> _Const;
  std::map<std::string, std::shared_ptr<Ref>> _Ref;
  std::unordered_map<std::string, std::shared_ptr<MenuItem>>
      _MenuLookupList; // For lookups only
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
  Input(Input&&) = default;

  // TODO parse Consistency?? Something we need to use?

  // Elements
  std::vector<DataItem> _DataItem;
};

class Output
{
public:
  Output(pugi::xml_node&&, pn_gsdml* gsdml);
  Output(Output&&) = default;

  // TODO parse Consistency??

  // Elements
  std::vector<DataItem> _DataItem;
};

class IOData
{
public:
  IOData(pugi::xml_node&&, pn_gsdml* gsdml);
  IOData(IOData&&) = default;
  Input _Input;
  Output _Output;
};

class VirtualSubmoduleItem
{
public:
  VirtualSubmoduleItem(pugi::xml_node&&, pn_gsdml* gsdml);
  VirtualSubmoduleItem(VirtualSubmoduleItem&&) = default;
  // Attributes
  uint _SubmoduleIdentNumber;
  GSDML::ValueList<uint> _FixedInSubslots;
  GSDML::ValueList<uint> _Writeable_IM_Records;
  bool _MayIssueProcessAlarm;

  // Elements
  IOData _IOData;
  std::vector<ParameterRecordDataItem> _RecordDataList;
  ModuleInfo _ModuleInfo;
};

class ModuleItem
{
public:
  ModuleItem(pugi::xml_node&&, pn_gsdml* gsdml);
  ModuleItem(ModuleItem&&) = default;
  uint _ModuleIdentNumber;
  ModuleInfo _ModuleInfo;
  std::map<std::string, std::shared_ptr<VirtualSubmoduleItem>>
      _VirtualSubmoduleList;
};

class ModuleItemRef
{
public:
  ModuleItemRef(pugi::xml_node&&, pn_gsdml* gsdml);
  ModuleItemRef(ModuleItemRef&&) = default;
  std::shared_ptr<ModuleItem> _ModuleItemTarget;
  GSDML::ValueList<uint> _AllowedInSlots;
};

class DeviceAccessPointItem
{
public:
  DeviceAccessPointItem(pugi::xml_node&&, pn_gsdml* gsdml);
  DeviceAccessPointItem(DeviceAccessPointItem&&) = default;
  // Attributes
  std::string _PNIO_Version;
  std::string _RequiredSchemaVersion;
  // ID="DIM 1"
  GSDML::ValueList<uint> _PhysicalSlots;
  uint _ModuleIdentNumber;
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

  // Sub elements
  ModuleInfo _ModuleInfo;
  std::vector<SubslotItem> _SubslotList;
  IOConfigData _IOConfigData;
  std::map<std::string, std::shared_ptr<ModuleItemRef>> _UseableModules;
};

} // namespace GSDML

#endif