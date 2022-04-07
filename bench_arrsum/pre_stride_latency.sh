#!/bin/bash

# trap 'read -p "run: $BASH_COMMAND"' DEBUG
source ../util.sh
port=2345
nruns=10

sudo rm log.*
# batch size VS latency
array_size=$(expr 1024 \* 2)
pre_stride=(2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 32 64 128 256)
usleep=0
sudo kill -9 `pgrep -f bench_recv_arrs` &>/dev/null
for s in "${pre_stride[@]}"
do 
    echo "Test prefetch stride: $s ..."
    setup_remote bench_recv_arrsum $port $usleep $array_size &>/dev/null &
    run_program bench_send_arrsum $port 5 $array_size $nruns 8 $s 1>log.$s 2>&1
    sleep 1
    ((port=port+1))
done
echo "Fetch at once ..."
setup_remote bench_recv_arrsum $port $usleep $array_size &>/dev/null &
run_program bench_send_arrsum $port 3 $array_size $nruns $s 0 1>log.comp

sudo kill -9 `pgrep -f bench_recv_arrs` &>/dev/null
