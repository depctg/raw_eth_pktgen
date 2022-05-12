link_libraries(common)

add_executable(${app}-local local.cpp)
add_executable(${app}-pre_remote pre_remote.cpp)
add_executable(${app}-pure-local pure_local.cpp)