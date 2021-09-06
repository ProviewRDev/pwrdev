#!/bin/bash

echo "Starting test_pwrtest01e.sh"

Xorg -noreset +extension GLX +extension RANDR +extension RENDER -config ./dummy_display_xorg.conf :99 &

sleep 10
su - pwrp
cd /home/pwrp
source /etc/pwrp_profile
export DISPLAY=:99
export PWR_BUS_ID=999
export PYTHONPATH=$pwr_exe
pwr_pkg.sh -i /home/pwrp/pwrp_pkg_pwrtest01e_0001.tgz

# Rename boot, node and plcfiles
nname=`eval uname -n`

bootfile=`eval ls -1 $pwrp_load/ld_boot*.dat`
nodefile=`eval ls -1 $pwrp_load/ld_node*.dat`
plcfile=`eval ls -1 $pwrp_exe/plc_*_0999_plc`
echo "alias pwrtest01e $nname 127.0.0.1" > $pwrp_load/pwrp_alias.dat

# Start project
rt_ini -i -n pwrtest01e&
sleep 300

export PYTHONPATH=$pwr_exe

ra_sevtest_e.py sqlite
. pwr_stop.sh

echo $nname"_setval Nodes-PwrTest01e-Servers-SevServer.Database = 2" >> $pwrp_load/pwrp_alias.dat

cat $pwrp_load/pwrp_alias.dat

# Start project
rt_ini -i -n pwrtest01e&
sleep 300

export PYTHONPATH=$pwr_exe

ra_sevtest_e.py hdf5

ls $pwrp_db -ltr
