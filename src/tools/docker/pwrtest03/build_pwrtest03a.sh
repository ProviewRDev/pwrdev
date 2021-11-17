#!/bin/bash

source /etc/pwrp_profile
pwr_pkg.sh -i /home/pwrp/pwrp_pkg_pwrtest03a_0001.tgz
echo "qcomBusId 999" > /etc/proview.cnf

cat > README <<EOF

# Start nginx
/etc/init.d/nginx start

# Login as pwrp and start pwrtest03a
su - pwrp
rt_ini -n pwrtest03a &

# Set display
export DISPLAY=unix:0

# Start xtt
rt_xtt op

# Open a web browser on http://172.17.0.2/pwrp_web/index.html

EOF