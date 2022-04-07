#!/bin/bash

setup_remote () {
    sudo ./$1 -addr tcp://*:$2 -job 1 -usleep $3 -size_array $4
}

run_program () {
    sudo ./$1 -addr tcp://localhost:$2 -job $3 -array_size $4 -n_runs $5 -size_batch $6 -pre_stride $7
}