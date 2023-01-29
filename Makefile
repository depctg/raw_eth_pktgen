all: r_offload

df_offloads:
	clang++ -std=c++2a -O3 -flto -I apps/dataframe/include /users/Zijian/pktgen_node0/raw_eth_pktgen/apps/dataframe/app/offloads.cc -c -o /users/Zijian/pktgen_node0/raw_eth_pktgen/build/libcommon/lib/CMakeFiles/common.dir/df_offloads.o

r_offload: df_offloads
	cd /users/Zijian/pktgen_node0/raw_eth_pktgen/build/libcommon/lib/CMakeFiles/common.dir; \
	clang++ -flto -O3 -std=c++2a /users/Zijian/pktgen_node0/raw_eth_pktgen/build/libcommon/lib/CMakeFiles/remote_server.dir/apps/remote_server.c.o df_offloads.o common.c.o common_util.c.o cache.c.o net.c.o offload.c.o offloads.c.o packet.c.o remote_pool.c.o side_channel.c.o apps/offload/sum.cpp.o apps/fromMLIR/assem.c.o -lpthread -libverbs -lnng -lm /users/Zijian/pktgen_node0/raw_eth_pktgen/build/lib/libDataFrame.a -o /users/Zijian/pktgen_node0/raw_eth_pktgen/rbin/remote_server 

