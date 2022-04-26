#!/bin/bash

source ../util.sh
# trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345
comp_ts=(1 2 3 4 5 6 7 8 9 10 20 30 40)
# comp_ts=(20 30 40)
batches=(16 32 64 128 256 512 1024 2048 4096)
# batches=(8192 12288 16384)

sudo kill -9 `pgrep -f pre_remote` &>/dev/null
for compt in "${comp_ts[@]}"
do
  for bs in "${batches[@]}"
  do
    echo "Test comp time $compt us, batch size $bs B"
    sed "s/constexpr static uint64_t compute_t = .*/constexpr static uint64_t compute_t = $compt;/g" local.cpp -i
    sed "s/constexpr static uint64_t batch_size = .*/constexpr static uint64_t batch_size = ($bs);/g" local.cpp -i
    sed "s/constexpr uint64_t batch_size = .*/constexpr uint64_t batch_size = $bs;/g" pre_remote.cpp -i

    cd ..
    make clean_compnet
    make comp_net_simu
    cd cpt_net
    cache_exp_remoteKVS pre_remote $port &>/dev/null &
    sleep 1
    cache_exp_access local $port | sudo tee comp_batch0/log.$compt.$bs
    ((port=port+1))
    sudo kill -9 `pgrep -f pre_remote` &>/dev/null
  done
done
echo "Done"
sudo kill -9 `pgrep -f pre_remote` &>/dev/null