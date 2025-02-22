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

#ifndef wb_cdrep_h
#define wb_cdrep_h

#include "wb_name.h"
#include "wb_ldh.h"

class wb_adrep;
class wb_bdrep;
class wb_mvrep;
class wb_orep;
class wb_orepdbs;
class wb_merep;
class wb_attribute;

class wb_cdrep {
  int m_nRef;
  wb_orep* m_orep;
  pwr_tStatus m_sts;
  wb_merep* m_merep;

public:
  wb_cdrep();
  wb_cdrep(wb_adrep*); // x = other_object
  wb_cdrep(const wb_orep&); // x = other orep
  wb_cdrep(wb_mvrep*, const wb_orep&);
  wb_cdrep(wb_mvrep*, pwr_tCid);
  wb_cdrep(wb_mvrep*, wb_name);
  ~wb_cdrep();

  void unref();
  wb_cdrep* ref();

  // wb_object& operator=(const wb_orep&);

  size_t size(pwr_eBix bix);
  pwr_mClassDef flags();
  pwr_tCid cid();

  const char* name() const;
  wb_name longName();
  void name(const char* name);
  void name(wb_name* name);

  wb_bdrep* bdrep(pwr_tStatus* sts, pwr_eBix bix);
  wb_bdrep* bdrep(pwr_tStatus* sts, const char* bname);
  wb_adrep* adrep(pwr_tStatus* sts, const char* aname);
  wb_orep* classBody(pwr_tStatus* sts, const char* bname);
  wb_cdrep* super(pwr_tStatus* sts);

  void templateBody(pwr_tStatus* sts, pwr_eBix bix, void* p, pwr_tOid oid);
  void attrTemplateBody(
      pwr_tStatus* sts, pwr_eBix bix, void* p, wb_attribute& a);
  void dbCallBack(pwr_tStatus* sts, ldh_eDbCallBack cb, char** methodName,
      pwr_sDbCallBack** o);
  wb_orep* menu(pwr_tStatus* sts, void** o);
  wb_orep* menuAfter(pwr_tStatus* sts, wb_orep* orep, void** o);
  wb_orep* menuFirst(pwr_tStatus* sts, wb_orep* orep, void** o);

  pwr_tTime ohTime();
  pwr_tTime modTime();
  pwr_tTime structModTime();
  void convertSubClass(
      pwr_tCid cid, wb_merep* merep, void* body_source, void* body_target);
  void convertObject(wb_merep* merep, void* rbody, void* dbody,
      size_t* cnv_rbody_size, size_t* cnv_dbody_size, void** cnv_rbody,
      void** cnv_dbody);

  void updateTemplateSubClass(
      wb_adrep* subattr, char* body, pwr_tOid oid, pwr_tOid toid, int aoffs);
  void updateTemplate(pwr_eBix bix, void* b, pwr_tOid oid, pwr_tOid toid);
  pwr_tStatus sts()
  {
    return m_sts;
  }
  ldh_eVolRep vtype() const;
  bool renameClass(pwr_tStatus* sts, wb_name& name);
  void merep(wb_merep* me)
  {
    m_merep = me;
  }
};

#endif
