make -C src clean
make -C src -j all
./src/mcf data/train/input/inp.in 2>debug.txt

make -C src clean
make -C src -j all USE_MLIR=1
./src/mcf data/train/input/inp.in 2>debug_poly.txt

cmp debug.txt debug_poly.txt