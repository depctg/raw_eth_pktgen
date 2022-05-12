link_libraries(common)

add_executable(${app}-sender sender.cpp)
add_executable(${app}-recver receiver.cpp)
add_executable(${app}-realp real_packet.cpp)
add_executable(${app}-pahead pahead.cpp)