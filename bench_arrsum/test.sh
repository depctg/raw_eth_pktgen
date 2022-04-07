#!/bin/bash
source ../util.sh
trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345
nruns=10
array_size=$(expr 1024 \* 4)
batch_size=4
usleep=0
setup_remote bench_recv_arrsum $port $usleep $array_size &>/dev/null &
run_program bench_send_arrsum $port 4 $array_size $nruns $batch_size 0 | grep avg >log.$batch_size
sudo kill -9 `pgrep -f bench_recv_arrs`