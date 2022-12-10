#include <vector>
#include <chrono>
#include "internal.h"
#include "rvector.h"
#include "simple_time.hpp"
#include <cassert>
#include "cache.h"
#include "common.h"
#include "offload.h"

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

template <typename K, typename T, typename F>
void get_data_by_sel (const char *name, F &sel_func, std::vector<T> &newvec) {
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

    const std::vector<int> &target_col = get_column<int>("VendorID");
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
    // sel_load(col_indices, get_column<int>("VendorID"), newvec);
    return;

    // return (df);
}

static std::vector<int> sel_whatever;
static std::vector<int> sel_vendor_ids;

void calculate_distribution_store_and_fwd_flag()
{
    printf("calculate_distribution_store_and_fwd_flag()\n");

    auto sel_N_saff_functor = [&](const uint64_t&, const char& saff) -> bool {
        return saff == 'N';
    };

    // get_data_by_sel<char>("store_and_fwd_flag", sel_N_saff_functor, sel_whatever);
    std::vector<size_t> &idx_ = get_index();
    size_t N = idx_.size();
    rvector<size_t> *indices = (rvector<size_t> *) &idx_;
    rvector<char> *flags = (rvector<char> *) &get_column<char>("store_and_fwd_flag");
    rvector<int> *vids = (rvector<int> *) &get_column<int>("VendorID");

    rvector<int> *vid_newvec;
    sel_whatever.reserve(N);
    remotelize(sel_whatever, false);
    vid_newvec = (rvector<int> *) &sel_whatever;

    size_t arg_size = 0;
    rvector<size_t> *gep_indices = (rvector<size_t> *) offload_arg_buf;
    *gep_indices = *indices;
    arg_size += sizeof(rvector<size_t>);

    rvector<char> *gep_flags = (rvector<char> *) (gep_indices + 1);
    *gep_flags = *flags;
    arg_size += sizeof(rvector<char>);

    char *gep_saff = (char *) (gep_flags + 1);
    *gep_saff = 'N';
    arg_size += sizeof(char);

    rvector<int> *gep_vids = (rvector<int> *) (gep_saff + 1);
    *gep_vids = *vids;
    arg_size += sizeof(rvector<int>);
    
    rvector<int> *gep_vid_newvec = (rvector<int> *) (gep_vids + 1);
    *gep_vid_newvec = *vid_newvec;
    arg_size += sizeof(rvector<int>);

    size_t s = * (size_t *) call_offloaded_service(6, arg_size, sizeof(size_t));

    printf("%f\n", static_cast<double>(s) / get_index().size());

    auto sel_Y_saff_functor = [&](const uint64_t&, const char& saff) -> bool {
        return saff == 'Y';
    };

    get_data_by_sel<char>("store_and_fwd_flag", sel_Y_saff_functor, sel_vendor_ids);

    rvector<int> *Y_vid_newvec;
    sel_vendor_ids.reserve(N);
    remotelize(sel_vendor_ids, false);
    Y_vid_newvec = (rvector<int> *) &sel_vendor_ids;

    arg_size = 0;
    *gep_indices = *indices;
    arg_size += sizeof(rvector<size_t>);

    *gep_flags = *flags;
    arg_size += sizeof(rvector<char>);

    *gep_saff = 'Y';
    arg_size += sizeof(char);

    *gep_vids = *vids;
    arg_size += sizeof(rvector<int>);
    
    *gep_vid_newvec = *Y_vid_newvec;
    arg_size += sizeof(rvector<int>);

    s = * (size_t *) call_offloaded_service(6, arg_size, sizeof(size_t));

    std::vector<int> unique_vendor_id_vec;
    // size_t s = sel_vendor_ids.size();

    // for (size_t i = 0; i < s; i++)  {
    //     int e = sel_vendor_ids[i];
    //     if (step4_firstTime(e))
    //         unique_vendor_id_vec.push_back(e);
    // }

    rring_init(yvid, int, (2 << 20), 32, (size_t) ((char*)rbuf + (8 << 20)), remoteAddr(Y_vid_newvec->head));

    rring_outer_loop(yvid, int, N) {

        rring_prefetch(yvid, 8);
        rring_inner_preloop(yvid, int);
        rring_sync(yvid);

        rring_inner_loop(yvid, j) {
            int e = _inner_yvid[j];
            if (step4_firstTime(e))
                unique_vendor_id_vec.push_back(e);
        }
    }

    printf("{");
    for (auto& vector_id : unique_vendor_id_vec) {
        printf("%d, ", vector_id);
    }
    printf("}\n\n");
}

int main()
{
    init_client();
    cache_init();
    channel_init();
    std::chrono::time_point<std::chrono::steady_clock> times[10];
    void * df  = load_data();
    times[0] = std::chrono::steady_clock::now();
    calculate_distribution_store_and_fwd_flag();
    times[1] = std::chrono::steady_clock::now();
    printf("Step 4: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
}
