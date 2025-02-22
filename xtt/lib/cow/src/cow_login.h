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

#ifndef cow_login_h
#define cow_login_h

#include "pwr.h"
#include "pwr_privilege.h"

typedef enum { login_mAttr_Navigator = 1 << 0 } login_mAttr;

class CoLogin {
private:
  static char m_username[40];
  static char m_password[40];
  static char m_ucpassword[40];
  static char m_group[40];
  static pwr_mPrv m_priv;
  static login_mAttr m_attribute;

public:
  void* parent_ctx;
  char name[80];
  void (*bc_success)(void*);
  void (*bc_cancel)(void*);
  char groupname[40];
  char password[40];

  CoLogin(void* wl_parent_ctx, const char* wl_name, const char* wl_groupname,
      void (*wl_bc_success)(void*), void (*wl_bc_cancel)(void*),
      pwr_tStatus* sts);

  virtual ~CoLogin();

  void activate_ok();
  void activate_cancel();

  virtual pwr_tStatus get_values()
  {
    return 0;
  }
  virtual void message(const char* new_label)
  {
  }
  virtual void pop()
  {
  }

  static pwr_tStatus user_check(
      const char* groupname, const char* username, const char* password);
  static pwr_tStatus insert_login_info(const char* groupname,
      const char* password, const char* username, unsigned long priv,
      unsigned long attr);
  static pwr_tStatus get_login_info(char* groupname, char* password,
      char* username, unsigned long* priv, unsigned long* attr);
  static unsigned int privilege()
  {
    return m_priv;
  }
  static void reduce_privilege(unsigned int mask)
  {
    m_priv = (pwr_mPrv)(m_priv & ~mask);
  }
  static char* username()
  {
    return m_username;
  }
  static char* ucpassword()
  {
    return m_ucpassword;
  }
};

#endif
