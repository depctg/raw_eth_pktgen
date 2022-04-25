#!/bin/bash

setup_remote () {
    sudo ./$1 -addr tcp://*:$2 -job 1 -nsleep $3 -size_array $4
}

run_program () {
    sudo stdbuf -o0 ./$1 -addr tcp://localhost:$2 -job $3 -array_size $4 -n_runs $5 -size_batch $6 -pre_stride $7
}

cache_exp_remoteKVS () {
    sudo ./$1 tcp://*:$2
}

cache_exp_access () {
    sudo stdbuf -o0 ./$1 tcp://localhost:$2
}