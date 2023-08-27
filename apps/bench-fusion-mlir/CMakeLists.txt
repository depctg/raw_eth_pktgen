link_libraries(common)

add_executable(micro-fusion-ref step3.cc internal_ref.cc)
add_executable(micro-fusion-remote step3_remote.cc internal_remote.cc)
