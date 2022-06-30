add_library(graph_util INTERFACE)
target_include_directories(graph_util INTERFACE ./)
link_libraries(common graph_util)

add_executable(${app}-SSSP SSSP.c)
add_executable(${app}-SSSP-Local SSSP_local.cpp)
add_executable(${app}-SSSP-Remote SSSP_remote.cpp)