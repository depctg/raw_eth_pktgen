#!/bin/bash

# trap 'read -p "run: $BASH_COMMAND"' DEBUG
source ./util.sh 
shmid=4
cls=(16 32 64 256 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152)

kill -9 `pgrep -f cache_Rstem-remote_test` &>/dev/null
kill -9 `pgrep -f common-shm-executor` &>/dev/null
for cl in "${cls[@]}"
do
  echo "Test cache line size: $cl"

  sed -i "s/constexpr static uint32_t cache_line_size .*/constexpr static uint32_t cache_line_size = $cl;/" ../apps/cache_Rstem/remote_test.cpp

  sed -i "s/constexpr static uint32_t cache_line_size .*/constexpr static uint32_t cache_line_size = $cl;/" ../apps/cache_Rstem/cache_test.cpp
  make -C ../build cache_Rstem-cache_test
  make -C ../build cache_Rstem-remote_test
  cd ../build/bin
  shm_recv cache_Rstem-remote_test $shmid &>/dev/null &
  sleep 1
  shm_send cache_Rstem-cache_test $shmid | tee $cl.perf &
  sleep 1
  shm_exec $shmid &
  while pgrep -f cache_Rstem-cache_test > /dev/null; do sleep 1; done
  kill -9 `pgrep -f cache_Rstem-remote_test` &>/dev/null
  kill -9 `pgrep -f common-shm-executor` &>/dev/null
  cd ../../scripts
done
echo "Done"
kill -9 `pgrep -f cache_Rstem-remote_test` &>/dev/null
kill -9 `pgrep -f common-shm-executor` &>/dev/null
