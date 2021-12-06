#!/bin/bash

echo "Starting test_pwrtest01f.sh"
echo "Version 1.0"

Xorg -noreset +extension GLX +extension RANDR +extension RENDER -config ./dummy_display_xorg.conf :99 &

usermod -a -G tty pwrp

# Create serial ports with socat
ofile="/tmp/pwrtest01pts.txt"
echo "" > $ofile

# Serial ports for remote serial
socat -d -d pty,link=/tmp/socat1,raw,echo=0 pty,link=/tmp/socat2,raw,echo=0 2>> $ofile&

# Serial ports for Modbus RTU
socat -d -d pty,link=/tmp/socat3,raw,echo=0 pty,link=/tmp/socat4,raw,echo=0 2>> $ofile &

sleep 2
devs=`grep /dev/pts $ofile | cut -d " " -f7`
for dev in $devs
do
  echo "Serial device $dev"
  chmod a+rw $dev
done

# mosquitto, create user and start
echo "pwrp:pwrp" > /etc/mosquitto/passwd
mosquitto_passwd -U /etc/mosquitto/passwd
echo "allow_anonymous false" >> /etc/mosquitto/mosquitto.conf
echo "password_file /etc/mosquitto/passwd" >> /etc/mosquitto/mosquitto.conf
# mosquitto bug
mkdir /run/mosquitto
chmod a+w /run/mosquitto
/usr/sbin/mosquitto -c /etc/mosquitto/mosquitto.conf &

su - pwrp
cd /home/pwrp

sleep 5

sleep 20
source /etc/pwrp_profile
echo "exe: $pwrp_exe"
echo "path: $PATH"
#source .bashrc
export DISPLAY=:99
export PWR_BUS_ID=999
export PYTHONPATH=$pwr_exe
which pwr_pkg.sh
pwr_pkg.sh -i /home/pwrp/pwrp_pkg_pwrtest01f_0001.tgz

# Rename boot, node and plcfiles
nname=`eval uname -n`

bootfile=`eval ls -1 $pwrp_load/ld_boot*.dat`
nodefile=`eval ls -1 $pwrp_load/ld_node*.dat`
plcfile=`eval ls -1 $pwrp_exe/plc_*_0999_plc`
echo "alias pwrtest01f $nname 127.0.0.1" > $pwrp_load/pwrp_alias.dat

# Disable ModbusRTU as device can't be opened
#echo "$nname""_setval Nodes-PwrTest01f-IO-MbRTU.Disable = 1" >> $pwrp_load/pwrp_alias.dat
#echo "$nname""_setval Nodes-PwrTest01f-IO-MbRTU_Server.DisableServer = 1" >> $pwrp_load/pwrp_alias.dat

cat $pwrp_load/pwrp_alias.dat

#mv $plcfile $pwrp_exe/"plc_"$nname"_0999_plc"

echo "pwrtest01f 0.254.254.206 127.0.0.1 0 0 0 0 600000 0.002000 0 0 - -" > $pwrp_load/ld_node_pwrtest01f_0999.dat

# Start project
rt_ini -i -n pwrtest01f&
sleep 10
cat > t.rtt_com <<EOF
main()
  string aname;
  string value;
  int sts;

  sts = GetAttribute("Nodes-PwrTest01f-IO-MbRTU.Disable");
  printf("MbRTU.Disable %d\n", sts);
  sts = GetAttribute("Nodes-PwrTest01f-IO-MbRTU_Server.DisableServer");
  printf("MbRTU_Server.DisableServer %d\n", sts);

  value = GetAttribute("Nodes-PwrTest01f-IO-MbRTU.Device");
  printf("MbRTU.Device %s\n", value);
  sts = GetAttribute("Nodes-PwrTest01f-IO-MbRTU-S1.Status");
  printf("MbRTU-S1.Status %d\n", sts);

  value = GetAttribute("Nodes-PwrTest01f-IO-MbRTU_Server.Device");
  printf("MbRTU_Server.Device %s\n", value);
  sts = GetAttribute("Nodes-PwrTest01f-IO-MbRTU_Server.Status");
  printf("MbRTU_Server.Status %d\n", sts);

  sts = GetAttribute("Nodes-PwrTest01f-IO-MQTT.Status");
  printf("MQTT.Status %d\n", sts);
endmain

EOF
rt_xtt_cmd @t

sleep 150

cat > t.rtt_com <<EOF
main()
  string aname;
  string value;
  int sts;

  sts = GetAttribute("Test01f-IO-MQTT-TestActive.ActualValue");
  printf("MQTT TestActive %d\n", sts);
  sts = GetAttribute("Test01f-IO-MQTT-ErrorDetected.ActualValue");
  printf("MQTT ErrorDetected %d\n", sts);
  sts = GetAttribute("Test01f-IO-MQTT-TestExecuted.ActualValue");
  printf("MQTT TestExecuted %d\n", sts);
  sts = GetAttribute("Test01f-IO-MQTT-ErrorState.ActualValue");
  printf("MQTT ErrorState %d\n", sts);
endmain

EOF
rt_xtt_cmd @t

export PYTHONPATH=$pwr_exe
ra_testall_f.sh
cat $pwrp_log/pwr*.log
ls -l /tmp/socat*
ls -l /dev/pts/*

sleep 3
