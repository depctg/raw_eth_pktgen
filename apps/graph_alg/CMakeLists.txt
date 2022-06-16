add_library(graph_util INTERFACE)
target_include_directories(graph_util INTERFACE ./)
link_libraries(common graph_util)

add_executable(${app}-SSSPL SSSP_L.cpp)