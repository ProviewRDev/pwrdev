#!/bin/bash

echo "qcomBusId 999" > /etc/proview.cnf

cat > README <<EOF
# Install pwrdemo
dpkg -i $1

# Start nginx
/etc/init.d/nginx start

# Login as pwrp and start pwrdemo
su - pwrp
sdf pwrdemo'ver'
rt_ini &

# Set display
export DISPLAY=unix:0

# Start xtt
rt_xtt op

# Open a web browser on http://172.17.0.2/pwrdemo/index.html

EOF
