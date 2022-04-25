#!/bin/bash

# trap 'read -p "run: $BASH_COMMAND"' DEBUG
source ../util.sh
port=2345
nruns=5

# sudo rm log.*
# batch size VS latency
array_size=$(expr 1024 \* 1024 \* 4)
# pre_stride=(1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16)
pre_stride=(9 10 11 12 13 14 15 16)
nsleep=10000
sudo kill -9 `pgrep -f bench_recv_arrs` &>/dev/null
for s in "${pre_stride[@]}"
do 
    echo "Test prefetch stride: $s ..."
    setup_remote bench_recv_arrsum $port $nsleep $array_size &>/dev/null &
    run_program bench_send_arrsum $port 5 $array_size $nruns 256 $s | tee log.$s 2>&1
    sleep 1
    ((port=port+1))
done

echo "No prefetch"
setup_remote bench_recv_arrsum $port $nsleep $array_size &>/dev/null &
run_program bench_send_arrsum $port 4 $array_size $nruns 256 0 | tee log.0 2>&1
sleep 1
((port=port+1))
echo "Fetch at once ..."
setup_remote bench_recv_arrsum $port $usleep $array_size &>/dev/null &
run_program bench_send_arrsum $port 3 $array_size $nruns $s 0 | tee log.comp

sudo kill -9 `pgrep -f bench_recv_arrs` &>/dev/null
