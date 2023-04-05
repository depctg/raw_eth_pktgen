link_libraries(common)

add_executable(mm-ref ref.cc)
add_executable(mm-direct ref-direct.cc)
add_executable(mm-direct-inner ref-direct-inner.cc)
add_executable(mm-mt ref-direct-mt.cc)
add_executable(mm-mt-gen ref-direct-mt.gen.cc)
# add_executable(prefetch-opt-ring prefetch.cc)