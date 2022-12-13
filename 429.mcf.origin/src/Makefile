# Include standard template for this suite
CC=clang
# LIBS = -ltcmalloc 
cgeist = /users/Zijian/Disagg-mlir/build/bin/cgeist
clang-b = /users/Zijian/Disagg-mlir/llvm-project/build/bin/clang
poly-opt = /users/Zijian/Disagg-mlir/build/bin/polygeist-opt
mlir-translate = /users/Zijian/Disagg-mlir/llvm-project/build/bin/mlir-translate
USE_MLIR ?= 0
CFLAGS += -O3 -std=c11 

obj_dir = obj
_obj = implicit.o mcf.o mcfutil.o output.o pbeampp.o pbla.o pflowup.o psimplex.o pstart.o readmin.o treeup.o 
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
all: $(obj_dir) mcf

mcf: $(OBJ) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm -f $(OBJ) $(addsuffix .ll, $(OBJ)) $(addsuffix _llvm.mlir, $(OBJ)) $(addsuffix .mlir, $(OBJ)) mcf
