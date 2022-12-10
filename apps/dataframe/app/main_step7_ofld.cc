#include <vector>
#include <unordered_set>
#include <chrono>
#include "internal.h"
#include "rvector.h"
#include <cassert>
#include "cache.h"
#include "common.h"
#include "offload.h"


template <typename T_Key>
void analyze_trip_durations_of_timestamps(const char* key_col_name)
{
    printf("analyze_trip_durations_of_timestamps() on key = %s\n", key_col_name);

    auto &copy_index        = get_index();
    auto &copy_key_col      = get_column<short>(key_col_name);
    auto &copy_key_duration = get_column<uint64_t>("duration");

    size_t N = copy_key_col.size();

    rvector<size_t> *indice_rv = (rvector<size_t> *) &copy_index;
    rvector<short> *key_col_rv = (rvector<short> *) &copy_key_col;
    rvector<uint64_t> *duration_rv = (rvector<uint64_t> *) &copy_key_duration;
    int ikey = (strcmp(key_col_name, "pickup_month") == 0);

    printf("%p %p %p\n", indice_rv->head, key_col_rv->head, duration_rv->head);

    size_t arg_size = 0;
    rvector<size_t> *gep_idx = (rvector<size_t> *) offload_arg_buf;
    *gep_idx = *indice_rv;
    arg_size += sizeof(rvector<size_t>);

    rvector<short> *gep_key_col = (rvector<short> *) (gep_idx + 1);
    *gep_key_col = *key_col_rv;
    arg_size += sizeof(rvector<short>);

    int *gep_ikey = (int *) (gep_key_col + 1);
    *gep_ikey = ikey;
    arg_size += sizeof(int);

    rvector<uint64_t> *gep_duration = (rvector<uint64_t> *) (gep_ikey + 1);
    *gep_duration = *duration_rv;
    arg_size += sizeof(rvector<uint64_t>);

    void *data = call_offloaded_service(9, arg_size, sizeof(rvector<short>) + sizeof(rvector<uint64_t>));

    rvector<short> *key_after = (rvector<short> *) data;
    rvector<uint64_t> *duration_after = (rvector<uint64_t> *) (key_after + 1);
    printf("%lu %lu\n", (uint64_t)key_after->head,(uint64_t) duration_after->head);

    // for (uint64_t i = 0; i < key_vec.size(); i++) {
    //     std::cout << static_cast<int>(key_vec[i]) << " " << duration_vec[i] << std::endl;
    // }
    size_t s = key_after->end - key_after->head;
    printf("mapped size %lu\n", s);
    printf("sbuf offset %x\n", remoteAddr(key_after->head));
    rring_init(rkey, short, (2 << 20), 4, (size_t) ((char*)rbuf + (8<<20)), remoteAddr(key_after->head));
    rring_init(rduration, uint64_t, (2 << 20), 4, (size_t) ((char*)rbuf + (72<<20)), remoteAddr(duration_after->head));

    rring_outer_loop_with(rkey, s);
    rring_outer_loop(rduration, uint64_t, s) {
        rring_prefetch(rkey, 1);
        rring_prefetch(rduration, 1);

        rring_inner_preloop(rkey, short);
        rring_inner_preloop(rduration, uint64_t);
        rring_sync(rduration);

        rring_inner_loop(rduration, j) {
            short k = _inner_rkey[j];
            uint64_t d = _inner_rduration[j];
            printf("%d %lu\n", k, d);
        }
        rring_outer_loop_with_post(rkey);
    }
}

int main()
{
    init_client();
    cache_init();
    channel_init();
    std::chrono::time_point<std::chrono::steady_clock> times[10];
    void * df  = load_data();
    load_trip_timestamp();
    // specially loaded column at remote now

    times[0] = std::chrono::steady_clock::now();
    analyze_trip_durations_of_timestamps<short>("pickup_day");
    times[1] = std::chrono::steady_clock::now();
    analyze_trip_durations_of_timestamps<short>("pickup_month");
    times[2] = std::chrono::steady_clock::now();
    printf("Step 7: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
}

