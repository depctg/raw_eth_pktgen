project(clang-passes)

set (LLVM_ROOT "../external/llvm-project/build/lib" CACHE PATH "LLVM project repo root")

# load local llvm over system one
list(APPEND CMAKE_MODULE_PATH "${LLVM_ROOT}/cmake/llvm")
list(APPEND CMAKE_MODULE_PATH "${LLVM_ROOT}/cmake/clang")
find_package(Clang REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}, PATH ${LLVM_SOURCE_DIR}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include(AddLLVM)
include(AddClang)

include_directories(SYSTEM "${LLVM_INCLUDE_DIRS};${CLANG_INCLUDE_DIRS}")
add_definitions(${LLVM_DEFINITIONS_LIST})

# Detailed executable 
set(LLVM_LINK_COMPONENTS support)

add_clang_executable(loop-convert
    LoopConvert.cpp
    )
target_link_libraries(loop-convert
    PRIVATE
    clangAST
    clangASTMatchers
    clangBasic
    clangFrontend
    clangSerialization
    clangTooling
    )
