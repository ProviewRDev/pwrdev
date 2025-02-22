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

#ifndef wb_dbs_h
#define wb_dbs_h

#include "co_tree.h"
#include "wb_status.h"
#include "wb_ldh.h"
#include "wb_import.h"

class wb_vrep;
class CoDepend;

class wb_dbs : public wb_status, public wb_import {
public:
  typedef union {
    struct {
      pwr_tBit devOnly : 1;
      pwr_tBit exist : 1;
    } b;

    pwr_tBitMask m;

#define mOentry_devOnly 1
#define mOentry_exist 2
  } mOentry;

  struct sOentry;
  struct sNentry;
  struct sCentry;
  struct sVentry;

  struct sOentry {
    tree_sNode node;
    dbs_sObject o;
    dbs_sBody rbody;
    dbs_sBody dbody;
    mOentry flags;
    dbs_tRef ref;
    dbs_tRef oidref;

    sOentry* poep;
    sOentry* boep;
    sOentry* aoep;
    sOentry* foep;
    sOentry* loep;

    sOentry* o_ll;
  };

  /* Name entry, used in name table.  */

  struct sNentry {
    tree_sNode node;
    dbs_sName n;
    dbs_tRef ref;
    sOentry* oep;
  };

  struct sCentry {
    tree_sNode node;
    dbs_sClass c;
    dbs_tRef ref;
    sOentry* o_lh; // Header of object list
    sOentry* o_lt; // Tail of object list
  };

  struct sVentry {
    tree_sNode node;
    dbs_sVolRef v;
    dbs_sEnv env;
  };

  FILE* m_fp; /**< file pointer */
  char m_fileName[512]; /**< name of load file */
  //    wb_object    *m_o;		/* Volume object header.  */
  wb_vrep* m_v;
  dbs_sVolume m_volume;
  pwr_tOid m_oid;
  pwr_tUInt32 m_dvVersion;
  dbs_sSect m_sect[dbs_eSect_]; /**< section header */
  int m_rtonly;

  unsigned int m_warnings;
  unsigned int m_errors;

  unsigned int m_nObjects;
  unsigned int m_nTreeObjects;
  unsigned int m_nClassObjects;
  unsigned int m_nNameObjects;
  unsigned int m_nRbodyObjects;
  unsigned int m_nDbodyObjects;

  sOentry* m_oep; /* object entry of volume object */

  tree_sTable* m_oid_th;
  tree_sTable* m_name_th;
  tree_sTable* m_class_th;
  tree_sTable* m_vol_th;
  CoDepend* m_depend;

  wb_dbs();
  wb_dbs(wb_vrep*);

  ~wb_dbs();

  // wb_dbs& operator=(const wb_object&);

  operator bool() const;

  bool operator==(const wb_dbs&) const;

  void setFileName(const char* name);
  void getFileName(char* name);
  void setTime(const pwr_tTime t);
  void setRtonly(const int rtonly)
  {
    m_rtonly = rtonly;
  }

  void getAliasServer(sOentry* oep, void* p);
  char* pathName(sOentry* oep);
  void getMountServer(sOentry* oep, void* p);

  void buildSectName();
  void buildSectOid();
  void buildSectClass();
  void checkObject(sOentry* oep);
  pwr_tStatus closeFile(pwr_tBoolean doDelete);
  pwr_tBoolean createFile();
  // ldhi_sObjHead *getAliasServer(sLCB *lcbp, ldhi_sObjHead *o, pwr_tOid
  // *soid);
  // pwr_tStatus    getMountServer(sLCB *lcbp, ldhi_sObjHead *o, pwr_tOid
  // *soid);

  void cidInsert(
      pwr_tStatus* sts, pwr_tCid cid, pwr_sAttrRef* arp, sCentry** cep);
  void classInsert(sOentry* oep);
  pwr_tStatus openFile();
  pwr_tStatus writeSectFile(size_t size);
  pwr_tStatus writeSectDirectory();
  pwr_tStatus writeSectVolume();
  pwr_tStatus prepareSectVolref();
  pwr_tStatus writeSectVolref(size_t size);
  pwr_tStatus writeSectOid();
  pwr_tStatus writeSectObject();
  pwr_tStatus writeSectRbody();
  pwr_tStatus writeSectName();
  pwr_tStatus writeSectClass();
  pwr_tStatus writeSectDbody();
  pwr_tStatus writeReferencedVolumes();
  pwr_tUInt32 getDvVersion(wb_vrep* v);
  void setDepend(CoDepend* depend)
  {
    m_depend = depend;
  }

  virtual bool importVolume(wb_export& e);

  virtual bool importHead(pwr_tOid oid, pwr_tCid cid, pwr_tOid poid,
      pwr_tOid boid, pwr_tOid aoid, pwr_tOid foid, pwr_tOid loid,
      const char* name, const char* normname, pwr_mClassDef flags,
      pwr_tTime ohTime, pwr_tTime rbTime, pwr_tTime dbTime, size_t rbSize,
      size_t dbSize);

  virtual bool importRbody(pwr_tOid oid, size_t size, void* body);

  virtual bool importDbody(pwr_tOid oid, size_t size, void* body);

  virtual bool importDocBlock(pwr_tOid oid, size_t size, char* block)
  {
    return true;
  }

  virtual bool importMeta(dbs_sMenv* mep);

private:
};

#endif
