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

#ifndef wb_db_h
#define wb_db_h

#include "db_cxx.h"
#include "wb_import.h"

class wb_name;

#pragma pack(push)
#pragma pack(4)
typedef struct {
  pwr_tOid oid; /**< object identifier */
  pwr_tCid cid; /**< class identifier */
  pwr_tOid poid; /**< object identifier of parent */
  pwr_tObjName name; /**< name of object */
  pwr_tObjName normname; /**< normalized object name. */
  pwr_tTime time; /**< time of last change in object header */
  pwr_tOid boid; /**< object before this object. */
  pwr_tOid aoid; /**< object after this object. */
  pwr_tOid foid; /**< first child object. */
  pwr_tOid loid; /**< last child object. */

  pwr_mClassDef flags;
  struct {
    pwr_tTime time;
    pwr_tUInt32 size;
  } body[2]; /**< bodies */
} db_sObject;
#pragma pack(pop)

class wb_db_txn;
class wb_db_ohead;
class wb_destination;

#define wb_db_txn DbTxn

class wb_db : public wb_import {
public:
  pwr_tVid m_vid;
  pwr_tCid m_cid;
  pwr_tObjName m_volumeName;
  char m_fileName[512];

  DbEnv* m_env;
  Db* m_t_ohead;
  Db* m_t_rbody;
  Db* m_t_dbody;
  Db* m_t_class;
  Db* m_t_name;
  Db* m_t_info;

  wb_db_txn* m_txn;

public:
  wb_db();
  wb_db(pwr_tVid vid);
  //~wb_db();

  pwr_tCid cid()
  {
    return m_cid;
  }
  pwr_tVid vid()
  {
    return m_vid;
  }
  // pwr_tTime time() { return m_volume.time;}
  char* volumeName()
  {
    return m_volumeName;
  }

  pwr_tOid new_oid(wb_db_txn* txn);
  pwr_tOid new_oid(wb_db_txn* txn, pwr_tOid oid);

  void close();
  void open(const char* fileName);
  void openDb(bool useTxn);

  void copy(wb_export& e, const char* fileName);
  void create(
      pwr_tVid vid, pwr_tCid cid, const char* volumeName, const char* fileName);

  int del_family(wb_db_txn* txn, Dbc* cp, pwr_tOid poid);

  wb_db_txn* begin(wb_db_txn* txn);

  bool commit(pwr_tStatus* sts);

  bool abort(pwr_tStatus* sts);

  // void adopt(wb_db_txn *txn, wb_db_ohead &o, wb_destination &dest);

  // void unadopt(wb_db_txn *txn, wb_db_ohead &o);

  bool deleteFamily(pwr_tStatus* sts, wb_db_ohead* o);

  // bool deleteOset(pwr_tStatus *sts, wb_oset *o);

  bool importVolume(wb_export& e);

  bool importHead(pwr_tOid oid, pwr_tCid cid, pwr_tOid poid, pwr_tOid boid,
      pwr_tOid aoid, pwr_tOid foid, pwr_tOid loid, const char* name,
      const char* normname, pwr_mClassDef flags, pwr_tTime ohTime,
      pwr_tTime rbTime, pwr_tTime dbTime, size_t rbSize, size_t dbSize);

  bool importRbody(pwr_tOid oid, size_t size, void* body);

  bool importDbody(pwr_tOid oid, size_t size, void* body);

  bool importDocBlock(pwr_tOid oid, size_t size, char* block)
  {
    return true;
  }

  bool importMeta(dbs_sMenv* mep);

  void checkClassList(pwr_tOid oid, pwr_tCid cid, bool update);
};

class wb_db_info {
public:
  struct {
    pwr_tVid vid;
    pwr_tCid cid;
    pwr_tTime time;
    pwr_tObjName name;
  } m_volume;

  wb_db* m_db;

  Dbt m_key;
  Dbt m_data;

  wb_db_info(wb_db* db);
  //~wb_db_info();

  void put(wb_db_txn* txn);
  void get(wb_db_txn* txn);

  pwr_tCid cid()
  {
    return m_volume.cid;
  }
  pwr_tVid vid()
  {
    return m_volume.vid;
  }
  pwr_tTime time()
  {
    return m_volume.time;
  }
  char* name()
  {
    return m_volume.name;
  }

  void cid(pwr_tCid cid)
  {
    m_volume.cid = cid;
  }
  void vid(pwr_tVid vid)
  {
    m_volume.vid = vid;
  }
  void time(pwr_tTime time)
  {
    m_volume.time = time;
  }
  void name(char const* name);
};

class wb_db_ohead {
public:
  db_sObject m_o;
  pwr_tOid m_oid;

  wb_db* m_db;
  Dbt m_key;
  Dbt m_data;

  Dbc* m_dbc;

  wb_db_ohead();
  wb_db_ohead(wb_db* db);
  wb_db_ohead(wb_db* db, pwr_tOid oid);
  wb_db_ohead(wb_db* db, wb_db_txn* txn, pwr_tOid oid);
  wb_db_ohead(wb_db* db, pwr_tOid oid, pwr_tCid cid, pwr_tOid poid,
      pwr_tOid boid, pwr_tOid aoid, pwr_tOid foid, pwr_tOid loid,
      const char* name, const char* normname, pwr_mClassDef flags,
      pwr_tTime ohTime, pwr_tTime rbTime, pwr_tTime dbTime, size_t rbSize,
      size_t dbSize);

  wb_db_ohead& get(wb_db_txn* txn);
  wb_db_ohead& get(wb_db_txn* txn, pwr_tOid oid);

  void setDb(wb_db* db)
  {
    m_db = db;
  }

  int put(wb_db_txn* txn);
  int del(wb_db_txn* txn);

  pwr_tOid oid()
  {
    return m_o.oid;
  }
  pwr_tVid vid()
  {
    return m_o.oid.vid;
  }
  pwr_tOix oix()
  {
    return m_o.oid.oix;
  }
  pwr_tCid cid()
  {
    return m_o.cid;
  }
  pwr_tOid poid()
  {
    return m_o.poid;
  }
  pwr_tOid foid()
  {
    return m_o.foid;
  }
  pwr_tOid loid()
  {
    return m_o.loid;
  }
  pwr_tOid boid()
  {
    return m_o.boid;
  }
  pwr_tOid aoid()
  {
    return m_o.aoid;
  }
  pwr_tTime ohTime()
  {
    return m_o.time;
  }

  const char* name()
  {
    return m_o.name;
  }

  const char* normname()
  {
    return m_o.normname;
  }

  pwr_mClassDef flags()
  {
    return m_o.flags;
  }

  size_t rbSize()
  {
    return m_o.body[0].size;
  }
  size_t dbSize()
  {
    return m_o.body[1].size;
  }
  pwr_tTime rbTime()
  {
    return m_o.body[0].time;
  }
  pwr_tTime dbTime()
  {
    return m_o.body[1].time;
  }

  void name(wb_name& name);
  void name(pwr_tOid& oid);

  void oid(pwr_tOid oid)
  {
    m_o.oid = m_oid = oid;
  }

  void cid(pwr_tCid cid)
  {
    m_o.cid = cid;
  }
  void poid(pwr_tOid oid)
  {
    m_o.poid = oid;
  }
  void foid(pwr_tOid oid)
  {
    m_o.foid = oid;
  }
  void loid(pwr_tOid oid)
  {
    m_o.loid = oid;
  }
  void boid(pwr_tOid oid)
  {
    m_o.boid = oid;
  }
  void aoid(pwr_tOid oid)
  {
    m_o.aoid = oid;
  }
  void flags(pwr_mClassDef flags)
  {
    m_o.flags = flags;
  }

  void rbSize(size_t size)
  {
    m_o.body[0].size = size;
  }
  void dbSize(size_t size)
  {
    m_o.body[1].size = size;
  }
  void ohTime(pwr_tTime& time)
  {
    m_o.time = time;
  }
  void rbTime(pwr_tTime& time)
  {
    m_o.body[0].time = time;
  }
  void dbTime(pwr_tTime& time)
  {
    m_o.body[1].time = time;
  }

  void clear();

  void iter(void (*func)(pwr_tOid oid, db_sObject* op));
  void iter(wb_import& i);
};

class wb_db_name {
public:
  struct {
    pwr_tOid poid;
    pwr_tObjName normname;
  } m_k;

  struct {
    pwr_tOid oid;
    // pwr_tCid      cid;   // saved here to optimize tree traversal
    // pwr_mClassDef flags; // saved here to optimize tree traversal
  } m_d;

  wb_db* m_db;
  Dbt m_key;
  Dbt m_data;
  Dbc* m_dbc;

  wb_db_name(wb_db* db, wb_db_txn* txn);
  wb_db_name(wb_db* db, wb_db_ohead& o);
  // wb_db_name(wb_db *db, pwr_tOid, char *name);
  wb_db_name(wb_db* db, pwr_tOid poid, const char* name);
  wb_db_name(wb_db* db, pwr_tOid oid, pwr_tOid poid, const char* name);
  wb_db_name(wb_db* db, wb_db_txn* txn, pwr_tOid poid, wb_name& name);

  int get(wb_db_txn* txn);
  int put(wb_db_txn* txn);
  int del(wb_db_txn* txn);

  void name(wb_name& name);
  void iter(void (*print)(pwr_tOid poid, pwr_tObjName name, pwr_tOid oid));

  pwr_tOid oid()
  {
    return m_d.oid;
  }
};

class wb_db_class {
public:
  struct {
    pwr_tCid cid;
    pwr_tOid oid;
  } m_k;

  wb_db* m_db;

  Dbt m_key;
  Dbt m_data;
  Dbc* m_dbc;

  wb_db_class(wb_db* db);
  wb_db_class(wb_db* db, pwr_tCid cid);
  wb_db_class(wb_db* db, pwr_tCid cid, pwr_tOid oid);
  wb_db_class(wb_db* db, wb_db_ohead& o);
  wb_db_class(wb_db* db, wb_db_txn* txn, pwr_tCid cid);
  ~wb_db_class();

  bool succ(pwr_tOid oid);
  bool succClass(pwr_tCid cid);
  bool pred(pwr_tOid oid);
  int put(wb_db_txn* txn);
  int del(wb_db_txn* txn);

  pwr_tCid cid()
  {
    return m_k.cid;
  }
  pwr_tOid oid()
  {
    return m_k.oid;
  }

  void iter(void (*func)(pwr_tOid oid, pwr_tCid cid));
};

class wb_db_class_iterator {
  struct {
    pwr_tCid cid;
    pwr_tOid oid;
  } m_k;

  wb_db* m_db;
  Dbt m_key;
  Dbt m_data;
  Dbc* m_dbc;
  bool m_atEnd;
  int m_rc;

public:
  wb_db_class_iterator(wb_db* db);
  wb_db_class_iterator(wb_db* db, pwr_tCid cid);
  wb_db_class_iterator(wb_db* db, pwr_tCid cid, pwr_tOid oid);
  ~wb_db_class_iterator();

  bool atEnd()
  {
    return m_atEnd;
  }
  bool first();
  bool succObject();
  bool succClass();
  bool succClass(pwr_tCid cid);

  pwr_tOid oid()
  {
    return m_k.oid;
  }
  pwr_tCid cid()
  {
    return m_k.cid;
  }
  void oid(pwr_tOid oid)
  {
    m_k.oid = oid;
  }
  void cid(pwr_tCid cid)
  {
    m_k.cid = cid;
  }
};

class wb_db_dbody {
public:
  wb_db* m_db;

  pwr_tOid m_oid;
  size_t m_size;
  void* m_p;

  Dbt m_key;
  Dbt m_data;
  Dbc* m_dbc;

  wb_db_dbody(wb_db* db);
  wb_db_dbody(wb_db* db, pwr_tOid oid);
  wb_db_dbody(wb_db* db, pwr_tOid oid, size_t size, void* p);

  void oid(pwr_tOid oid)
  {
    m_oid = oid;
  }

  int get(wb_db_txn* txn, size_t offset, size_t size, void* p);
  int put(wb_db_txn* txn);
  int put(wb_db_txn* txn, size_t offset, size_t size, void* p);

  int del(wb_db_txn* txn);

  void iter(void (*print)(pwr_tOid oid));
  void iter(wb_import& i);
};

class wb_db_rbody {
public:
  wb_db* m_db;

  pwr_tOid m_oid;
  size_t m_size;
  void* m_p;

  Dbt m_key;
  Dbt m_data;
  Dbc* m_dbc;

  wb_db_rbody(wb_db* db);
  wb_db_rbody(wb_db* db, pwr_tOid oid);
  wb_db_rbody(wb_db* db, pwr_tOid oid, size_t size, void* p);

  void oid(pwr_tOid oid)
  {
    m_oid = oid;
  }

  int get(wb_db_txn* txn, size_t offset, size_t size, void* p);
  int put(wb_db_txn* txn);
  int put(wb_db_txn* txn, size_t offset, size_t size, void* p);

  int del(wb_db_txn* txn);

  void iter(void (*print)(pwr_tOid oid));
  void iter(wb_import& i);
};

#endif
