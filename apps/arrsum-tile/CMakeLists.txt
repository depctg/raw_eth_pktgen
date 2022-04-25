link_libraries(common)

add_executable(${app}-cache_remote cache_remote.cpp)
add_executable(${app}-keepC_local keepC_local.cpp)
add_executable(${app}-local tile_local.cpp)

