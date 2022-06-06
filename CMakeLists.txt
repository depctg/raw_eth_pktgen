cmake_minimum_required(VERSION 3.14)
project(raw-pkt-gen)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# options
option(COMMON_IMPL_SHM, "use local shared memory implementation")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# compiler flags 
add_compile_options(-Wall)
add_compile_definitions(
    _POSIX_C_SOURCE=199309L
    _GNU_SOURCE
    )

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wextra -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_DEBUG "-g -Wextra -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

# targets
add_subdirectory(libcommon)
#add_subdirectory(clang-passes)
add_subdirectory(apps)
add_subdirectory(spec2006-bzip2)
