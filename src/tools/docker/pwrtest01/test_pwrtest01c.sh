#!/bin/bash

echo "Starting test_pwrtest01c.sh"
echo "Version 1.0"

Xorg -noreset +extension GLX +extension RANDR +extension RENDER -config ./dummy_display_xorg.conf :99 &

# mosquitto, create user and start
echo "pwrp:pwrp" > /etc/mosquitto/passwd
mosquitto_passwd -U /etc/mosquitto/passwd
echo "allow_anonymous false" >> /etc/mosquitto/mosquitto.conf
echo "password_file /etc/mosquitto/passwd" >> /etc/mosquitto/mosquitto.conf
# mosquitto bug
mkdir /run/mosquitto
chmod a+w /run/mosquitto
/usr/sbin/mosquitto -c /etc/mosquitto/mosquitto.conf &

sleep 20
su - pwrp
cd /home/pwrp
source /etc/pwrp_profile
echo "exe: $pwrp_exe"
echo "path: $PATH"
#source .bashrc
export DISPLAY=:99
export PWR_BUS_ID=999
export PYTHONPATH=$pwr_exe
which pwr_pkg.sh
pwr_pkg.sh -i /home/pwrp/pwrp_pkg_pwrtest01c_0001.tgz

# Rename boot, node and plcfiles
nname=`eval uname -n`

bootfile=`eval ls -1 $pwrp_load/ld_boot*.dat`
nodefile=`eval ls -1 $pwrp_load/ld_node*.dat`
plcfile=`eval ls -1 $pwrp_exe/plc_*_0999_plc`
echo "alias pwrtest01c $nname 127.0.0.1" > $pwrp_load/pwrp_alias.dat
#mv $plcfile $pwrp_exe/"plc_"$nname"_0999_plc"

# Start project
rt_ini -i -n pwrtest01c&
sleep 10

export PYTHONPATH=$pwr_exe
ra_testall_c.sh

sleep 3