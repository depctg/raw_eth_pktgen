#include <vector>
#include <chrono>
#include "internal.h"
#include "rvector.h"
#include "simple_time.hpp"
#include <cassert>
#include "cache.h"
#include "common.h"
#include <cmath>
#include "offload.h"

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


template<typename T> void sel_load
(std::vector<size_t>& indices, const std::vector<T> &vec, std::vector<T>& new_col)  {
    const size_type vec_size = vec.size();

    new_col.reserve(std::min(indices.size(), vec_size));
    size_t s = indices.size();
    for (size_t i = 0; i < s; i++)  {
        size_t citer = indices[i];
        const size_type index =
            citer >= 0 ? citer : static_cast<size_t>(s) + citer;

        if (index < vec_size)
            new_col.push_back(vec[index]);
        else
            break;
    }
    return;
}

// template: sel_by K, retrieve vec T, sel_fn F
template <typename K, typename T, typename F>
void get_data_by_sel (const char *name, F &sel_func, 
                      const char *target_name, std::vector<T> &newvec) {
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

    const std::vector<T> &target_col = get_column<T>(target_name);

    uint64_t idx_base = remoteAddr((void *)&indices_[0]);
    uint64_t vec_base = remoteAddr((void *)&vec[0]);
    uint64_t tgt_base = remoteAddr((void *)&target_col[0]);

    rring_init(rids, size_t, (2 << 20), 4, (size_t) ((char*)rbuf + (8<<20)), idx_base);
    rring_init(r_filter_by, K, (2 << 20), 32, (size_t) ((char*)rbuf + (16<<20)), vec_base);
    rring_init(r_get_from, T, (2 << 20), 32, (size_t) ((char*)rbuf + (80<<20)), tgt_base);

    rring_outer_loop_with(r_filter_by, col_s);
    rring_outer_loop_with(r_get_from, col_s);
    rring_outer_loop(rids, size_t, col_s) {
        rring_prefetch(rids, 2);
        rring_prefetch(r_filter_by, 8);
        rring_prefetch(r_get_from, 8);

        rring_inner_preloop(rids, size_t);
        rring_inner_preloop(r_filter_by, K);
        rring_inner_preloop(r_get_from, T);

        rring_sync(r_get_from);

        rring_inner_loop(rids, j) {
            size_t index = _inner_rids[j];
            K fby = _inner_r_filter_by[j];
            T rget = _inner_r_get_from[j];
            // printf("%lu %d %d\n", index, fby, rget);
            if (sel_func (index, fby)) {
                // nth = _t_<name> * _bn_<name> + j;
                // size_t nth = _t_rids * _bn_rids + j;
                // col_indices.push_back(nth);
                newvec.push_back(rget);
            }
        }
        rring_outer_loop_with_post(r_get_from);
        rring_outer_loop_with_post(r_filter_by);
    }

    // DataFrame       df;

    // df.load_index(std::move(new_index));

    // Might need to restore this part. 
    // This do a all to all copy

    // for (auto col_citer : column_tb_)  {
    //     sel_load_functor_<size_type, Ts ...>    functor (
    //         col_citer.first.c_str(),
    //         col_indices,
    //         idx_s,
    //         df);

    //     data_[col_citer.second].change(functor);
    // }

    // TODO: get indices only
    // sel_load(col_indices, get_column<T>(target_name), newvec);
    return;

    // return (df);
}

static std::vector<double> haversine_distance_vec;
static std::vector<int> sel_5_whatever;
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
    haversine_distance_vec.reserve(N);
    remotelize(haversine_distance_vec, false);

    rvector<double> *plon = (rvector<double> *) &pickup_longitude_vec;
    rvector<double> *plat = (rvector<double> *) &pickup_latitude_vec;
    rvector<double> *dlon = (rvector<double> *) &dropoff_longitude_vec;
    rvector<double> *dlat = (rvector<double> *) &dropoff_latitude_vec;
    rvector<double> *haversine_d = (rvector<double> *) &haversine_distance_vec;

    size_t arg_size = 0;
    rvector<double> *gep_plon = (rvector<double> *) offload_arg_buf;
    *gep_plon = *plon;
    arg_size += sizeof(rvector<double>);

    rvector<double> *gep_plat = (rvector<double> *) (gep_plon + 1);
    *gep_plat = *plat;
    arg_size += sizeof(rvector<double>);

    rvector<double> *gep_dlon = (rvector<double> *) (gep_plat + 1);
    *gep_dlon = *dlon;
    arg_size += sizeof(rvector<double>);

    rvector<double> *gep_dlat = (rvector<double> *) (gep_dlon + 1);
    *gep_dlat = *dlat;
    arg_size += sizeof(rvector<double>);

    rvector<double> *gep_haversine_d = (rvector<double> *) (gep_dlat + 1);
    *gep_haversine_d = *haversine_d;
    arg_size += sizeof(rvector<double>);

    call_offloaded_service(7, arg_size, 0);

    // for (uint64_t i = 0; i < N; i++) {
    //     haversine_distance_vec.push_back(haversine(pickup_latitude_vec[i], pickup_longitude_vec[i],
    //                                                dropoff_latitude_vec[i],
    //                                                dropoff_longitude_vec[i]));
    // }

    // load_column("haversine_distance", std::move(haversine_distance_vec));
                    
    // Can jump this part
    auto sel_functor = [&](const uint64_t&, const double& dist) -> bool { return dist > 100; };


    sel_5_whatever.reserve(N);
    remotelize(sel_5_whatever, false);

    rvector<size_t> *indices = (rvector<size_t> *) &get_index();
    haversine_d = (rvector<double> *) &haversine_distance_vec;
    rvector<int> *rvids = (rvector<int> *) &get_column<int>("VendorID");
    rvector<int> *vid_newvec = (rvector<int> *) &sel_5_whatever;

    arg_size = 0;
    rvector<size_t> *gep_idx = (rvector<size_t> *) offload_arg_buf;
    *gep_idx = *indices;
    arg_size += sizeof(rvector<size_t>);

    gep_haversine_d = (rvector<double> *) (gep_idx + 1);
    *gep_haversine_d = *haversine_d;
    arg_size += sizeof(rvector<double>);

    double *gep_filter_d = (double *) (gep_haversine_d + 1);
    *gep_filter_d = 100;
    arg_size += sizeof(double);

    rvector<int> *gep_rvid = (rvector<int> *) (gep_filter_d + 1);
    *gep_rvid = *rvids;
    arg_size += sizeof(rvector<int>);

    rvector<int> *gep_vid_newvec = (rvector<int> *) (gep_rvid + 1);
    *gep_vid_newvec = *vid_newvec;
    arg_size += sizeof(rvector<int>);

    size_t s = * (size_t *) call_offloaded_service(8, arg_size, sizeof(size_t));

    printf("Number of rows that have haversine_distance > 100 KM = %lu\n", s);
    // get_data_by_sel<double>("haversine_distance", sel_functor, "VendorID", sel_whatever);
    // printf("Number of rows that have haversine_distance > 100 KM = %lu\n", sel_whatever.size());
    printf("\n");
}

int main()
{
    init_client();
    cache_init();
    channel_init();
    std::chrono::time_point<std::chrono::steady_clock> times[10];
    void * df  = load_data();
    times[0] = std::chrono::steady_clock::now();
    calculate_haversine_distance_column();
    times[1] = std::chrono::steady_clock::now();
    printf("Step 5: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
}
