#include <vector>
#include <chrono>
#include "internal.h"
#include "rvector.h"
#include "simple_time.hpp"
#include <cassert>
#include "cache.h"
#include "common.h"
#include <cmath>

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

    std::vector<double> haversine_distance_vec;
    haversine_distance_vec.reserve(pickup_latitude_vec.size());
    size_t N = pickup_latitude_vec.size();

    // for (uint64_t i = 0; i < N; i++) {
    //     haversine_distance_vec.push_back(haversine(pickup_latitude_vec[i], pickup_longitude_vec[i],
    //                                                dropoff_latitude_vec[i],
    //                                                dropoff_longitude_vec[i]));
    // }

    uint64_t plon_base = remoteAddr((void *)&pickup_longitude_vec[0]);
    uint64_t plat_base = remoteAddr((void *)&pickup_latitude_vec[0]);
    uint64_t dlon_base = remoteAddr((void *)&dropoff_longitude_vec[0]); 
    uint64_t dlat_base = remoteAddr((void *)&dropoff_latitude_vec[0]); 

    rring_init(rplon, uint64_t, (2 << 20), 8, (size_t) ((char*)rbuf + (8<<20)), plon_base);
    rring_init(rplat, uint64_t, (2 << 20), 8, (size_t) ((char*)rbuf + (24<<20)), plat_base);
    rring_init(rdlon, uint64_t, (2 << 20), 8, (size_t) ((char*)rbuf + (40<<20)), dlon_base);
    rring_init(rdlat, uint64_t, (2 << 20), 8, (size_t) ((char*)rbuf + (56<<20)), dlat_base);

    rring_outer_loop_with(rplon, N);
    rring_outer_loop_with(rplat,  N);
    rring_outer_loop_with(rdlon, N);
    rring_outer_loop(rdlat, uint64_t, N) {
        rring_prefetch(rplon, 4);
        rring_prefetch(rplat, 4);
        rring_prefetch(rdlon, 4);
        rring_prefetch(rdlat, 4);

        rring_inner_preloop(rplon, uint64_t);
        rring_inner_preloop(rplat, uint64_t);
        rring_inner_preloop(rdlon, uint64_t);
        rring_inner_preloop(rdlat, uint64_t);

        rring_sync(rdlat);

        rring_inner_loop(rdlat, j) {
            double rel = haversine(
                _inner_rplat[j], _inner_rplon[j],
                _inner_rdlat[j], _inner_rdlon[j]
            );
            haversine_distance_vec.push_back(rel);
        }
        rring_outer_loop_with_post(rplon);
        rring_outer_loop_with_post(rplat);
        rring_outer_loop_with_post(rdlon);
    }

    load_column("haversine_distance", std::move(haversine_distance_vec));
                    
    // Can jump this part
    auto sel_functor = [&](const uint64_t&, const double& dist) -> bool { return dist > 100; };

    std::vector<int> sel_whatever;
    get_data_by_sel<double>("haversine_distance", sel_functor, "VendorID", sel_whatever);
    printf("Number of rows that have haversine_distance > 100 KM = %lu\n", sel_whatever.size());
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
