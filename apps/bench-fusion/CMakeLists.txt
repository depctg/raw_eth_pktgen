link_libraries(common)

add_executable(fusion-ref ref.cc)
add_executable(fusion-channel-base channel_base.cc)
add_executable(fusion-best-efforts channel_best.cc)