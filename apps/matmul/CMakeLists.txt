link_libraries(common)

add_executable(${app}-send sender.cpp)
add_executable(${app}-recv recver.cpp)