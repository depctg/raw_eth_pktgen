link_libraries(common)

add_executable(newr-fusion-ref ref.cc)
add_executable(newr-fusion-ref-fuse ref-fused.cc)
add_executable(newr-fusion-best-efforts channel_best.cc)
add_executable(newr-fusion-new new-rt-fused.cc)
add_executable(newr-fusion-opt new-rt-fused-opt.cc)
add_executable(newr-fusion-base no_fuse.cc)
