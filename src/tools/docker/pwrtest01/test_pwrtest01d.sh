#!/bin/bash

echo "Starting test_pwrtest01d.sh"

Xorg -noreset +extension GLX +extension RANDR +extension RENDER -config ./dummy_display_xorg.conf :99 &

# Install mysql, install command in $1
echo "arg: $1"
$1

ps aux
p1=`pidof mariadbd`
p2=`pidof mysqld`
echo "mysql pid p1: $p1 p2: $p2"
if [ "$p1" == "" ] && [ "$p2" == "" ]; then
  mkdir /run/mysqld
  chmod a+w /run/mysqld
  chown mysql:mysql /usr/sbin/mysqld
  chmod u+s /usr/sbin/mysqld
  ls -l /usr/sbin/mysqld
  mysqld --basedir=/usr --datadir=/var/lib/mysql --plugin-dir=/usr/lib/mysql/plugin &
fi

set -o xtrace
# mosquitto, create user and start
echo "pwrp:pwrp" > /etc/mosquitto/passwd
mosquitto_passwd -U /etc/mosquitto/passwd
echo "allow_anonymous false" >> /etc/mosquitto/mosquitto.conf
echo "password_file /etc/mosquitto/passwd" >> /etc/mosquitto/mosquitto.conf
if [ ! -e /run/mosquitto ]; then
  mkdir /run/mosquitto
  chown mosquitto:mosquitto /run/mosquitto
fi
/usr/sbin/mosquitto -c /etc/mosquitto/mosquitto.conf &

sleep 10
mysql -e 'CREATE USER pwrp@localhost;'
mysql -e 'grant all privileges on *.* to pwrp@localhost;'
mysql -e 'ALTER USER pwrp@localhost IDENTIFIED WITH mysql_native_password;'
mysql -e 'ALTER USER pwrp@localhost IDENTIFIED BY "'""'", pwrp@localhost PASSWORD EXPIRE NEVER;'
set +o xtrace
su - pwrp
cd /home/pwrp
source /etc/pwrp_profile
export DISPLAY=:99
export PWR_BUS_ID=999
export PYTHONPATH=$pwr_exe
pwr_pkg.sh -i /home/pwrp/pwrp_pkg_pwrtest01d_0001.tgz

# Rename boot, node and plcfiles
nname=`eval uname -n`

bootfile=`eval ls -1 $pwrp_load/ld_boot*.dat`
nodefile=`eval ls -1 $pwrp_load/ld_node*.dat`
plcfile=`eval ls -1 $pwrp_exe/plc_*_0999_plc`
echo "alias pwrtest01d $nname 127.0.0.1" > $pwrp_load/pwrp_alias.dat

# Start project
rt_ini -i -n pwrtest01d&
sleep 300

mysql -u pwrp -D pwrp__pwrtest01 -e 'select * from O000_254_254_204_00000044__1;'
mysql -u pwrp -D pwrp__pwrtest01 -e 'show tables;'
mysql -u pwrp -D pwrp__pwrtest01 -e 'select * from O000_254_254_204_000000be__3;'
mysql -u pwrp -D pwrp__pwrtest01 -e 'select * from items;'
ps ax
export PYTHONPATH=$pwr_exe

ra_sevtest.py
ra_sev_mqtttest.py
