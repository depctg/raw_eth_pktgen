link_libraries(common)

add_executable(conv-ref ref.c)
add_executable(conv-ref-ring ref-ring.c)
# add_executable(prefetch-opt-ring prefetch.cc)