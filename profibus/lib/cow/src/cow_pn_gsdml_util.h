#include "cow_pn_gsdml_classes.h"

//#define ATTR_VALUE_TO_STRING(type_id, value_ptr) attr_value_to_string(type_id, value_ptr, __FILE__, __LINE__) 

namespace GSDML
{

int string_to_datavalue(eValueDataType datatype, void* value, const char* str);

template<typename T>
int string_to_datavalue_impl(T* value, const char* str);

std::string datavalue_to_string(eValueDataType datatype, void* value);

template<typename T>
std::string datavalue_to_string_impl(T const* value);


/*
    Attribute values to string. These are using pwr_eType as type_id...
*/
//std::string attr_value_to_string(int type_id, void const* value_ptr, const char* _file, int _line);
std::string attr_value_to_string(int type_id, void const* value_ptr);
/*
    Attribute string to value. These are using pwr_eType as type_id...
*/
int attr_string_to_value(int type_id, void* value, const char* value_str);

template<typename T>
int attr_string_to_value_impl(T* value, const char* str);

template<>
int attr_string_to_value_impl(std::string* value, const char* str);

}

#include "cow_pn_gsdml_util.tpp"