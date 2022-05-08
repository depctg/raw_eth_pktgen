## Build

Requires CMake >= 3.12, libnng and libibverbs.

```bash
mkdir build && cd build
cmake ..
make <app>
```

Build LLVM Libararies, requires ninja build tool
```bash
mkdir external/llvm-project/build
cd external/llvm-project/build
cmake -G Ninja ../llvm \
          -DLLVM_ENABLE_PROJECTS="mlir;clang" \
          -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
          -DLLVM_TARGETS_TO_BUILD="host" \
          -DLLVM_ENABLE_ASSERTIONS=ON \
          -DCMAKE_BUILD_TYPE=DEBUG
```
