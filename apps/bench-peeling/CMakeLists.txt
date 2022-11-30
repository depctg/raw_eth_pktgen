link_libraries(common)

add_executable(peeling-ref ref.cpp)
add_executable(peeling-base local_base.cpp)
add_executable(peeling-opt local_opt.cpp)