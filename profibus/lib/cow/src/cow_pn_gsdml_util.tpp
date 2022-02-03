namespace GSDML
{

template<typename T>
int string_to_datavalue_impl(T* value, const char* str)
{
    T val;
    std::istringstream input(str, std::ios_base::in);    
    input >> val;
    val = (sizeof(T) == 4 ? htonl(val) : htons(val));
    //val = htonl(val);
    memcpy(value, &val, sizeof(val));
    return PB__SUCCESS;
}

int string_to_datavalue(eValueDataType datatype, void* value, const char* str)
{
    switch (datatype)
    {
        case ValueDataType_Integer16:
            return string_to_datavalue_impl(static_cast<int16_t*>(value), str);
            break;
        case ValueDataType_Unsigned16:
            return string_to_datavalue_impl(static_cast<uint16_t*>(value), str);
            break;
        case ValueDataType_Integer32:
            return string_to_datavalue_impl(static_cast<int32_t*>(value), str);
            break;
        case ValueDataType_Unsigned32:
            return string_to_datavalue_impl(static_cast<uint32_t*>(value), str);
            break;
        default:
            // TODO Add more datatypes and template specializations
            return PB__NYI;
    }

    return PB__SYNTAX; // Not possible to reach...
}

template<typename T>
std::string datavalue_to_string_impl(T const* value)
{   
    std::ostringstream output(std::ios_base::out);
    T val = (sizeof(T) == 4 ? ntohl(*value) : ntohs(*value));
    output << val;
    return output.str();
}

std::string datavalue_to_string(eValueDataType datatype, void* value)
{
    switch (datatype)
    {
        case ValueDataType_Integer16:
            return datavalue_to_string_impl(static_cast<int16_t*>(value));
            break;
        case ValueDataType_Unsigned16:
            return datavalue_to_string_impl(static_cast<uint16_t*>(value));
            break;
        case ValueDataType_Integer32:
            return datavalue_to_string_impl(static_cast<int32_t*>(value));
            break;
        case ValueDataType_Unsigned32:
            return datavalue_to_string_impl(static_cast<uint32_t*>(value));
            break;
        default:
            return "";
    }
}

//std::string attr_value_to_string(int type_id, void const* value_ptr, const char* _file, int _line)
std::string attr_value_to_string(int type_id, void const* value_ptr)
{
  std::ostringstream result(std::ios_base::out);
  
  if (value_ptr == 0)
  {
    result << "Undefined (Null pointer argument)";
    return result.str();
  }

  switch (type_id)
  {
  case pwr_eType_Boolean:
    result << (*(pwr_tBoolean*)value_ptr ? "Yes" : "No");    
    break;
  case pwr_eType_Float32:
    result << *(pwr_tFloat32*)value_ptr;
    break;
  case pwr_eType_UInt8:
    result << *(pwr_tUInt8*)value_ptr;    
    break;
  case pwr_eType_UInt16:
    result << *(pwr_tUInt16*)value_ptr;    
    break;
  case pwr_eType_UInt32:
    result << *(pwr_tUInt32*)value_ptr;    
    break;
  case pwr_eType_Int8:
    result << *(pwr_tInt8*)value_ptr;    
    break;
  case pwr_eType_Int16:
    result << *(pwr_tInt16*)value_ptr;    
    break;
  case pwr_eType_Int32:
    result << *(pwr_tInt32*)value_ptr;    
    break;
  case pwr_eType_String:
  {
    result << *(std::string*)value_ptr;    
    break;
  }
  default:;      
  }

  return result.str();
}

template<typename T>
int attr_string_to_value_impl(T* value, const char* str)
{
    T val;
    std::istringstream input(str, std::ios_base::in);    
    input >> val;
    val = (sizeof(T) == 4 ? htonl(val) : htons(val));
    //val = htonl(val);
    memcpy(value, &val, sizeof(val));
    return PB__SUCCESS;
}

// String specialization
template<>
int attr_string_to_value_impl(std::string* value, const char* str)
{    
    *value = std::string(str);
    return PB__SUCCESS;
}

int attr_string_to_value(int type_id, void* value, const char* value_str)
{
  switch (type_id)
  {
    case pwr_eType_Boolean:
      return attr_string_to_value_impl(static_cast<pwr_tBoolean*>(value), value_str);
      break;
    case pwr_eType_Float32:
      return attr_string_to_value_impl(static_cast<pwr_tFloat32*>(value), value_str);
      break;
    case pwr_eType_Int32:
      return attr_string_to_value_impl(static_cast<pwr_tInt32*>(value), value_str);
      break;
    case pwr_eType_UInt32:
      return attr_string_to_value_impl(static_cast<pwr_tUInt32*>(value), value_str);
      break;
    case pwr_eType_String:
      return attr_string_to_value_impl(static_cast<std::string*>(value), value_str);
      break;
    default:
      return PB__NYI;
  }
  return PB__SYNTAX;
}

}
