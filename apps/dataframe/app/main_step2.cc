#include <vector>
#include <iostream>
#include <chrono>
#include "internal.h"
#include "rvector.h"
#include "cache.h"
#include "common.h"

template<typename T> void sel_load
(std::vector<size_t>& indices, const std::vector<T> &vec, std::vector<T>& new_col)  {
    const size_type vec_size = vec.size();

    new_col.reserve(std::min(indices.size(), vec_size));

    size_t s = new_col.size();
    for (size_t i = 0; i < s; i++)  {
        size_t citer = indices[i];
        const size_type index =
            citer >= 0 ? citer : static_cast<size_t>(s) + citer;

        if (index < vec_size) {
            new_col.push_back(vec[index]);
        }
        else
            break;
    }

    return;
}

template <typename T, typename F> void
get_data_by_sel (const char *name, F &sel_func, std::vector<T>& newvec) {
    auto &indices_ = get_index();

    const std::vector<T>    &vec = get_column<T>(name);
    const size_type         idx_s = indices_.size();
    const size_type         col_s = vec.size();
    // std::vector<size_type>  col_indices;

    // TODO: measure col_indices size
    // make sure this do not trigger realloc
    // and consider remotelize
    // col_indices.reserve(idx_s);
    newvec.reserve(col_s);


    // for (size_type i = 0; i < col_s; ++i)
    //     if (sel_func (indices_[i], vec[i])) {
    //         col_indices.push_back(i);
    //     }
    const std::vector<int> &target_col = get_column<int>("passenger_count");

    uint64_t idx_base = remoteAddr((void *)&indices_[0]);
    uint64_t vec_base = remoteAddr((void *)&vec[0]);
    uint64_t tgt_base = remoteAddr((void *)&target_col[0]);

    // printf("%lu\n", remoteAddr(p));
    rring_init(rids, size_t, (2 << 20), 4, (size_t) ((char*)rbuf + (8<<20)), idx_base);
    rring_init(r_filter_by, int, (2 << 20), 32, (size_t) ((char*)rbuf + (16<<20)), vec_base);
    rring_init(r_get_from, int, (2 << 20), 32, (size_t) ((char*)rbuf + (80<<20)), tgt_base);

    rring_outer_loop_with(r_filter_by, col_s);
    rring_outer_loop_with(r_get_from, col_s);
    rring_outer_loop(rids, size_t, col_s) {
        rring_prefetch(rids, 2);
        rring_prefetch(r_filter_by, 4);
        rring_prefetch(r_get_from, 4);

        rring_inner_preloop(rids, size_t);
        rring_inner_preloop(r_filter_by, int);
        rring_inner_preloop(r_get_from, int);

        rring_sync(r_get_from);

        rring_inner_loop(rids, j) {
            size_t index = _inner_rids[j];
            int fby = _inner_r_filter_by[j];
            int rget = _inner_r_get_from[j];
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
    // IndexVecType    new_index;

    // new_index.reserve(col_indices.size());
    // for (const auto citer: col_indices)
    //     new_index.push_back(indices_[citer]);
    // df.load_index(std::move(new_index));

    // for (auto col_citer : column_tb_)  {
    //     sel_load_functor_<size_type, Ts ...>    functor (
    //         col_citer.first.c_str(),
    //         col_indices,
    //         idx_s,
    //         df);

    //     data_[col_citer.second].change(functor);
    // }

    // TODO: populate entire dataframe
    // std::vector<int> uselessVenderId;
    // sel_load(col_indices, get_column<int>("VendorID"), uselessVenderId);

    // Target column
    // sel_load(col_indices, get_column<int>("passenger_count"), newvec);
    // Fo full copy
    return;

    // return (df);
}

void print_passage_counts_by_vendor_id(int vendor_id)
{
    printf("print_passage_counts_by_vendor_id(vendor_id), vendor_id = %d\n", vendor_id);

    auto sel_vendor_functor = [&](const uint64_t&, const int& vid) -> bool {
        return vid == vendor_id;
    };

    std::vector<int> passage_count_vec;
    get_data_by_sel("VendorID", sel_vendor_functor, passage_count_vec);

    size_t s = passage_count_vec.size();
    printf("newvec size %lu\n", s);
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

int main()
{
    init_client();
    cache_init();
    channel_init();
    std::chrono::time_point<std::chrono::steady_clock> times[10];
    void * df  = load_data();
    times[0] = std::chrono::steady_clock::now();
    print_passage_counts_by_vendor_id(1);
    times[1] = std::chrono::steady_clock::now();
    print_passage_counts_by_vendor_id(2);
    times[2] = std::chrono::steady_clock::now();
    printf("Step 2-1: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
    printf("Step 2-2: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[2] - times[1])
        .count());
}
