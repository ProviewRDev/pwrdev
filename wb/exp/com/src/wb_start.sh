#!/bin/bash

# ProviewR   Open Source Process Control.
# Copyright (C) 2005-2020 SSAB EMEA AB.
#
# This file is part of ProviewR.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ProviewR. If not, see <http://www.gnu.org/licenses/>
#
# Linking ProviewR statically or dynamically with other modules is
# making a combined work based on ProviewR. Thus, the terms and
# conditions of the GNU General Public License cover the whole
# combination.
#
# In addition, as a special exception, the copyright holders of
# ProviewR give you permission to, from the build function in the
# ProviewR Configurator, combine ProviewR with modules generated by the
# ProviewR PLC Editor to a PLC program, regardless of the license
# terms of these modules. You may copy and distribute the resulting
# combined work under the terms of your choice, provided that every
# copy of the combined work is accompanied by a complete copy of
# the source code of ProviewR (the version used to produce the
# combined work), being distributed under the terms of the GNU
# General Public License plus this exception.

# Start the navigator

username=$1
password=$2
volume=$3

if [ "`eval locale -a | grep -i "^en_US.UTF-8$"`" != "" ]; then
  export LC_TIME="en_US.UTF-8"
elif [ "`eval locale -a | grep -i "^en_US$"`" != "" ]; then
  export LC_TIME="en_US"
fi
export LC_NUMERIC="POSIX"

wb $username $password $volume $4 $5 $6 $7