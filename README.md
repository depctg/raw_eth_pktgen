## Build

Requires CMake >= 3.12, libnng and libibverbs.

```bash
mkdir build && cd build
cmake ..
make <app>
```

### Build clang-passes

requires ninja build tool. (apt install ninja-build)
first, build clang libraries

```bash
mkdir external/llvm-project/build
cd external/llvm-project/build
cmake -G Ninja ../llvm \
          -DLLVM_ENABLE_PROJECTS="mlir;clang" \
          -DLLVM_ENABLE_RUNTIMES="libcxx;libcxxabi;libunwind" \
          -DLLVM_ENABLE_ASSERTIONS=ON \
          -DCMAKE_BUILD_TYPE=DEBUG
```
          -DLLVM_TARGETS_TO_BUILD="X86;NVPTX;AMDGPU" \

build clang-passes
```bash
mkdir build && cd build
cmake ..
```
