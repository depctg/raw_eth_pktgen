#!/bin/bash

source ./util.sh
# trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345
# comp_ts=(1 2 3 4 5 6 7 8 9 10 20 30 40)
batches=(16 32 64 128 256 512)
iter_aheads=(0)
# batches=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192)
# iter_aheads=(0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30)
# iter_aheads=(31 32 33 34 35 36 37 38 39 40 41 42 43 44 45)

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