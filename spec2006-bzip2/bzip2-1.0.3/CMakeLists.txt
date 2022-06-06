add_library(bzip2 
  blocksort.c
  huffman.c
  crctable.c
  randtable.c
  compress.c
  decompress.c
  bzlib.c
)

target_include_directories(bzip2 PUBLIC ./)
