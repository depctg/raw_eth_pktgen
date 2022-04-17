#!/bin/bash

# trap 'read -p "run: $BASH_COMMAND"' DEBUG
source ../util.sh
port=2345
nruns=5

# sudo rm log.*
# batch size VS latency
array_size=$(expr 1024 \* 1024 \* 4)
batch_size=(16 32 64 128 256 512 1024 2048 4096 8192)
# batch_size=(1024 2048 4096 8192)
nsleep=10000
sudo kill -9 `pgrep -f bench_recv_arrsum` &>/dev/null
for bs in "${batch_size[@]}"
do 
    echo "Test batch size: $bs ..."
    setup_remote bench_recv_arrsum $port $nsleep $array_size &>/dev/null &
    sleep 0.5
    run_program bench_send_arrsum $port 4 $array_size $nruns $bs 0 | tee log.$bs 
    ((port=port+1))
done
echo "Fetch at once ..."
setup_remote bench_recv_arrsum $port $nsleep $array_size &>/dev/null &
run_program bench_send_arrsum $port 3 $array_size $nruns 0 0 | tee log.comp

sudo kill -9 `pgrep -f bench_recv_arrsum` &>/dev/null
