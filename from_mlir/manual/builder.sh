#!/bin/bash
export rt_lib=/home/wuklab/projects/pl-zijian/raw_eth_pktgen/from_mlir/libs

function mkd_out() {
  mkdir -p ./out
  rm -f ./out/*
}

function link_mlir_obj() {
  mkd_out
  rm -f $rt_lib/*
  cp -r /home/wuklab/projects/pl-zijian/raw_eth_pktgen/build/libcommon/lib/CMakeFiles/common.dir/* $rt_lib
  objs=$(find $rt_lib/*.o)
  set -x
  clang-b++ -flto -libverbs -lnng -lm base.o -fprofile-instr-generate $objs -o out/$1
  set +x
  echo "Link Complete"
}
