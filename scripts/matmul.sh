#!/bin/bash

source ./util.sh
# trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345
max_sizes=()

kill -9 `pgrep -f cpt_net-pre_remote` &>/dev/null
for bs in "${batches[@]}"
do
  let "bsk = $((bs))"
  sed -i "s/constexpr static uint64_t batch_size = .*/constexpr static uint64_t batch_size = $bsk;/g" ../apps/cpt_net/local.cpp
  for ia in "${iter_aheads[@]}"
  do
    echo "Test batch size $bsk, iter ahead $ia"
    sed -i "s/constexpr static int iter_ahead = .*/constexpr static int iter_ahead = $ia;/g" ../apps/cpt_net/local.cpp
    sed -i "s/constexpr static uint64_t batch_size = .*/constexpr static uint64_t batch_size = $bsk;/g" ../apps/cpt_net/pre_remote.cpp

    make -C ../build cpt_net-pre_remote
    make -C ../build cpt_net-local
    cd ../build/bin

    rdma_recv cpt_net-pre_remote $port &>/dev/null &
    sleep 1
    rdma_send cpt_net-local $port | tee ../../scripts/$bsk.$ia.txt
    ((port=port+1))
    kill -9 `pgrep -f cpt_net-pre_remote` &>/dev/null
    cd ../../scripts
  done
done
echo "Done"
sudo kill -9 `pgrep -f cpt_net-pre_remote` &>/dev/null