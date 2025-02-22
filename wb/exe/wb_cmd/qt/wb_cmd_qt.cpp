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

/* wb_cmd_qt.c -- command file processing
   The main program of pwrc.  */

#include <stdlib.h>
#include <string.h>

#include "co_log.h"

//#include "cow_style_qt.h"

#include "wb_log_qt.h"
#include "wb_wnav_qt.h"

#include "wb_cmdc_qt.h"

#include <QApplication>

CmdQt::CmdQt(int argc, char* argv[])
{
  QWidget* w;
  pwr_tStatus sts;

  // Attach to history log
  new wb_log_qt(0);

  for (int i = 0; i < argc; i++) {
    printf(argv[i]);
    printf(" ");
  }
  printf("\n");

  wnav = new WNavQt((void*)this, NULL, "", "", &w, ldhses, (wnav_sStartMenu*)0,
      wnav_eWindowType_No, &sts);
  wnav->attach_volume_cb = attach_volume_cb;
  wnav->detach_volume_cb = detach_volume_cb;
  wnav->get_wbctx_cb = get_wbctx;
  wnav->save_cb = save_cb;
  wnav->revert_cb = revert_cb;
  wnav->close_cb = close_cb;

  parse(argc, argv);
}

int main(int argc, char* argv[])
{
  log_setLevel(LOG_TRACE);
  QApplication app(argc, argv);
  //  QApplication::setStyle(new PwrStyle());
  setlocale(LC_ALL, "en_US");
  setlocale(LC_NUMERIC, "POSIX");
  setlocale(LC_TIME, "en_US");
  new CmdQt(argc, argv);
  return app.exec();
}
