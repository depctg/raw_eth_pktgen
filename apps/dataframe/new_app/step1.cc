#include <vector>
#include <unordered_set>
#include <chrono>
#include "internal.h"
#include "rvector.h"
#include "common.h"
#include "cache.hpp"

// total num_eles approx 128 * 1024 * 1024
const size_t s1_nb = 512 * 1024;
const size_t s1_n_block = 256;

// token offset, raddr offset, laddr offset, slots, slot size bytes, id 
using rvid = DirectCache<0,0,0,s1_n_block,s1_nb * sizeof(int),0>;
using rvid_R = CacheReq<rvid>;

template<typename T>
size_t get_col_unique_values(const std::vector<T> & vec) {
    size_t N = vec.size();
    // TODO: disagg this variable
    std::vector<T>              result;
    result.reserve(N);

    for (size_t i = 0; i < N; i++)  {
        T e = vec[i];
        if (step1_firstTime(e))
            result.push_back(e);
    }

    return(result.size());
}

void print_number_vendor_ids_and_unique()
{
    printf("print_number_vendor_ids_and_unique()\n");
    printf("number of vendor_ids in the train dataset: %ld\n", 
        get_column<int>("VendorID").size());
    printf("Number of unique vendor_ids in the train dataset: %ld\n\n",
        get_col_unique_values(get_column<int>("VendorID")));
}

int main()
{
    init_client();
    std::chrono::time_point<std::chrono::steady_clock> times[10];
    void * df  = load_data();
    new_remotelize<int, rvid, rvid_R>(get_column<int>("VendorID"), true);
    printf("VendorID remote\n");
    times[0] = std::chrono::steady_clock::now();
    print_number_vendor_ids_and_unique();
    times[1] = std::chrono::steady_clock::now();
    printf("Step 1: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
}

