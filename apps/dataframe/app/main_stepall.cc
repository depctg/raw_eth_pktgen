#include <vector>
#include <unordered_set>
#include <chrono>
#include "internal.h"
#include "rvector.h"
#include "simple_time.hpp"
#include "common.h"
#include <assert.h>
#include <cmath>

// total num_eles approx 128 * 1024 * 1024
const size_t sall_nb = 512 * 1024;
const size_t sall_n_block = 256;  // 82.3 MB/block (all steps) 

// total 30 GB -> local X * 30
rring_init(rvid,       int,        sall_nb * sizeof(int),        sall_n_block, 0, 8192);
rring_init(rpickdate,  SimpleTime, sall_nb * sizeof(SimpleTime), sall_n_block, 0, 8192 + (1ULL << 30));
rring_init(rdropdate,  SimpleTime, sall_nb * sizeof(SimpleTime), sall_n_block, 0, 8192 + (2ULL << 30));
rring_init(rpsgcnt,    int,        sall_nb * sizeof(int),        sall_n_block, 0, 8192 + (3ULL << 30));
rring_init(rtripdist,  double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (4ULL << 30));
rring_init(rplon,      double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (5ULL << 30));
rring_init(rplat,      double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (6ULL << 30));
rring_init(rrateid,    int,        sall_nb * sizeof(int),        sall_n_block, 0, 8192 + (7ULL << 30));
rring_init(rflag,      char,       sall_nb * sizeof(char),       sall_n_block, 0, 8192 + (8ULL << 30));
rring_init(rdlon,      double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (9ULL << 30));
rring_init(rdlat,      double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (10ULL << 30));
rring_init(rptype,     int,        sall_nb * sizeof(int),        sall_n_block, 0, 8192 + (11ULL << 30));
rring_init(rfare,      double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (12ULL << 30));
rring_init(rextra,     double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (13ULL << 30));
rring_init(rmta,       double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (14ULL << 30));
rring_init(rtip,       double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (15ULL << 30));
rring_init(rtolls,     double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (16ULL << 30));
rring_init(rimpv,      double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (17ULL << 30));
rring_init(rtotal,     double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (18ULL << 30));
rring_init(rids,       size_t,     sall_nb * sizeof(size_t),     sall_n_block, 0, 8192 + (19ULL << 30));
rring_init(rhaversine, double,     sall_nb * sizeof(double),     sall_n_block, 0, 8192 + (20ULL << 30));
rring_init(rduration,  uint64_t,   sall_nb * sizeof(uint64_t),   sall_n_block, 0, 8192 + (21ULL << 30));
rring_init(rpickday,   short,      sall_nb * sizeof(short),      sall_n_block, 0, 8192 + (22ULL << 30));
rring_init(rpickmonth, short,      sall_nb * sizeof(short),      sall_n_block, 0, 8192 + (23ULL << 30));

//================= STEP 1 =====================// 
template<typename T>
size_t get_col_unique_values(const std::vector<T> & vec) {
    size_t N = vec.size();
    // TODO: disagg this variable
    std::vector<T>              result;
    result.reserve(N);

    // for (size_t i = 0; i < N; i++)  {
    //     T e = vec[i];
    //     if (step1_firstTime(e))
    //         result.push_back(e);
    // }
    void *p = (void *)&vec[0];

    rring_outer_loop(rvid, int, N) {

        rring_prefetch(rvid, 16);
        rring_inner_preloop(rvid, int);
        rring_sync(rvid);

        rring_inner_loop(rvid, j) {
            T e = _inner_rvid[j];
            if (step1_firstTime(e))
                result.push_back(e);
        }
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
    const size_type         idx_s = indices_.size();
    const size_type         col_s = vec.size();
    std::vector<size_type>  col_indices;

    // TODO: measure col_indices size
    // make sure this do not trigger realloc
    // and consider remotelize
    col_indices.reserve(idx_s);
    newvec.reserve(col_s);

    // for (size_type i = 0; i < col_s; ++i)
    //     if (sel_func (indices_[i], vec[i])) {
    //     }

    rring_outer_loop_with(rvid, col_s);
    rring_outer_loop_with(rpsgcnt, col_s);
    rring_outer_loop(rids, size_t, col_s) {
        rring_prefetch_with(rids, rvid, 8);
        rring_prefetch_with(rids, rpsgcnt, 8);
        rring_prefetch(rids, 8);

        rring_inner_preloop(rvid, int);
        rring_inner_preloop(rpsgcnt, int);
        rring_inner_preloop(rids, size_t);

        rring_sync(rids);

        rring_inner_loop(rids, j) {
            size_t index = _inner_rids[j];
            int fby = _inner_rvid[j];
            int rget = _inner_rpsgcnt[j];
            // printf("%lu %d %d\n", index, fby, rget);
            if (sel_func (index, fby)) {
                // dummy
                size_t nth = _t_rids * _bn_rids + j;
                col_indices.push_back(nth);
                newvec.push_back(rget);
            }
        }
        rring_outer_loop_with_post(rvid);
        rring_outer_loop_with_post(rpsgcnt);
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

template<typename T, typename V>
V visit (std::vector<T> &vec, V &visitor)  {

    std::vector<size_t>& indices_ = get_index();

    const size_t    idx_s = indices_.size();
    const size_t    min_s = std::min<size_t   >(vec.size(), idx_s);

    // visitor.pre();
    // for (size_t i = 0; i < min_s; ++i) {
    //     // printf("%lu\n", vec[i]);
    //     visitor (indices_[i], vec[i]);
    // }
    // visitor.post();

    visitor.pre();
    rring_outer_loop_with(rduration, min_s);
    rring_outer_loop(rids, size_t, min_s) {
      rring_prefetch_with(rids, rduration, 16);
      rring_prefetch(rids, 16);

      rring_inner_preloop(rduration, uint64_t);
      rring_inner_preloop(rids, size_t);

      rring_sync(rids);
      rring_inner_loop(rids, j) {
          size_t index = _inner_rids[j];
          uint64_t e = _inner_rduration[j];
          visitor (index, e);
      }
      rring_outer_loop_with_post(rduration);
    }

    visitor.post();
    return (visitor);
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

    // for (uint64_t i = 0; i < N; i++) {
    //     auto pickup_time_second  = pickup_time_vec[i].to_second();
    //     auto dropoff_time_second = dropoff_time_vec[i].to_second();
    //     // can directly change pointers.
    //     if (dropoff_time_second < pickup_time_second)
    //         duration_vec.push_back(0);
    //     else
    //         duration_vec.push_back(dropoff_time_second - pickup_time_second);
    // }

    rring_outer_loop_with(rpickdate, N);
    rring_outer_loop(rdropdate, SimpleTime, N) {
        rring_prefetch_with(rdropdate, rpickdate, 8);
        rring_prefetch(rdropdate, 8);

        rring_inner_preloop(rpickdate, SimpleTime);
        rring_inner_preloop(rdropdate, SimpleTime);
        rring_sync(rdropdate);

        rring_inner_loop(rdropdate, j) {
          uint64_t pickup_time_second  = _inner_rpickdate[j].to_second();
          uint64_t dropoff_time_second = _inner_rdropdate[j].to_second();
          if (dropoff_time_second < pickup_time_second)
            duration_vec.push_back(0);
          else
            duration_vec.push_back(dropoff_time_second - pickup_time_second);
        }
        rring_outer_loop_with_post(rpickdate);
    }

    load_column("duration", std::move(duration_vec));

    // remotelize in load_column
    {
      auto &duration_vec = get_column<uint64_t>("duration");
      rvector<uint64_t> * rv_duration = (rvector<uint64_t> *) &duration_vec;
      size_t s = duration_vec.size();
      size_t c = duration_vec.capacity();
      rring_outer_loop(rduration, uint64_t, c) {
        rring_inner_preloop(rduration, uint64_t);
        rring_sync_writeonly(rduration);
        rring_inner_loop(rduration, j) {
          size_t nth = _t_rduration * _bn_rduration + j;
          _inner_rduration[j] = rv_duration->head[nth];
        }
        rring_inner_wb(rduration);
      }
      rring_cleanup_writeonly(rduration);
      duration_vec.clear();
      duration_vec.shrink_to_fit();
      rv_duration->head = (uint64_t *) (4096ULL + (20ULL << 30));
      rv_duration->end  = rv_duration->head + s;
      rv_duration->tail = rv_duration->head + c;
    }

    MaxVisitor<uint64_t> max_visitor;
    MinVisitor<uint64_t> min_visitor;
    MeanVisitor<uint64_t> mean_visitor;

    visit(get_column<uint64_t>("duration"), max_visitor);
    visit(get_column<uint64_t>("duration"), min_visitor);
    visit(get_column<uint64_t>("duration"), mean_visitor);

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
    const size_type         idx_s = indices_.size();
    const size_type         col_s = vec.size();
    // std::vector<size_type>  col_indices;

    // make sure this do not trigger realloc
    // and consider remotelize
    // col_indices.reserve(idx_s);
    newvec.reserve(col_s);

    // for (size_type i = 0; i < col_s; ++i)
    //     if (sel_func(indices_[i], vec[i])) {
    //         col_indices.push_back(i);
    //     }

    rring_outer_loop_with(rvid, col_s);
    rring_outer_loop_with(rflag, col_s);
    rring_outer_loop(rids, size_t, col_s) {
        rring_prefetch_with(rids, rflag, 8);
        rring_prefetch_with(rids, rvid, 8);
        rring_prefetch(rids, 8);

        rring_inner_preloop(rflag, K);
        rring_inner_preloop(rvid, T);
        rring_inner_preloop(rids, size_t);

        rring_sync(rids);

        rring_inner_loop(rids, j) {
            size_t index = _inner_rids[j];
            K fby = _inner_rflag[j];
            T rget = _inner_rvid[j];
            // printf("%lu %d %d\n", index, fby, rget);
            if (sel_func (index, fby)) {
                // nth = _t_<name> * _bn_<name> + j;
                // size_t nth = _t_rids * _bn_rids + j;
                // col_indices.push_back(nth);
                newvec.push_back(rget);
            }
        }
        rring_outer_loop_with_post(rvid);
        rring_outer_loop_with_post(rflag);
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

    // const std::vector<K>    &vec = get_column<K>(name);
    const size_type         idx_s = indices_.size();
    const size_type         col_s = idx_s;
    // std::vector<size_type>  col_indices;

    // make sure this do not trigger realloc
    // and consider remotelize
    // col_indices.reserve(idx_s);
    newvec.reserve(col_s);

    // for (size_type i = 0; i < col_s; ++i)
    //     if (sel_func(indices_[i], vec[i])) {
    //         col_indices.push_back(i);
    //     }

    rring_outer_loop_with(rhaversine, col_s);
    rring_outer_loop_with(rvid, col_s);
    rring_outer_loop(rids, size_t, col_s) {
        rring_prefetch_with(rids, rhaversine, 8);
        rring_prefetch_with(rids, rvid, 8);
        rring_prefetch(rids, 8);

        rring_inner_preloop(rhaversine, K);
        rring_inner_preloop(rvid, T);
        rring_inner_preloop(rids, size_t);

        rring_sync(rids);

        rring_inner_loop(rids, j) {
            size_t index = _inner_rids[j];
            K fby = _inner_rhaversine[j];
            T rget = _inner_rvid[j];
            // printf("%lu %d %d\n", index, fby, rget);
            if (sel_func (index, fby)) {
                // nth = _t_<name> * _bn_<name> + j;
                // size_t nth = _t_rids * _bn_rids + j;
                // col_indices.push_back(nth);
                newvec.push_back(rget);
            }
        }
        rring_outer_loop_with_post(rvid);
        rring_outer_loop_with_post(rhaversine);
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

    // for (uint64_t i = 0; i < N; i++) {
    //     haversine_distance_vec.push_back(haversine(pickup_latitude_vec[i], pickup_longitude_vec[i],
    //                                                dropoff_latitude_vec[i],
    //                                                dropoff_longitude_vec[i]));
    // }

    auto t0 = std::chrono::steady_clock::now();

    rring_outer_loop_with(rplon, N);
    rring_outer_loop_with(rplat,  N);
    rring_outer_loop_with(rdlon, N);
    rring_outer_loop(rdlat, double, N) {
        rring_prefetch_with(rdlat, rplon, 8);
        rring_prefetch_with(rdlat, rplat, 8);
        rring_prefetch_with(rdlat, rdlon, 8);
        rring_prefetch(rdlat, 8);

        rring_inner_preloop(rplon, double);
        rring_inner_preloop(rplat, double);
        rring_inner_preloop(rdlon, double);
        rring_inner_preloop(rdlat, double);

        rring_sync(rdlat);

        double computet = 0, pusht = 0;
        rring_inner_loop(rdlat, j) {
            auto rplat = _inner_rplat[j];
            auto rplon = _inner_rplon[j];
            auto rdlat = _inner_rdlat[j];
            auto rdlon = _inner_rdlon[j];
            double rel = haversine(
                rplat, rplon, rdlat, rdlon
                // _inner_rplat[j], _inner_rplon[j],
                // _inner_rdlat[j], _inner_rdlon[j]
            );
            haversine_vec[_t_rdlat * _bn_rdlat + j] = (rel);
        }

        // printf("compute loop haversine %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(te - ts).count());

        rring_outer_loop_with_post(rplon);
        rring_outer_loop_with_post(rplat);
        rring_outer_loop_with_post(rdlon);
    }
    // auto t1 = std::chrono::steady_clock::now();
    // printf("populate local haversine %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count());
    // load_column("haversine_distance", std::move(haversine_vec));
    // remotalize haversine
    {
        rring_outer_loop(rhaversine, double, N) {
        rring_inner_preloop(rhaversine, double);
        rring_sync_writeonly(rhaversine);
        rring_inner_loop(rhaversine, j) {
            size_t nth = _t_rhaversine * _bn_rhaversine + j;
            _inner_rhaversine[j] = haversine_vec[nth];
        }
        rring_inner_wb(rhaversine);
        }
        rring_cleanup_writeonly(rhaversine);
    }
    // auto t2 = std::chrono::steady_clock::now();
    // printf("remotelize local haversine %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count());

    // load_column("haversine_distance", std::move(haversine_distance_vec));
                    
    // Can jump this part
    auto sel_functor = [&](const uint64_t&, const double& dist) -> bool { return dist > 100; };

    std::vector<int> sel_whatever;
    step5_get_data_by_sel<double>("haversine_distance", sel_functor, "VendorID", sel_whatever);
    // auto t3 = std::chrono::steady_clock::now();
    // printf("get by sel %ld us\n", std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count());

    printf("Number of rows that have haversine_distance > 100 KM = %lu\n", sel_whatever.size());
    printf("\n");

}

//====================== STEP 7 =======================/
template <typename T_Key>
void analyze_trip_durations_of_day(const char* key_col_name)
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

    rring_outer_loop_with(rpickday, N);
    rring_outer_loop_with(rduration, N);
    rring_outer_loop(rids, size_t, N) {
        rring_prefetch_with(rids, rpickday, 8);
        rring_prefetch_with(rids, rduration, 8);
        rring_prefetch(rids, 8);

        rring_inner_preloop(rpickday, short);
        rring_inner_preloop(rduration, uint64_t);
        rring_inner_preloop(rids, size_t);

        rring_sync(rids);

        rring_inner_loop(rids, j) {
            size_t index = _inner_rids[j];
            short col = _inner_rpickday[j];
            uint64_t d = _inner_rduration[j];
            // printf("%lu %d %d\n", index, fby, rget);
            local_index.push_back(index);
            local_key_col.push_back(col);
            local_key_duration.push_back(d);
        }
        rring_outer_loop_with_post(rpickday);
        rring_outer_loop_with_post(rduration);
    }

    // printf("data copy done\n");
    step7_process_after_copy(key_col_name, local_index, local_key_col, local_key_duration);
}

template <typename T_Key>
void analyze_trip_durations_of_month(const char* key_col_name)
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

    rring_outer_loop_with(rpickmonth, N);
    rring_outer_loop_with(rduration, N);
    rring_outer_loop(rids, size_t, N) {
        rring_prefetch_with(rids, rpickmonth, 8);
        rring_prefetch_with(rids, rduration, 8);
        rring_prefetch(rids, 8);

        rring_inner_preloop(rpickmonth, short);
        rring_inner_preloop(rduration, uint64_t);
        rring_inner_preloop(rids, size_t);

        rring_sync(rids);

        rring_inner_loop(rids, j) {
            size_t index = _inner_rids[j];
            short col = _inner_rpickmonth[j];
            uint64_t d = _inner_rduration[j];
            // printf("%lu %d %d\n", index, fby, rget);
            local_index.push_back(index);
            local_key_col.push_back(col);
            local_key_duration.push_back(d);
        }
        rring_outer_loop_with_post(rpickmonth);
        rring_outer_loop_with_post(rduration);
    }

    // printf("data copy done\n");
    step7_process_after_copy(key_col_name, local_index, local_key_col, local_key_duration);
}

int main() {
init_client();
    void * df  = load_data();
    load_trip_timestamp();

_lbase_rvid = (uint64_t) rbuf;
_lbase_rpickdate = (uint64_t) rbuf + (1ULL << 30);
_lbase_rdropdate = (uint64_t) rbuf + (2ULL << 30);
_lbase_rpsgcnt = (uint64_t) rbuf + (3ULL << 30);
_lbase_rtripdist = (uint64_t) rbuf + (4ULL << 30);
_lbase_rplon = (uint64_t) rbuf + (5ULL << 30);
_lbase_rplat = (uint64_t) rbuf + (6ULL << 30);
_lbase_rrateid = (uint64_t) rbuf + (7ULL << 30);
_lbase_rflag = (uint64_t) rbuf + (8ULL << 30);
_lbase_rdlon = (uint64_t) rbuf + (9ULL << 30);
_lbase_rdlat = (uint64_t) rbuf + (10ULL << 30);
_lbase_rptype = (uint64_t) rbuf + (11ULL << 30);
_lbase_rfare = (uint64_t) rbuf + (12ULL << 30);
_lbase_rextra = (uint64_t) rbuf + (13ULL << 30);
_lbase_rmta = (uint64_t) rbuf + (14ULL << 30);
_lbase_rtip = (uint64_t) rbuf + (15ULL << 30);
_lbase_rtolls = (uint64_t) rbuf + (16ULL << 30);
_lbase_rimpv = (uint64_t) rbuf + (17ULL << 30);
_lbase_rtotal = (uint64_t) rbuf + (18ULL << 30);
_lbase_rids = (uint64_t) rbuf + (19ULL << 30);
_lbase_rhaversine = (uint64_t) rbuf + (20ULL << 30);
_lbase_rduration = (uint64_t) rbuf + (21ULL << 30);
_lbase_rpickday = (uint64_t) rbuf + (22ULL << 30);
_lbase_rpickmonth = (uint64_t) rbuf + (23ULL << 30);

{
    auto &vendorid = get_column<int>("VendorID");
    rvector<int> * rv_vendorid = (rvector<int> *) &vendorid;
    size_t s = vendorid.size();
    size_t c = vendorid.capacity();
    rring_outer_loop(rvid, int, c) {
    rring_inner_preloop(rvid, int);
    rring_sync_writeonly(rvid);
    rring_inner_loop(rvid, j) {
        size_t nth = _t_rvid * _bn_rvid + j;
        _inner_rvid[j] = rv_vendorid->head[nth];
    }
    rring_inner_wb(rvid);
    }
    rring_cleanup_writeonly(rvid);
    vendorid.clear();
    vendorid.shrink_to_fit();
    rv_vendorid->head = (int *) (4096ULL);
    rv_vendorid->end  = rv_vendorid->head + s;
    rv_vendorid->tail = rv_vendorid->head + c;
    printf("VendorID remote\n");
}

{
    auto &pickupvec = get_column<SimpleTime>("tpep_pickup_datetime");
    rvector<SimpleTime> * rv_pick = (rvector<SimpleTime> *) &pickupvec;
    size_t s = pickupvec.size();
    size_t c = pickupvec.capacity();
    rring_outer_loop(rpickdate, SimpleTime, c) {
    rring_inner_preloop(rpickdate, SimpleTime);
    rring_sync_writeonly(rpickdate);
    rring_inner_loop(rpickdate, j) {
        size_t nth = _t_rpickdate * _bn_rpickdate + j;
        _inner_rpickdate[j] = rv_pick->head[nth];
    }
    rring_inner_wb(rpickdate);
    }
    rring_cleanup_writeonly(rpickdate);
    pickupvec.clear();
    pickupvec.shrink_to_fit();
    rv_pick->head = (SimpleTime *) (4096ULL + (1ULL << 30));
    rv_pick->end  = rv_pick->head + s;
    rv_pick->tail = rv_pick->head + c;
    printf("pickup remote\n");
}


{
    auto &dropvec = get_column<SimpleTime>("tpep_dropoff_datetime");
    rvector<SimpleTime> * rv_drop = (rvector<SimpleTime> *) &dropvec;
    size_t s = dropvec.size();
    size_t c = dropvec.capacity();
    rring_outer_loop(rdropdate, SimpleTime, c) {
    rring_inner_preloop(rdropdate, SimpleTime);
    rring_sync_writeonly(rdropdate);
    rring_inner_loop(rdropdate, j) {
        size_t nth = _t_rdropdate * _bn_rdropdate + j;
        _inner_rdropdate[j] = rv_drop->head[nth];
    }
    rring_inner_wb(rdropdate);
    }
    rring_cleanup_writeonly(rdropdate);
    dropvec.clear();
    dropvec.shrink_to_fit();
    rv_drop->head = (SimpleTime *) (4096ULL + (2ULL << 30));
    rv_drop->end  = rv_drop->head + s;
    rv_drop->tail = rv_drop->head + c;
    printf("dropoff remote\n");
}

{
    auto &psgvec = get_column<int>("passenger_count");
    rvector<int> * rv_psg = (rvector<int> *) &psgvec;
    size_t s = psgvec.size();
    size_t c = psgvec.capacity();
    rring_outer_loop(rpsgcnt, int, c) {
    rring_inner_preloop(rpsgcnt, int);
    rring_sync_writeonly(rpsgcnt);
    rring_inner_loop(rpsgcnt, j) {
        size_t nth = _t_rpsgcnt * _bn_rpsgcnt + j;
        _inner_rpsgcnt[j] = rv_psg->head[nth];
    }
    rring_inner_wb(rpsgcnt);
    }
    rring_cleanup_writeonly(rpsgcnt);
    psgvec.clear();
    psgvec.shrink_to_fit();
    rv_psg->head = (int *) (4096ULL + (3ULL << 30));
    rv_psg->end  = rv_psg->head + s;
    rv_psg->tail = rv_psg->head + c;
    printf("psg cnt remote\n");
}

{
    auto &distvec = get_column<double>("trip_distance");
    rvector<double> * rv_dist = (rvector<double> *) &distvec;
    size_t s = distvec.size();
    size_t c = distvec.capacity();
    rring_outer_loop(rtripdist, double, c) {
    rring_inner_preloop(rtripdist, double);
    rring_sync_writeonly(rtripdist);
    rring_inner_loop(rtripdist, j) {
        size_t nth = _t_rtripdist * _bn_rtripdist + j;
        _inner_rtripdist[j] = rv_dist->head[nth];
    }
    rring_inner_wb(rtripdist);
    }
    rring_cleanup_writeonly(rtripdist);
    distvec.clear();
    distvec.shrink_to_fit();
    rv_dist->head = (double *) (4096ULL + (4ULL << 30));
    rv_dist->end  = rv_dist->head + s;
    rv_dist->tail = rv_dist->head + c;
    printf("trip dist remote\n");
}

{
    auto &plonvec = get_column<double>("pickup_longitude");
    rvector<double> * rv_plon = (rvector<double> *) &plonvec;
    size_t s = plonvec.size();
    size_t c = plonvec.capacity();
    rring_outer_loop(rplon, double, c) {
    rring_inner_preloop(rplon, double);
    rring_sync_writeonly(rplon);
    rring_inner_loop(rplon, j) {
        size_t nth = _t_rplon * _bn_rplon + j;
        _inner_rplon[j] = rv_plon->head[nth];
    }
    rring_inner_wb(rplon);
    }
    rring_cleanup_writeonly(rplon);
    plonvec.clear();
    plonvec.shrink_to_fit();
    rv_plon->head = (double *) (4096ULL + (5ULL << 30));
    rv_plon->end  = rv_plon->head + s;
    rv_plon->tail = rv_plon->head + c;
    printf("pickup lon remote\n");
}

{
    auto &platvec = get_column<double>("pickup_latitude");
    rvector<double> * rv_plat = (rvector<double> *) &platvec;
    size_t s = platvec.size();
    size_t c = platvec.capacity();
    rring_outer_loop(rplat, double, c) {
    rring_inner_preloop(rplat, double);
    rring_sync_writeonly(rplat);
    rring_inner_loop(rplat, j) {
        size_t nth = _t_rplat * _bn_rplat + j;
        _inner_rplat[j] = rv_plat->head[nth];
    }
    rring_inner_wb(rplat);
    }
    rring_cleanup_writeonly(rplat);
    platvec.clear();
    platvec.shrink_to_fit();
    rv_plat->head = (double *) (4096ULL + (6ULL << 30));
    rv_plat->end  = rv_plat->head + s;
    rv_plat->tail = rv_plat->head + c;
    printf("pickup lat remote\n");
}
{
    auto &ratevec = get_column<int>("RatecodeID");
    rvector<int> * rv_rate = (rvector<int> *) &ratevec;
    size_t s = ratevec.size();
    size_t c = ratevec.capacity();
    rring_outer_loop(rrateid, int, c) {
    rring_inner_preloop(rrateid, int);
    rring_sync_writeonly(rrateid);
    rring_inner_loop(rrateid, j) {
        size_t nth = _t_rrateid * _bn_rrateid + j;
        _inner_rrateid[j] = rv_rate->head[nth];
    }
    rring_inner_wb(rrateid);
    }
    rring_cleanup_writeonly(rrateid);
    ratevec.clear();
    ratevec.shrink_to_fit();
    rv_rate->head = (int *) (4096ULL + (7ULL << 30));
    rv_rate->end  = rv_rate->head + s;
    rv_rate->tail = rv_rate->head + c;
    printf("ratecodeID remote\n");
}
{
    auto &flag_vec = get_column<char>("store_and_fwd_flag");
    rvector<char> * rv_flag = (rvector<char> *) &flag_vec;
    size_t s = flag_vec.size();
    size_t c = flag_vec.capacity();
    rring_outer_loop(rflag, char, c) {
    rring_inner_preloop(rflag, char);
    rring_sync_writeonly(rflag);
    rring_inner_loop(rflag, j) {
        size_t nth = _t_rflag * _bn_rflag + j;
        _inner_rflag[j] = rv_flag->head[nth];
    }
    rring_inner_wb(rflag);
    }
    rring_cleanup_writeonly(rflag);
    flag_vec.clear();
    flag_vec.shrink_to_fit();
    rv_flag->head = (char *) (4096ULL + (8ULL << 30));
    rv_flag->end  = rv_flag->head + s;
    rv_flag->tail = rv_flag->head + c;
    printf("store and fwd flag remote\n");
}
{
    auto &dlon_vec = get_column<double>("dropoff_longitude");
    rvector<double> * rv_dlon = (rvector<double> *) &dlon_vec;
    size_t s = dlon_vec.size();
    size_t c = dlon_vec.capacity();
    rring_outer_loop(rdlon, double, c) {
    rring_inner_preloop(rdlon, double);
    rring_sync_writeonly(rdlon);
    rring_inner_loop(rdlon, j) {
        size_t nth = _t_rdlon * _bn_rdlon + j;
        _inner_rdlon[j] = rv_dlon->head[nth];
    }
    rring_inner_wb(rdlon);
    }
    rring_cleanup_writeonly(rdlon);
    dlon_vec.clear();
    dlon_vec.shrink_to_fit();
    rv_dlon->head = (double *) (4096ULL + (9ULL << 30));
    rv_dlon->end  = rv_dlon->head + s;
    rv_dlon->tail = rv_dlon->head + c;
    printf("dropoff lon remote\n");
}
{
    auto &dlat_vec = get_column<double>("dropoff_latitude");
    rvector<double> * rv_dlat = (rvector<double> *) &dlat_vec;
    size_t s = dlat_vec.size();
    size_t c = dlat_vec.capacity();
    rring_outer_loop(rdlat, double, c) {
    rring_inner_preloop(rdlat, double);
    rring_sync_writeonly(rdlat);
    rring_inner_loop(rdlat, j) {
        size_t nth = _t_rdlat * _bn_rdlat + j;
        _inner_rdlat[j] = rv_dlat->head[nth];
    }
    rring_inner_wb(rdlat);
    }
    rring_cleanup_writeonly(rdlat);
    dlat_vec.clear();
    dlat_vec.shrink_to_fit();
    rv_dlat->head = (double *) (4096ULL + (10ULL << 30));
    rv_dlat->end  = rv_dlat->head + s;
    rv_dlat->tail = rv_dlat->head + c;
    printf("dropoff lat remote\n");
}
{
    auto &type_vec = get_column<int>("payment_type");
    rvector<int> * rv_type = (rvector<int> *) &type_vec;
    size_t s = type_vec.size();
    size_t c = type_vec.capacity();
    rring_outer_loop(rptype, int, c) {
    rring_inner_preloop(rptype, int);
    rring_sync_writeonly(rptype);
    rring_inner_loop(rptype, j) {
        size_t nth = _t_rptype * _bn_rptype + j;
        _inner_rptype[j] = rv_type->head[nth];
    }
    rring_inner_wb(rptype);
    }
    rring_cleanup_writeonly(rptype);
    type_vec.clear();
    type_vec.shrink_to_fit();
    rv_type->head = (int *) (4096ULL + (11ULL << 30));
    rv_type->end  = rv_type->head + s;
    rv_type->tail = rv_type->head + c;
    printf("payment type remote\n");
}
{
    auto &fare_vec = get_column<double>("fare_amount");
    rvector<double> * rv_fare = (rvector<double> *) &fare_vec;
    size_t s = fare_vec.size();
    size_t c = fare_vec.capacity();
    rring_outer_loop(rfare, double, c) {
    rring_inner_preloop(rfare, double);
    rring_sync_writeonly(rfare);
    rring_inner_loop(rfare, j) {
        size_t nth = _t_rfare * _bn_rfare + j;
        _inner_rfare[j] = rv_fare->head[nth];
    }
    rring_inner_wb(rfare);
    }
    rring_cleanup_writeonly(rfare);
    fare_vec.clear();
    fare_vec.shrink_to_fit();
    rv_fare->head = (double *) (4096ULL + (12ULL << 30));
    rv_fare->end  = rv_fare->head + s;
    rv_fare->tail = rv_fare->head + c;
    printf("fare amount remote\n");
}
{
    auto &extra_vec = get_column<double>("extra");
    rvector<double> * rv_extra = (rvector<double> *) &extra_vec;
    size_t s = extra_vec.size();
    size_t c = extra_vec.capacity();
    rring_outer_loop(rextra , double, c) {
    rring_inner_preloop(rextra, double);
    rring_sync_writeonly(rextra);
    rring_inner_loop(rextra, j) {
        size_t nth = _t_rextra * _bn_rextra + j;
        _inner_rextra[j] = rv_extra->head[nth];
    }
    rring_inner_wb(rextra);
    }
    rring_cleanup_writeonly(rextra);
    extra_vec.clear();
    extra_vec.shrink_to_fit();
    rv_extra->head = (double *) (4096ULL + (13ULL << 30));
    rv_extra->end  = rv_extra->head + s;
    rv_extra->tail = rv_extra->head + c;
    printf("extra remote\n");
}
{
    auto &mta_vec = get_column<double>("mta_tax");
    rvector<double> * rv_mta = (rvector<double> *) &mta_vec;
    size_t s = mta_vec.size();
    size_t c = mta_vec.capacity();
    rring_outer_loop(rmta, double, c) {
    rring_inner_preloop(rmta, double);
    rring_sync_writeonly(rmta);
    rring_inner_loop(rmta, j) {
        size_t nth = _t_rmta * _bn_rmta + j;
        _inner_rmta[j] = rv_mta->head[nth];
    }
    rring_inner_wb(rmta);
    }
    rring_cleanup_writeonly(rmta);
    mta_vec.clear();
    mta_vec.shrink_to_fit();
    rv_mta->head = (double *) (4096ULL + (14ULL << 30));
    rv_mta->end  = rv_mta->head + s;
    rv_mta->tail = rv_mta->head + c;
    printf("mta tax\n");
}
{
    auto &tip_vec = get_column<double>("tip_amount");
    rvector<double> * rv_tip = (rvector<double> *) &tip_vec;
    size_t s = tip_vec.size();
    size_t c = tip_vec.capacity();
    rring_outer_loop(rtip, double, c) {
    rring_inner_preloop(rtip, double);
    rring_sync_writeonly(rtip);
    rring_inner_loop(rtip, j) {
        size_t nth = _t_rtip * _bn_rtip + j;
        _inner_rtip[j] = rv_tip->head[nth];
    }
    rring_inner_wb(rtip);
    }
    rring_cleanup_writeonly(rtip);
    tip_vec.clear();
    tip_vec.shrink_to_fit();
    rv_tip->head = (double *) (4096ULL + (15ULL << 30));
    rv_tip->end  = rv_tip->head + s;
    rv_tip->tail = rv_tip->head + c;
    printf("tip amount\n");
}
{
    auto &tolls_vec = get_column<double>("tolls_amount");
    rvector<double> * rv_tolls = (rvector<double> *) &tolls_vec;
    size_t s = tolls_vec.size();
    size_t c = tolls_vec.capacity();
    rring_outer_loop(rtolls, double, c) {
    rring_inner_preloop(rtolls, double);
    rring_sync_writeonly(rtolls);
    rring_inner_loop(rtolls, j) {
        size_t nth = _t_rtolls * _bn_rtolls + j;
        _inner_rtolls[j] = rv_tolls->head[nth];
    }
    rring_inner_wb(rtolls);
    }
    rring_cleanup_writeonly(rtolls);
    tolls_vec.clear();
    tolls_vec.shrink_to_fit();
    rv_tolls->head = (double *) (4096ULL + (16ULL << 30));
    rv_tolls->end  = rv_tolls->head + s;
    rv_tolls->tail = rv_tolls->head + c;
    printf("tolls amount\n");
}
{
    auto &impv_vec = get_column<double>("improvement_surcharge");
    rvector<double> * rv_impv = (rvector<double> *) &impv_vec;
    size_t s = impv_vec.size();
    size_t c = impv_vec.capacity();
    rring_outer_loop(rimpv, double, c) {
    rring_inner_preloop(rimpv, double);
    rring_sync_writeonly(rimpv);
    rring_inner_loop(rimpv, j) {
        size_t nth = _t_rimpv * _bn_rimpv + j;
        _inner_rimpv[j] = rv_impv->head[nth];
    }
    rring_inner_wb(rimpv);
    }
    rring_cleanup_writeonly(rimpv);
    impv_vec.clear();
    impv_vec.shrink_to_fit();
    rv_impv->head = (double *) (4096ULL + (17ULL << 30));
    rv_impv->end  = rv_impv->head + s;
    rv_impv->tail = rv_impv->head + c;
    printf("imp surcharge\n");
}
{
    auto &total_vec = get_column<double>("total_amount");
    rvector<double> * rv_total = (rvector<double> *) &total_vec;
    size_t s = total_vec.size();
    size_t c = total_vec.capacity();
    rring_outer_loop(rtotal, double, c) {
    rring_inner_preloop(rtotal, double);
    rring_sync_writeonly(rtotal);
    rring_inner_loop(rtotal, j) {
        size_t nth = _t_rtotal * _bn_rtotal + j;
        _inner_rtotal[j] = rv_total->head[nth];
    }
    rring_inner_wb(rtotal);
    }
    rring_cleanup_writeonly(rtotal);
    total_vec.clear();
    total_vec.shrink_to_fit();
    rv_total->head = (double *) (4096ULL + (18ULL << 30));
    rv_total->end  = rv_total->head + s;
    rv_total->tail = rv_total->head + c;
    printf("total amount\n");
}
{
    auto &vidx = get_index();
    rvector<size_t> * rv_idx = (rvector<size_t> *) &vidx;
    size_t s = vidx.size();
    size_t c = vidx.capacity();
    rring_outer_loop(rids, size_t, c) {
    rring_inner_preloop(rids, size_t);
    rring_sync_writeonly(rids);
    rring_inner_loop(rids, j) {
        size_t nth = _t_rids * _bn_rids + j;
        _inner_rids[j] = rv_idx->head[nth];
    }
    rring_inner_wb(rids);
    }
    rring_cleanup_writeonly(rids);
    vidx.clear();
    vidx.shrink_to_fit();
    rv_idx->head = (size_t *) (4096ULL + (19ULL<<30));
    rv_idx->end  = rv_idx->head + s;
    rv_idx->tail = rv_idx->head + c;
    printf("index remote\n");
}

{
    auto &pday_vec = get_column<short>("pickup_day");
    rvector<short> * rv_pday = (rvector<short> *) &pday_vec;
    size_t s = pday_vec.size();
    size_t c = pday_vec.capacity();
    rring_outer_loop(rpickday, short, c) {
    rring_inner_preloop(rpickday, short);
    rring_sync_writeonly(rpickday);
    rring_inner_loop(rpickday, j) {
        size_t nth = _t_rpickday * _bn_rpickday + j;
        _inner_rpickday[j] = rv_pday->head[nth];
    }
    rring_inner_wb(rpickday);
    }
    rring_cleanup_writeonly(rpickday);
    pday_vec.clear();
    pday_vec.shrink_to_fit();
    rv_pday->head = (short *) (4096ULL + (22ULL << 30));
    rv_pday->end  = rv_pday->head + s;
    rv_pday->tail = rv_pday->head + c;
    printf("pickup day\n");
}
{
    auto &pmon_vec = get_column<short>("pickup_month");
    rvector<short> * rv_pmon = (rvector<short> *) &pmon_vec;
    size_t s = pmon_vec.size();
    size_t c = pmon_vec.capacity();
    rring_outer_loop(rpickmonth, short, c) {
    rring_inner_preloop(rpickmonth, short);
    rring_sync_writeonly(rpickmonth);
    rring_inner_loop(rpickmonth, j) {
        size_t nth = _t_rpickmonth * _bn_rpickmonth + j;
        _inner_rpickmonth[j] = rv_pmon->head[nth];
    }
    rring_inner_wb(rpickmonth);
    }
    rring_cleanup_writeonly(rpickmonth);
    pmon_vec.clear();
    pmon_vec.shrink_to_fit();
    rv_pmon->head = (short *) (4096ULL + (23ULL << 30));
    rv_pmon->end  = rv_pmon->head + s;
    rv_pmon->tail = rv_pmon->head + c;
    printf("pickup month\n");
}

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
    analyze_trip_durations_of_day<short>("pickup_day");
    times[7] = std::chrono::steady_clock::now();
    analyze_trip_durations_of_month<short>("pickup_month");
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

