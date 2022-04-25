#!/bin/bash

source ../util.sh
# trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345
int_per_line=(1 2 4 8 16 32 64 128 256 1024 2048)
# int_per_line=64

sudo kill -9 `pgrep -f remote_test` &>/dev/null
for ipl in "${int_per_line[@]}"
do
  let "cache_line_size = $((ipl * 8))"
  echo "Test cache line size: $cache_line_size"

  sed "s/static uint32_t cache_line_size.*/static uint32_t cache_line_size = $cache_line_size;/" remote_test.cpp -i

  sed "s/constexpr static uint32_t cache_line_size.*/constexpr static uint32_t cache_line_size = $cache_line_size;/" cache_test.cpp -i
  cd ..
  make clean_cache_opts
  make caches
  cd cache_Rstem/
  cache_exp_remoteKVS remote_test $port &>/dev/null &
  sleep 1
  cache_exp_access cache_test $port | tee log.$cache_line_size
  ((port=port+1))
  sudo kill -9 `pgrep -f remote_test` &>/dev/null
done
echo "Done"
sudo kill -9 `pgrep -f remote_test` &>/dev/null