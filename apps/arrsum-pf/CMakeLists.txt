link_libraries(common)

add_executable(${app}-local local.cpp)
add_executable(${app}-remote pre_remote.cpp)

