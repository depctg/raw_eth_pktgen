#include <vector>
#include <unordered_set>
#include <iostream>
#include <chrono>
#include "internal.h"
#include "simple_time.hpp"


template <typename K, typename T, typename F>
void get_data_by_sel (const char *name, F &sel_func, 
                      std::vector<T> &newvec) {
    auto &indices_ = get_index();

    const std::vector<K>    &vec = get_column<K>(name);
    const size_type         idx_s = indices_.size();
    const size_type         col_s = vec.size();

    // TODO: measure col_indices size
    // make sure this do not trigger realloc
    // and consider remotelize
    std::vector<int> &psg = get_column<int>("passenger_count");
    newvec.reserve(col_s);
    for (size_type i = 0; i < col_s; ++i)
        if (sel_func (indices_[i], vec[i])) {
            newvec.push_back(psg[i]);
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

    // Target column
    // std::vector<int> new_psg_count;
    // sel_copy(newvec, psg, col_indices, idx_s);
    return;
}

void print_passage_counts_by_vendor_id(int vendor_id)
{
    printf("print_passage_counts_by_vendor_id(vendor_id), vendor_id = %d\n", vendor_id);

    auto sel_vendor_functor = [&](const uint64_t&, const int& vid) -> bool {
        return vid == vendor_id;
    };

    std::vector<int> passage_count_vec;
    get_data_by_sel<int>("VendorID", sel_vendor_functor, passage_count_vec);

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

int main()
{
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
