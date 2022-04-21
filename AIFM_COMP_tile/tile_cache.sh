#!/bin/bash

source ../util.sh
# trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345
local_rams=(64 96 128 160 192 224 256)
sks=(0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1 1.1 1.2 1.3)

sudo kill -9 `pgrep -f ary_sum_remote` &>/dev/null
for mem in "${local_mem[@]}"
do
  echo "Test local mem size: $mem"

  sed "s/constexpr static uint64_t kCacheSize = .*/constexpr static uint64_t kCacheSize = $mem << 20;/g" ary_sum_local.cpp -i

  cd ..
  make clean_AIFMCOMP
  make AIFMCOMP_seq
  cd AIFM_COMP_seq/
  cache_exp_remoteKVS ary_sum_remote $port &>/dev/null &
  sleep 1
  cache_exp_access ary_sum_local $port | tee log.$mem
  ((port=port+1))
  sudo kill -9 `pgrep -f ary_sum_remote` &>/dev/null
done
echo "Done"
sudo kill -9 `pgrep -f ary_sum_remote` &>/dev/null