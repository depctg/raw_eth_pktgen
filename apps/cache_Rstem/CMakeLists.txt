link_libraries(common)

add_executable(${app}-cache_test cache_test.cpp)
add_executable(${app}-local_multi_cache local_multi_cache.cpp)
add_executable(${app}-local_multi_cache_nopref local_multi_cache_nopref.cpp)
add_executable(${app}-remote_test remote_test.cpp)
add_executable(${app}-remote_pool_test remote_pool_test.cpp)