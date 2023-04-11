link_libraries(common)

add_executable(mm-shared-mt mm-shared-mt.cc)
add_executable(mm-old-mt mm-old-mt.cc)
add_executable(mm-old-mt-test mm-old-mt-test.cc)
# add_executable(mm-mt-gen ref-direct-mt.gen.cc)
# add_executable(prefetch-opt-ring prefetch.cc)