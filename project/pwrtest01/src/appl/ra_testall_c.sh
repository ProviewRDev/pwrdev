#!/bin/bash

exec()
{
    echo "Executing $1"
    $1
}

dexec()
{
    echo "Executing $1"
    $1 &
    pid[$i]=$!
    i=$i+1
}

let i=0

exec "ra_plctest"
exec "ra_timetest"
exec "ra_nettimetest"
exec "ra_cdhtest"
exec "ra_gdhtest"
exec "ra_errhtest"
exec "ra_aproctest"
exec "ra_mhtest"
exec "ra_mhappltest"
exec "ra_qcomtest"
exec "ra_pwrrttest.py"
exec "ra_mqtttest.py"
exec "rt_xtt_cmd @$pwrp_exe/ra_ccmtest"
exec "rt_xtt_cmd @$pwrp_exe/ra_xttscripttest"
dexec "ra_qmontest"
exec "ra_qmontest 2"

# Wait for spawned processes
for p in ${pid[*]}; do
  wait $p
done
