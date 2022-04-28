link_libraries(common)

add_executable(${app}-sender sender.cpp)
add_executable(${app}-recver receiver.cpp)