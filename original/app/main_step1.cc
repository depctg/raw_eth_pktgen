#include <vector>
#include <unordered_set>
#include <chrono>
#include "internal.h"
#include "rvector.h"

template<typename T>
size_t get_col_unique_values(const std::vector<T> & vec) {
    // auto                    hash_func =
    //     [](const T& v) -> std::size_t  {
    //         return(std::hash<T>{}(v));
    // };
    // auto                    equal_func =
    //     [](const T& lhs,
    //        const T& rhs) -> bool  {
    //         return(lhs == rhs);
    // };

    // std::unordered_set<T,
    //     decltype(hash_func),
    //     decltype(equal_func)>   table(vec.size(), hash_func, equal_func);
    // bool                        counted_nan = false;

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
    std::chrono::time_point<std::chrono::steady_clock> times[10];
    void * df  = load_data();
    times[0] = std::chrono::steady_clock::now();
    print_number_vendor_ids_and_unique();
    times[1] = std::chrono::steady_clock::now();
    printf("Step 1: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
}

