# Include standard template for this suite
CC=clang
LIBS = -lprofiler -libverbs -lnng -lm -lpthread
cgeist = /users/Zijian/Disagg-mlir/build/bin/cgeist
clang-b = /users/Zijian/Disagg-mlir/llvm-project/build/bin/clang
poly-opt = /users/Zijian/Disagg-mlir/build/bin/polygeist-opt
mlir-translate = /users/Zijian/Disagg-mlir/llvm-project/build/bin/mlir-translate
USE_MLIR ?= 0
CFLAGS += -O3 -flto -g

obj_dir = obj

rt_obj = cache.c.o common_util.c.o common.c.o net.c.o offload.c.o offloads.c.o packet.c.o remote_pool.c.o side_channel.c.o 
_obj = implicit.c.o mcf.c.o mcfutil.c.o output.c.o pbeampp.c.o pbla.c.o pflowup.c.o psimplex.c.o pstart.c.o readmin.c.o treeup.c.o $(rt_obj)

moveMCF: $(obj_dir)
	cp -r /mnt/data/raw_eth_pktgen/build/apps/mcf/CMakeFiles/mcf-main.dir/*.o $(obj_dir)

moveRT: $(obj_dir)
	cp -r /mnt/data/raw_eth_pktgen/build/libcommon/lib/CMakeFiles/common.dir/*.o $(obj_dir)

OBJ = $(addprefix $(obj_dir)/, $(_obj))

$(obj_dir):
	mkdir -p $@

$(obj_dir)/%.o : %.c *.h
	if [ $(USE_MLIR) -ne 0 ]; then \
		echo "use mlir"; \
		$(cgeist) -S $(CFLAGS) -function=* -I. $< -o $@.mlir; \
		$(poly-opt) -convert-polygeist-to-llvm $@.mlir -o $@_llvm.mlir; \
		$(mlir-translate) --mlir-to-llvmir $@_llvm.mlir -o $@.ll; \
		$(CC) $(CFLAGS) $@.ll -c -o $@; \
	else \
		echo "not use mlir"; \
		$(CC) $(CFLAGS) $< -c -o $@; \
	fi

.PHONY: all
all: $(obj_dir) moveMCF moveRT mcf

mcf: $(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJ) $(addsuffix .ll, $(OBJ)) $(addsuffix _llvm.mlir, $(OBJ)) $(addsuffix .mlir, $(OBJ)) mcf
