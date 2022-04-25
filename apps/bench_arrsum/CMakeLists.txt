link_libraries(common)

add_executable(${app}-send bench_send_arrsum.cpp)
add_executable(${app}-recv bench_recv_arrsum.cpp)

