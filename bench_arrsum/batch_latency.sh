#!/bin/bash

# trap 'read -p "run: $BASH_COMMAND"' DEBUG
source ../util.sh
port=2345
nruns=10

sudo rm log.*
# batch size VS latency
array_size=$(expr 1024 \* 2)
batch_size=(4 8 16 32 64 128 256 512 1024)
usleep=0
sudo kill -9 `pgrep -f bench_recv_arrs` &>/dev/null
for bs in "${batch_size[@]}"
do 
    echo "Test batch size: $bs ..."
    setup_remote bench_recv_arrsum $port $usleep $array_size &>/dev/null &
    run_program bench_send_arrsum $port 4 $array_size $nruns $bs 0 1>log.$bs 
    sleep 0.5
    ((port=port+1))
done
echo "Fetch at once ..."
setup_remote bench_recv_arrsum $port $usleep $array_size &>/dev/null &
run_program bench_send_arrsum $port 3 $array_size $nruns $bs 0 1>log.comp

sudo kill -9 `pgrep -f bench_recv_arrs` &>/dev/null
