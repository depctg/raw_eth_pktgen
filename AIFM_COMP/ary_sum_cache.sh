#!/bin/bash

source ../util.sh
# trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345
local_mem=(64 96 128 160 224 288 384 480)

sudo kill -9 `pgrep -f ary_sum_remote` &>/dev/null
for mem in "${local_mem[@]}"
do
  echo "Test local mem size: $mem"

  sed "s/constexpr static uint64_t kCacheSize = .*/constexpr static uint64_t kCacheSize = $mem << 20;/g" ary_sum_local.cpp -i

  cd ..
  make clean_AIFMCOMP
  make AIFMCOMP
  cd AIFM_COMP/
  cache_exp_remoteKVS ary_sum_remote $port &>/dev/null &
  sleep 1
  cache_exp_access ary_sum_local $port | tee log.$mem
  ((port=port+1))
  sudo kill -9 `pgrep -f ary_sum_remote` &>/dev/null
done
echo "Done"
sudo kill -9 `pgrep -f ary_sum_remote` &>/dev/null