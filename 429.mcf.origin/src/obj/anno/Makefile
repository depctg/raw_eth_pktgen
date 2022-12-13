# Include standard template for this suite
CC=clang
LIBS = -libverbs -lnng -lm -lpthread
cgeist = /users/Zijian/Disagg-mlir/build/bin/cgeist
clang-b = /users/Zijian/Disagg-mlir/llvm-project/build/bin/clang
poly-opt = /users/Zijian/Disagg-mlir/build/bin/polygeist-opt
mlir-translate = /users/Zijian/Disagg-mlir/llvm-project/build/bin/mlir-translate
CFLAGS += -O3 -flto -g

obj_dir = obj
mlir_dir = lower

rt_obj = cache.c.o common_util.c.o common.c.o net.c.o offload.c.o offloads.c.o packet.c.o remote_pool.c.o side_channel.c.o 
mlir_obj = mcf.o mcfutil.o pstart.o readmin.o psimplex.o pbeampp.o pbla.o pflowup.o treeup.o implicit.o output.o  
_outer_obj = internal_opts.o

outer_obj = $(addprefix outer/, $(_outer_obj))
_obj = $(rt_obj) $(mlir_obj) $(_outer_obj)

OBJ = $(addprefix $(obj_dir)/, $(_obj)) 

$(obj_dir):
	mkdir -p $@

rt: $(obj_dir)
	cp -r /users/Zijian/raw_eth_pktgen/build/libcommon/lib/CMakeFiles/common.dir/*.o $(obj_dir)

outer: $(obj_dir)
	cp -t $(obj_dir) $(outer_obj)

$(obj_dir)/%.o : $(mlir_dir)/%.lower.mlir
	$(poly-opt) --pass-pipeline="cse,canonicalize,convert-polygeist-to-llvm{use-c-style-memref=0}" $< -o $@_llvm.mlir; \
 	$(mlir-translate) --mlir-to-llvmir $@_llvm.mlir -o $@.ll; \
	$(clang-b) $(CFLAGS) $@.ll -c -o $@; \

.PHONY: all
all: $(obj_dir) rt outer mcf

mcf: $(OBJ)
	$(clang-b) -o $@ $^ $(CFLAGS) $(LIBS); \
	cp mcf /users/Zijian/raw_eth_pktgen/build/bin

.PHONY: clean
clean:
	rm -f $(OBJ) $(addsuffix .ll, $(OBJ)) mcf
