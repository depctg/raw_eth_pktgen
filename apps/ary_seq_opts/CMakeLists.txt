link_libraries(common)


add_executable(${app}-async_fetch async_fetch.cpp)
add_executable(${app}-batch_fetch batch_fetch.cpp)
add_executable(${app}-comp_fetch comp_fetch.cpp)
add_executable(${app}-general_recv general_recv.cpp)
add_executable(${app}-naive_local naive_local.cpp)
add_executable(${app}-pre_fetch pre_fetch.cpp)
add_executable(${app}-simple_send simple_send.cpp)