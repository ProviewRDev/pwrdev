#!/bin/bash
set -o xtrace

aroot="/usr/pwrp/adm"

# Get version
if [ -e $pwr_inc/pwr_version.h ]; then
  ver=`eval cat $pwr_inc/pwr_version.h | grep "\bpwrv_cWbdbVersionShortStr\b" | awk '{print $3}'`

  if [ -z $ver ]; then
    echo "Unable to get pwr version"
    ver="V00"
  fi
  ver=$(echo "$ver" | tr -d '"' | sed 's/^V//')

  # Extract major.minor version for display (e.g. "V6.1")
  vermajmin=`eval cat $pwr_inc/pwr_version.h | grep "\\bpwrv_cPwrVersionStr\\b" | awk '{print $3}'`
  vermajmin=$(echo "$vermajmin" | tr -d '"' | sed 's/\.[^.]*$//')
fi

datfile=$pwre_sroot/tools/pkg/cygwin/pwrdemo/control

{
  while read line; do
    if [ "${line:0:9}" = "Version: " ]; then
      version=${line#Version: }
    fi
  done
} < $datfile

proot=/data0/pwrp/pwrdemo$ver
pkgroot=$pwre_broot/$pwre_target/bld/pkg/pwrdemo$ver
pkgsrc=$pwre_sroot/tools/pkg/cygwin/pwrdemo
packagename=pwrdemo$ver-$version.tar.gz

echo "-- Building pwrdemo$ver"

# Create directories
mkdir -p $pkgroot/etc/pwrdemo$ver

# Copy pwrdemo
mkdir -p $pkgroot/usr/pwrp/pwrdemo$ver
mkdir -p $pkgroot/usr/pwrp/pwrdemo$ver/cnf
currentdir="`eval pwd`"
tarfile=$pwre_broot/$pwre_target/bld/pkg/pwrtmp.tar
cd $proot
cd ..
echo "-- copy project to package tree"
tar -cf $tarfile pwrdemo$ver
cd $pkgroot/usr/pwrp
tar -xf $tarfile
rm $tarfile
cd $currentdir

cp $pwre_sroot/tools/pkg/cygwin/user/ProviewDemo.lnk $pkgroot/usr/pwrp/pwrdemo$ver/cnf
cp $pwre_sroot/tools/pkg/cygwin/user/proview_icon.ico $pkgroot/usr/pwrp/pwrdemo$ver/cnf

# Generate startscript
{
  echo "#!/bin/bash"
  echo "export PATH=/usr/local/bin:/usr/bin"
  echo "export pwra_db=$aroot/db"
  echo "source \$pwra_db/pwr_setup.sh"
  echo "source \$pwra_db/pwra_env.sh set base $vermajmin"
  echo "source \$pwra_db/pwra_env.sh set bus"
  echo "pwrp set project pwrdemo$ver"
  echo "export DISPLAY=127.0.0.1:0"
  echo "pwr_rtmon"
} > $pkgroot/usr/pwrp/pwrdemo$ver/cnf/pwrdemostart

# Create package
echo "-- Building package"
cd $pkgroot
cp $pkgsrc/control $pkgroot/etc/pwrdemo$ver
cp $pkgsrc/prerm $pkgroot/etc/pwrdemo$ver
echo "#!/bin/bash" > $pkgroot/etc/pwrdemo$ver/postinst
echo "ver=\"$ver\"" >> $pkgroot/etc/pwrdemo$ver/postinst
echo "vermajmin=\"$vermajmin\"" >> $pkgroot/etc/pwrdemo$ver/postinst
tail -n +2 $pkgsrc/postinst >> $pkgroot/etc/pwrdemo$ver/postinst
chmod 755 $pkgroot/etc/pwrdemo$ver/postinst

tar -czf ../$packagename *

rm -r $pkgroot
