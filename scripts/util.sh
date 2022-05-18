#!/bin/bash

setup_remote () {
    sudo ./$1 -addr tcp://*:$2 -job 1 -nsleep $3 -size_array $4
}

run_program () {
    sudo stdbuf -o0 ./$1 -addr tcp://localhost:$2 -job $3 -array_size $4 -n_runs $5 -size_batch $6 -pre_stride $7
}

rdma_recv () {
    sudo ./$1 tcp://*:$2
}

rdma_send () {
    sudo stdbuf -o0 ./$1 tcp://localhost:$2
}

shm_recv() {
    numactl -N 0 -m 0 ./$1 $2
}

shm_send() {
    stdbuf -o0 numactl -N 0 -m 0 ./$1 $2
}

shm_exec() {
    numactl -N 0 -m 0 ./common-shm-executor $1
}