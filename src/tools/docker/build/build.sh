#!/bin/bash
BUILDVERSION=$1" "$2
JDK_DIR=$3
Xorg -noreset +extension GLX +extension RANDR +extension RENDER -config pwr/src/tools/dummy_display/xorg.conf :99 &
export DISPLAY=:99
export jdk=$JDK_DIR

# Running docker armhf in aarch64, machine is still aarch64. Change to armhf.
if [ "$ARCH" == "armhf" ]; then
  mv /usr/bin/uname /usr/bin/uname.orig
  cat > /usr/bin/uname << EOF
if [ "\$1" == "-m" ]; then
  echo "armhf"
else
  uname.orig \$1
fi
EOF
  chmod a+x /usr/bin/uname
fi

touch /etc/proview.cnf
cd pwr
./build.sh --version $BUILDVERSION
cd ..
./build_pwrdemo.sh
./build_pwrtest01.sh
