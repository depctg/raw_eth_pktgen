link_libraries(common)

add_executable(newr-fusion-ref ref.cc)
add_executable(newr-fusion-ref-fuse ref-fused.cc)
add_executable(newr-fusion-best-efforts channel_best.cc)
add_executable(newr-fusion-new channel_best_new.cc)
add_executable(newr-fusion-base no_fuse.cc)
