link_libraries(common)

add_executable(peeling-ref ref.cpp)
add_executable(peeling-cache-base local_cache_base.cpp)
add_executable(peeling-channel-base local_channel_base.cpp)
add_executable(peeling-peel-opt local_peel_opt.cpp)