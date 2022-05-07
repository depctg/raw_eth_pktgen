link_libraries(common)

add_executable(${app}-send gen_send.c)
add_executable(${app}-recv gen_recv.c)
add_executable(${app}-simple-send gen_simple_send.c)
add_executable(${app}-simple-recv gen_simple_recv.c)
add_executable(${app}-latency-send gen_latency_send.c)
add_executable(${app}-latency-recv gen_latency_recv.c)

