#!/bin/bash

source ./util.sh
# trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345
# packet_sizes=(16 32 64 128 256 512)
packet_sizes=(1 2 4 8 16 32 64 128 256 512 1024 2048 4096)

kill -9 `pgrep -f net-lat-est-recver` &>/dev/null
for ps in "${packet_sizes[@]}"
do
  let "psk = $((1024 * ps))"
  sed -i "s/constexpr static uint64_t packet_size = .*/constexpr static uint64_t packet_size = $psk;/g" ../apps/net-lat-est/sender.cpp
  sed -i "s/constexpr static uint64_t packet_size = .*/constexpr static uint64_t packet_size = $psk;/g" ../apps/net-lat-est/receiver.cpp

  make -C ../build net-lat-est-recver
  make -C ../build net-lat-est-sender
  cd ../build/bin

  rdma_recv net-lat-est-recver $port &>/dev/null &
  sleep 1
  rdma_send net-lat-est-sender $port | tee ../../scripts/$psk.txt
  ((port=port+1))
  kill -9 `pgrep -f net-lat-est-recver` &>/dev/null
  cd ../../scripts
done
echo "Done"
kill -9 `pgrep -f net-lat-est-recver` &>/dev/null