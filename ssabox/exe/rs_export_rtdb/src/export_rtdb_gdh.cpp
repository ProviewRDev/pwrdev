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

#if defined PWRE_CONF_RDKAFKA

#include "pwr.h"
#include "pwr_baseclasses.h"
#include "rt_gdh.h"
#include "rt_gdh_msg.h"
#include "co_cdh.h"
#include "export_rtdb_avro.h"
#include "export_rtdb_gdh.h"

const bool MAKE_VALUES_OPTIONAL = true;

// This function converts from latin1 encoding to utf8
static std::string to_utf8(char *s) {
  std::string res = "";
  std::string str(s);
  for (int i = 0; i < str.size(); i++) {
    unsigned char in = str[i];
    if (in < 128) {
      res += in;
    } else {
      res += 0xc2 + (in > 0xbf);
      res += (in & 0x3f) + 0x80;
    }
  }
  return res;
}

template<typename T>
T attr_to_val(pwr_sAttrRef* aref, void* val) {
  T a;
  a = *(T*)val;
  return a;
}

int encode_val(AvroEncoder &enc, pwr_eType type_id, bool is_ptr, pwr_sAttrRef* aref, void* val) {
  if (MAKE_VALUES_OPTIONAL && type_id < pwr_eType_Void) {
    enc.encodeUnionIndex(1);
  }
  switch(type_id) {
  case pwr_eType_Boolean: {
    pwr_tBoolean a = attr_to_val<pwr_tBoolean>(aref, val);
    enc.encodeBool(a ? true : false);
    break;
  }
  case pwr_eType_Float32: {
    enc.encodeFloat(attr_to_val<pwr_tFloat32>(aref, val));
    break;
  }
  case pwr_eType_Float64: {
    enc.encodeDouble(attr_to_val<pwr_tFloat64>(aref, val));
    break;
  }
  case pwr_eType_Char: {
    enc.encodeInt(attr_to_val<pwr_tChar>(aref, val));
    break;
  }
  case pwr_eType_Int8: {
    enc.encodeInt(attr_to_val<pwr_tInt8>(aref, val));
    break;
  }
  case pwr_eType_Int16: {
    enc.encodeInt(attr_to_val<pwr_tInt16>(aref, val));
    break;
  }
  case pwr_eType_Int32: {
    enc.encodeInt(attr_to_val<pwr_tInt32>(aref, val));
    break;
  }
  case pwr_eType_Int64: {
    enc.encodeLong(attr_to_val<pwr_tInt64>(aref, val));
    break;
  }
  case pwr_eType_UInt8: {
    enc.encodeInt(attr_to_val<pwr_tUInt8>(aref, val));
    break;
  }
  case pwr_eType_UInt16: {
    enc.encodeInt(attr_to_val<pwr_tUInt16>(aref, val));
    break;
  }
  case pwr_eType_UInt32:
  case pwr_eType_Mask:
  case pwr_eType_DisableAttr:
  case pwr_eType_Enum:
  case pwr_eType_NetStatus:
  case pwr_eType_Status: {
    enc.encodeInt(attr_to_val<pwr_tUInt32>(aref, val));
    break;
  }
  case pwr_eType_UInt64: {
    enc.encodeLong(attr_to_val<pwr_tUInt64>(aref, val));
    break;
  }
  case pwr_eType_String: {
    pwr_tString80 a;
    strcpy(a, *(pwr_tString80*)val);
    a[79] = 0;
    enc.encodeString(to_utf8(a));
    break;
  }
  case pwr_eType_Text: {
    pwr_tString256 a;
    strcpy(a, *(pwr_tText*)val);
    pwr_tString256 str;
    char* s;
    char* t;
    for (s = (char*)a, t = str; *s != 10 && *s != 0; s++, t++) {
      *t = *s;
    }
    *t = 0;
    str[255] = 0;
    enc.encodeString(to_utf8(str));
    break;
  }
  case pwr_eType_Objid: {
    pwr_tObjid a = attr_to_val<pwr_tObjid>(aref, val);
    enc.encodeBytes((uint8_t*)&a, sizeof(a));
    break;
  }
  case pwr_eType_AttrRef: {
    pwr_sAttrRef attrref;
    attrref = *(pwr_sAttrRef*)val;
    enc.encodeBytes((uint8_t*)&attrref, sizeof(attrref));
    break;
  }
  case pwr_eType_DataRef: {
    pwr_tDataRef dataref;
    dataref = *(pwr_tDataRef*)val;
    enc.encodeBytes((uint8_t*)&dataref, sizeof(dataref));
    break;
  }
  case pwr_eType_Time: {
    pwr_tTime a = attr_to_val<pwr_tTime>(aref, val);
    uint8_t tmp[sizeof(a.tv_sec)+sizeof(a.tv_nsec)];
    for (int i = 0; i < sizeof(a.tv_sec); i++) {
      tmp[i] = ((a.tv_sec >> (8*i)) & 0xFF);
    }
    for (int i = 0; i < sizeof(a.tv_nsec); i++) {
      tmp[i+sizeof(a.tv_sec)] = ((a.tv_nsec >> (8*i)) & 0xFF);
    }
    enc.encodeBytes(tmp, sizeof(a.tv_sec)+sizeof(a.tv_nsec));
    break;
  }
  case pwr_eType_DeltaTime: {
    pwr_tDeltaTime a = attr_to_val<pwr_tDeltaTime>(aref, val);
    uint8_t tmp[sizeof(a.tv_sec)+sizeof(a.tv_nsec)];
    for (int i = 0; i < sizeof(a.tv_sec); i++) {
      tmp[i] = ((a.tv_sec >> (8*i)) & 0xFF);
    }
    for (int i = 0; i < sizeof(a.tv_nsec); i++) {
      tmp[i+sizeof(a.tv_sec)] = ((a.tv_nsec >> (8*i)) & 0xFF);
    }
    enc.encodeBytes(tmp, sizeof(a.tv_sec)+sizeof(a.tv_nsec));
    break;
  }
  case pwr_eType_ObjectIx: {
    pwr_tObjectIx a = attr_to_val<pwr_tObjectIx>(aref, val);
    enc.encodeBytes((uint8_t*)&a, sizeof(a));
    break;
  }
  case pwr_eType_ClassId: {
    pwr_tClassId a = attr_to_val<pwr_tClassId>(aref, val);
    enc.encodeBytes((uint8_t*)&a, sizeof(a));
    break;
  }
  case pwr_eType_TypeId:
  case pwr_eType_CastId: {
    pwr_tTypeId a = attr_to_val<pwr_tTypeId>(aref, val);
    enc.encodeBytes((uint8_t*)&a, sizeof(a));
    break;
  }
  case pwr_eType_VolumeId: {
    pwr_tVolumeId a = attr_to_val<pwr_tVolumeId>(aref, val);
    enc.encodeBytes((uint8_t*)&a, sizeof(a));
    break;
  }
  case pwr_eType_RefId: {
    pwr_tSubid a = attr_to_val<pwr_tSubid>(aref, val);
    enc.encodeBytes((uint8_t*)&a, sizeof(a));
    break;
  }
  default:
    fprintf(stderr, "Not handling type %d\n", type_id);
    return GDH__CONVERT;
  }
  return GDH__SUCCESS;
}

std::string pwr_eType_to_str(pwr_eType tid) {
  std::string str;
  switch (tid) {
  case pwr_eType_Boolean:
    str = "boolean";
    break;
  case pwr_eType_Float32:
    str =  "float";
    break;
  case pwr_eType_Float64:
    str = "double";
    break;
  case pwr_eType_Char:
    str = "bytes";
    break;
  case pwr_eType_Int8:
  case pwr_eType_Int16:
  case pwr_eType_Int32:
  case pwr_eType_UInt8:
  case pwr_eType_UInt16:
  case pwr_eType_UInt32:
  case pwr_eType_Mask:
  case pwr_eType_DisableAttr:
  case pwr_eType_Enum:
  case pwr_eType_NetStatus:
  case pwr_eType_Status:
    str = "int";
    break;
  case pwr_eType_Int64:
  case pwr_eType_UInt64:
    str = "long";
    break;
  case pwr_eType_String:
  case pwr_eType_Text:
    str = "string";
    break;
  case pwr_eType_Objid:
  case pwr_eType_AttrRef:
  case pwr_eType_DataRef:
  case pwr_eType_ObjectIx:
  case pwr_eType_ClassId:
  case pwr_eType_TypeId:
  case pwr_eType_CastId:
  case pwr_eType_VolumeId:
  case pwr_eType_RefId:
    str = "bytes";
    break;
  case pwr_eType_Time:
  case pwr_eType_DeltaTime:
    str = "bytes";
    break;
  default:
    str = "";
  }
  if (MAKE_VALUES_OPTIONAL && str.size() > 0) {
    return "[\\\"null\\\",\\\"" + str + "\\\"]";
  }
  return str;
}

#endif
