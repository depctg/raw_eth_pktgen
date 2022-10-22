#!/bin/bash
export rt_lib=/home/wuklab/projects/pl-zijian/raw_eth_pktgen/from_mlir/libs

function mkd_out() {
  mkdir -p ./out
  rm -f ./out/*
}

function link_mlir_obj() {
  mkd_out
  rm -f $rt_lib/*
  cp /home/wuklab/projects/pl-zijian/raw_eth_pktgen/build/libcommon/lib/CMakeFiles/common.dir/* $rt_lib
  objs=$(find $rt_lib/*.o)
  clang -flto -libverbs -lnng -lm base.o $objs -o out/$1
  echo "Link Complete"
}