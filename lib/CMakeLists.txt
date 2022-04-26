add_library(common OBJECT cache.c common.c net.c packet.c)

target_include_directories(common PUBLIC ../include)
target_link_libraries(common PUBLIC ibverbs nng)