project(clang-passes)

set (LLVM_ROOT "../external/llvm-project" CACHE PATH "LLVM project repo root")
list(INSERT CMAKE_PREFIX_PATH 0 "${LLVM_ROOT}/clang")
list(INSERT CMAKE_PREFIX_PATH 0 "${LLVM_ROOT}/llvm")
set(LLVM_SOURCE_DIR ${LLVM_ROOT}/llvm)

# load local llvm over system one
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}, PATH ${LLVM_SOURCE_DIR}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

find_package(Clang REQUIRED CONFIG)
message(STATUS "Found LLVM ${CLANG_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${CLANG_DIR}")


include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
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
