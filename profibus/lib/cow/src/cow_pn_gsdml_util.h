#ifndef COW_PN_GSDML_UTIL__H
#define COW_PN_GSDML_UTIL__H

//#define ATTR_VALUE_TO_STRING(type_id, value_ptr) attr_value_to_string(type_id, value_ptr, __FILE__, __LINE__) 

namespace GSDML
{

//int string_to_datavalue(eValueDataType datatype, void* value, const char* str);

// template<typename T>
// int string_to_datavalue_impl(T* value, const char* str);

//std::string datavalue_to_string(eValueDataType datatype, void* value);

// template<typename T>
// std::string datavalue_to_string_impl(T const* value);


/*
    Attribute values to string. These are using pwr_eType as type_id...
*/
//std::string attr_value_to_string(int type_id, void const* value_ptr, const char* _file, int _line);
std::string attr_value_to_string(int type_id, void const* value_ptr);

/*
    Attribute string to value. These are using pwr_eType as type_id...
*/
//int attr_string_to_value(int type_id, void* value, const char* value_str);

// template<typename T>
// std::string datavalue_to_string_impl(T const* value)
// {   
//     std::ostringstream output(std::ios_base::out);
//     T val = (sizeof(T) == 4 ? ntohl(*value) : ntohs(*value));
//     output << val;
//     return output.str();
// }

// template<typename T>
// int attr_string_to_value_impl(T* value, const char* str)
// {
//     T val;
//     std::istringstream input(str, std::ios_base::in);    
//     input >> val;
//     val = (sizeof(T) == 4 ? htonl(val) : htons(val));
//     //val = htonl(val);
//     memcpy(value, &val, sizeof(val));
//     return PB__SUCCESS;
// }

// // String specialization
// template<>
// int attr_string_to_value_impl(std::string* value, const char* str)
// {    
//     *value = std::string(str);
//     return PB__SUCCESS;
// }

// template<typename T>
// int attr_string_to_value_impl(T* value, const char* str);

// template<>
// int attr_string_to_value_impl(std::string* value, const char* str);

// template<typename T>
// int string_to_datavalue_impl(T* value, const char* str)
// {
//     T val;
//     std::istringstream input(str, std::ios_base::in);    
//     input >> val;
//     val = (sizeof(T) == 4 ? htonl(val) : htons(val));
//     //val = htonl(val);
//     memcpy(value, &val, sizeof(val));
//     return PB__SUCCESS;
// }

template<typename T>
T reverse_type(const T in_type )
{
  T return_value;
  char *type_to_convert = ( char* ) & in_type;
  char *return_type = ( char* ) & return_value;
  switch(sizeof(T))
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

}

#endif