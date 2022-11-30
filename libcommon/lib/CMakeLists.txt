add_library(common OBJECT
    cache.c
    net.c
    packet.c
    remote_pool.c
    offload.c
    side_channel.c
    $<IF:$<BOOL:${COMMON_IMPL_SHM}>, common_shm.c, common.c>
    )

# internal includes
include_directories(PRIVATE include)

target_include_directories(common PUBLIC ../include)
target_link_libraries(common PUBLIC ibverbs nng m)

if(COMMON_IMPL_SHM)
    set(COMMON_EXE_SHM_EXECUTOR common-shm-executor)
    message("Build common library with shm. binary ${COMMON_EXE_SHM_EXECUTOR} can be built")

    add_executable(${COMMON_EXE_SHM_EXECUTOR} apps/shm_executor.c)
    target_link_libraries(${COMMON_EXE_SHM_EXECUTOR} common)
endif()

add_executable(remote_server apps/remote_server.c apps/fromMLIR/foo.c apps/fromMLIR/assem.c)
target_link_libraries(remote_server common)

# copy objects to /home/wuklab/projects/pl-zijian/raw_eth_pktgen/from_mlir/libs

