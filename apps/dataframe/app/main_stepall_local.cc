#include <vector>
#include <unordered_set>
#include <chrono>
#include "internal.h"
#include "rvector.h"
#include "simple_time.hpp"
#include "common.h"
#include <assert.h>
#include <cmath>

//================= STEP 1 =====================// 
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

    // rvector<int> *vids = (rvector<int> *) &get_column<int>("VendorID");
    // * ((rvector<int> *) offload_arg_buf) = *vids;
    // size_t unique_count = *(size_t *) call_offloaded_service(1, sizeof(*vids), sizeof(size_t));

    printf("Number of unique vendor_ids in the train dataset: %ld\n\n",
        get_col_unique_values(get_column<int>("VendorID")));
}


//=================== SETP 2 =======================//

template <typename K, typename T, typename F>
void step2_get_data_by_sel (const char *name, F &sel_func, std::vector<T>& newvec) {
    auto &indices_ = get_index();

    const std::vector<T>    &vec = get_column<T>(name);
    auto& passage_count_vec = get_column<int>("passenger_count");
    const size_type         idx_s = indices_.size();
    const size_type         col_s = vec.size();
    std::vector<size_type>  col_indices;

    // TODO: measure col_indices size
    // make sure this do not trigger realloc
    // and consider remotelize
    col_indices.reserve(idx_s);
    newvec.reserve(col_s);

    for (size_type i = 0; i < col_s; ++i) {
        if (sel_func (indices_[i], vec[i])) {
            col_indices.push_back(i);
            newvec.push_back(passage_count_vec[i]);
        }
    }

    // std::vector<int> new_vendor_id;
    // std::vector<int> &vids = get_column<int>("VendorID");
    // uint64_t vid_base = remoteAddr((void *)&vids[0]);
    // dummy_reader(new_vendor_id, (2<<20), 16, 8, sel_size, vid_base);

    const size_t sel_size = newvec.size();
    return;
}

void print_passage_counts_by_vendor_id(int vendor_id)
{
    printf("print_passage_counts_by_vendor_id(vendor_id), vendor_id = %d\n", vendor_id);

    auto sel_vendor_functor = [&](const uint64_t&, const int& vid) -> bool {
        return vid == vendor_id;
    };

    std::vector<int> passage_count_vec;
    step2_get_data_by_sel<int>("VendorID", sel_vendor_functor, passage_count_vec);

    size_t s = passage_count_vec.size();
    if (vendor_id == 1) {
        for (size_t i = 0; i < s; i++) 
            step21_passage_count(passage_count_vec[i]);
        step21_count_result();
    } else {
        for (size_t i = 0; i < s; i++) 
            step22_passage_count(passage_count_vec[i]);
        step22_count_result();
    }
    printf("\n");
}


//==================== STEP 3 =========================//
template <typename D, typename I = size_t>
class MaxVisitor {
public:
  I index_ = 0;
  D max_ = 0;
  bool is_first = true;

  void pre() {}
  void post() {}
  void operator()(I idx, D dat) {
    // printf("%lu\n", dat);
    if (is_first || dat > max_) {
      max_ = dat;
      index_ = idx;
      is_first = false;
        // printf("update max %lu\n", max_);
    }
  }
  D get_result () const  { return (max_); }
  I get_index () const  { return (index_); }
};

template <typename D, typename I = size_t>
class MinVisitor {
public:
  I index_ = 0;
  D min_ = 0;
  bool is_first = true;

  void pre() {}
  void post() {}
  void operator()(I idx, D dat) {
    if (is_first || dat < min_) {
      min_ = dat;
      index_ = idx;
      is_first = false;
    }
  }
  D get_result () const  { return (min_); }
  I get_index () const  { return (index_); }
};

template <typename D, typename I = size_t>
class MeanVisitor {
public:
  D mean_ = 0;
  size_type cnt_ = 0;

  void pre() { mean_ = 0; cnt_ = 0; }
  void post() {}
  void operator()(I idx, D dat) {
    mean_ += dat;
    cnt_ ++;
  }
  size_type get_count () const  { return (cnt_); }
  D get_sum () const  { return (mean_); }
  D get_result () const  {
    return (mean_ / D(cnt_));
  }
  
};

template<typename T, typename V1, typename V2, typename V3>
void visit (std::vector<T> &vec, V1 &v1, V2 &v2, V3 &v3)  {

    std::vector<size_t>& indices_ = get_index();

    const size_t    idx_s = indices_.size();
    const size_t    min_s = std::min<size_t   >(vec.size(), idx_s);

    v1.pre();
    v2.pre();
    v3.pre();
    for (size_t i = 0; i < min_s; ++i) {
        // printf("%lu\n", vec[i]);
        v1 (indices_[i], vec[i]);
        v2 (indices_[i], vec[i]);
        v3 (indices_[i], vec[i]);
    }
    v1.post();
    v2.post();
    v3.post();
}

void calculate_trip_duration()
{
    printf("calculate_trip_duration()\n" );

    // sizeof(SimpleTime) 8 bytes
    auto& pickup_time_vec  = get_column<SimpleTime>("tpep_pickup_datetime");
    auto& dropoff_time_vec = get_column<SimpleTime>("tpep_dropoff_datetime");
    assert(pickup_time_vec.size() == dropoff_time_vec.size());

    // Option 2: remotelize this
    std::vector<uint64_t> duration_vec;
    // duration_vec = __disagg_alloc();
    size_t N = pickup_time_vec.size();
    duration_vec.reserve(N);

    for (uint64_t i = 0; i < N; i++) {
        auto pickup_time_second  = pickup_time_vec[i].to_second();
        auto dropoff_time_second = dropoff_time_vec[i].to_second();
        // can directly change pointers.
        if (dropoff_time_second < pickup_time_second)
            duration_vec.push_back(0);
        else
            duration_vec.push_back(dropoff_time_second - pickup_time_second);
    }

    load_column("duration", std::move(duration_vec));

    MaxVisitor<uint64_t> max_visitor;
    MinVisitor<uint64_t> min_visitor;
    MeanVisitor<uint64_t> mean_visitor;

    visit(get_column<uint64_t>("duration"), max_visitor, min_visitor, mean_visitor);
    // visit(get_column<uint64_t>("duration"), min_visitor);
    // visit(get_column<uint64_t>("duration"), mean_visitor);

    printf("Mean duration %lu seconds\n", mean_visitor.get_result());
    printf("Min duration %lu seconds\n", min_visitor.get_result());
    printf("Max duration %lu seconds\n", max_visitor.get_result());
    printf("\n");
}


//================== STEP 4 ========================//

template <typename K, typename T, typename F>
void step4_get_data_by_sel (const char *name, F &sel_func, std::vector<T> &newvec) {
    auto &indices_ = get_index();

    const std::vector<K>    &vec = get_column<K>(name);
    const std::vector<int>  &vid = get_column<int>("VendorID");
    const size_type         idx_s = indices_.size();
    const size_type         col_s = vec.size();
    // std::vector<size_type>  col_indices;

    // make sure this do not trigger realloc
    // and consider remotelize
    // col_indices.reserve(idx_s);
    newvec.reserve(col_s);

    for (size_type i = 0; i < col_s; ++i)
        if (sel_func(indices_[i], vec[i])) {
            newvec.push_back(vid[i]);
        }

    return;
}


void calculate_distribution_store_and_fwd_flag()
{
    printf("calculate_distribution_store_and_fwd_flag()\n");

    auto sel_N_saff_functor = [&](const uint64_t&, const char& saff) -> bool {
        return saff == 'N';
    };

    std::vector<int> sel_whatever;
    step4_get_data_by_sel<char>("store_and_fwd_flag", sel_N_saff_functor, sel_whatever);

    printf("%f\n", static_cast<double>(sel_whatever.size()) / get_index().size());

    auto sel_Y_saff_functor = [&](const uint64_t&, const char& saff) -> bool {
        return saff == 'Y';
    };

    std::vector<int> sel_vendor_ids;
    step4_get_data_by_sel<char>("store_and_fwd_flag", sel_Y_saff_functor, sel_vendor_ids);

    std::vector<int> unique_vendor_id_vec;
    size_t N = sel_vendor_ids.size();
    for (size_t i = 0; i < N; i++)  {
        int e = sel_vendor_ids[i];
        if (step4_firstTime(e))
            unique_vendor_id_vec.push_back(e);
    }
    printf("{");
    for (auto& vector_id : unique_vendor_id_vec) {
        printf("%d, ", vector_id);
    }
    printf("}\n\n");
}

//======================== STEP 5 ============================//


static double haversine(double lat1, double lon1, double lat2, double lon2)
{
    // Distance between latitudes and longitudes
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    // Convert to radians.
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    // Apply formulae.
    double a   = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
    double rad = 6371;
    double c   = 2 * asin(sqrt(a));
    return rad * c;
}


// template: sel_by K, retrieve vec T, sel_fn F
template <typename K, typename T, typename F>
void step5_get_data_by_sel (const char *name, F &sel_func, 
                      const char *target_name, std::vector<T> &newvec) {
    auto &indices_ = get_index();

    const std::vector<K>    &vec = get_column<K>(name);
    const std::vector<T>    &vt = get_column<T>(target_name);
    const size_type         idx_s = indices_.size();
    const size_type         col_s = idx_s;
    // std::vector<size_type>  col_indices;

    // make sure this do not trigger realloc
    // and consider remotelize
    // col_indices.reserve(idx_s);
    newvec.reserve(col_s);

    for (size_type i = 0; i < col_s; ++i)
        if (sel_func(indices_[i], vec[i])) {
            newvec.push_back(vt[i]);
        }

    const size_t sel_size = newvec.size();

    return;
}

void calculate_haversine_distance_column()
{
    printf("calculate_haversine_distance_column()\n");
    auto& pickup_longitude_vec  = get_column<double>("pickup_longitude");
    auto& pickup_latitude_vec   = get_column<double>("pickup_latitude");
    auto& dropoff_longitude_vec = get_column<double>("dropoff_longitude");
    auto& dropoff_latitude_vec  = get_column<double>("dropoff_latitude");

    assert(pickup_longitude_vec.size() == pickup_latitude_vec.size());
    assert(pickup_longitude_vec.size() == dropoff_longitude_vec.size());
    assert(pickup_longitude_vec.size() == dropoff_latitude_vec.size());

    size_t N = pickup_latitude_vec.size();
    std::vector<double> haversine_vec;
    haversine_vec.reserve(N);

    for (uint64_t i = 0; i < N; i++) {
        haversine_vec.push_back(haversine(pickup_latitude_vec[i], pickup_longitude_vec[i],
                                                   dropoff_latitude_vec[i],
                                                   dropoff_longitude_vec[i]));
    }

    load_column("haversine_distance", std::move(haversine_vec));
                    
    auto sel_functor = [&](const uint64_t&, const double& dist) -> bool { return dist > 100; };

    std::vector<int> sel_whatever;
    step5_get_data_by_sel<double>("haversine_distance", sel_functor, "VendorID", sel_whatever);

    printf("Number of rows that have haversine_distance > 100 KM = %lu\n", sel_whatever.size());
    printf("\n");

}

//====================== STEP 7 =======================/
template <typename T_Key>
void analyze_trip_durations_of_timestamp(const char* key_col_name)
{
    printf("analyze_trip_durations_of_timestamps() on key = %s\n", key_col_name);

    // auto copy_index        = get_index();
    // auto copy_key_col      = get_column<T_Key>(key_col_name);
    // auto copy_key_duration = get_column<uint64_t>("duration");

    // Take this out 
    // can offload all
    // step7_do_process(key_col_name);
    // Take this out

    // data copy
    auto &copy_index        = get_index();
    auto &copy_key_col      = get_column<short>(key_col_name);
    auto &copy_key_duration = get_column<uint64_t>("duration");

    std::vector<size_t> local_index;
    local_index.reserve(copy_index.size());
    std::vector<short> local_key_col;
    local_key_col.reserve(copy_key_col.size());
    std::vector<uint64_t> local_key_duration;
    local_key_duration.reserve(copy_key_duration.size());

    size_t N = copy_key_col.size();

    for (size_t i = 0; i < N; ++ i) {
        local_index.push_back(copy_index[i]);
        local_key_col.push_back(copy_key_col[i]);
        local_key_duration.push_back(copy_key_duration[i]);
    }

    // printf("data copy done\n");
    step7_process_after_copy(key_col_name, local_index, local_key_col, local_key_duration);
}


int main() {
    void * df  = load_data();
    load_trip_timestamp();

    std::chrono::time_point<std::chrono::steady_clock> times[10];
    times[0] = std::chrono::steady_clock::now();
    print_number_vendor_ids_and_unique();
    times[1] = std::chrono::steady_clock::now();
    print_passage_counts_by_vendor_id(1);
    times[2] = std::chrono::steady_clock::now();
    print_passage_counts_by_vendor_id(2);
    times[3] = std::chrono::steady_clock::now();
    calculate_trip_duration();
    times[4] = std::chrono::steady_clock::now();
    calculate_distribution_store_and_fwd_flag();
    times[5] = std::chrono::steady_clock::now(); 
    calculate_haversine_distance_column();
    times[6] = std::chrono::steady_clock::now();
    analyze_trip_durations_of_timestamp<short>("pickup_day");
    times[7] = std::chrono::steady_clock::now();
    analyze_trip_durations_of_timestamp<short>("pickup_month");
    times[8] = std::chrono::steady_clock::now();

    printf("Step 1: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
    printf("Step 2-1: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[2] - times[1])
        .count());
    printf("Step 2-2: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[3] - times[2])
        .count());
    printf("Step 3: %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(times[4] - times[3]).count());
    printf("Step 4: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[5] - times[4])
        .count());
    printf("Step 5: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[6] - times[5])
        .count());

    printf("Step 7: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[7] - times[6])
        .count());

    printf("Step 8: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[8] - times[7])
        .count());    
    printf("Total: %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(times[8] - times[0])
        .count());
}

