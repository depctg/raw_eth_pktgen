#include <vector>
#include <unordered_set>
#include <chrono>
#include "internal.h"
#include "rvector.h"


template <typename T_Key>
void analyze_trip_durations_of_timestamps(const char* key_col_name)
{
    printf("analyze_trip_durations_of_timestamps() on key = %s\n", key_col_name);

    // auto copy_index        = get_index();
    // auto copy_key_col      = get_column<T_Key>(key_col_name);
    // auto copy_key_duration = get_column<uint64_t>("duration");

    // Take this out 
    step7_do_process(key_col_name);
    // Take this out

    printf("\n");
}

int main()
{
    std::chrono::time_point<std::chrono::steady_clock> times[10];
    void * df  = load_data();
    load_trip_timestamp();

    times[0] = std::chrono::steady_clock::now();
    analyze_trip_durations_of_timestamps<char>("pickup_day");
    times[1] = std::chrono::steady_clock::now();
    analyze_trip_durations_of_timestamps<char>("pickup_month");
    times[2] = std::chrono::steady_clock::now();
    printf("Step 7: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
}

