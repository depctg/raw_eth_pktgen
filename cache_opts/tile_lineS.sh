#!/bin/bash

source ../util.sh
# trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345
tile=(1 2 4 8 16 32 64 128 256 1024 2048)

sudo kill -9 `pgrep -f remote_test` &>/dev/null
for t in "${tile[@]}"
do
  echo "Test tile: $t"

  sed "s/constexpr static uint64_t tile.*/constexpr static uint64_t tile = $t;/" cache_test.cpp -i
  cd ..
  make clean_cache_opts
  make caches
  cd cache_opts/
  cache_exp_remoteKVS remote_test $port &>/dev/null &
  sleep 1
  cache_exp_access cache_test $port | tee log.$t
  ((port=port+1))
  sudo kill -9 `pgrep -f remote_test` &>/dev/null
done
echo "Done"
sudo kill -9 `pgrep -f remote_test` &>/dev/null