add_executable(gen_send gen_send.c)
add_executable(gen_recv gen_recv.c)

target_link_libraries(gen_send PRIVATE $<TARGET_OBJECTS:common>)
target_link_libraries(gen_recv PRIVATE $<TARGET_OBJECTS:common>)

