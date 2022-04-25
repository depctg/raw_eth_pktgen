link_libraries(common)

add_executable(${app}-send gen_send.c)
add_executable(${app}-recv gen_recv.c)

