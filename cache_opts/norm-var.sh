#!/bin/bash

source ../util.sh
# trap 'read -p "run: $BASH_COMMAND"' DEBUG

port=2345

wing=(1 2 4 8 16 32)
sudo kill -9 `pgrep -f remote_test` &>/dev/null
for w in "${wing[@]}"
do
  let "std = $((262144 * w))"
  echo "Test std: $std"

  sed "s/constexpr static uint64_t sigma.*/constexpr static uint64_t sigma = $std;/" cache_test.cpp -i
  cd ..
  make clean_cache_opts
  make caches
  cd cache_opts/
  cache_exp_remoteKVS remote_test $port &>/dev/null &
  sleep 1
  cache_exp_access cache_test $port | tee log.$w
  ((port=port+1))
  sudo kill -9 `pgrep -f remote_test` &>/dev/null
done
echo "Done"
sudo kill -9 `pgrep -f remote_test` &>/dev/null