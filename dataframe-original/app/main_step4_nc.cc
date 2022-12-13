#include <vector>
#include <unordered_set>
#include <chrono>
#include "internal.h"
#include "rvector.h"


template <typename K, typename T, typename F>
void get_data_by_sel (const char *name, F &sel_func, 
                      std::vector<T> &newvec) {
    auto &indices_ = get_index();

    const std::vector<K>    &vec = get_column<K>(name);
    const size_type         idx_s = indices_.size();
    const size_type         col_s = vec.size();

    std::vector<int> &vids = get_column<int>("VendorID");
    // TODO: measure col_indices size
    // make sure this do not trigger realloc
    // and consider remotelize
    newvec.reserve(col_s);
    for (size_type i = 0; i < col_s; ++i)
        if (sel_func (indices_[i], vec[i])) {
            newvec.push_back(vids[i]);
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
    // sel_copy(newvec, vids, col_indices, idx_s);
    return;
}

void calculate_distribution_store_and_fwd_flag()
{
    printf("calculate_distribution_store_and_fwd_flag()\n");

    auto sel_N_saff_functor = [&](const uint64_t&, const char& saff) -> bool {
        return saff == 'N';
    };

    std::vector<int> sel_whatever;
    get_data_by_sel<char>("store_and_fwd_flag", sel_N_saff_functor, sel_whatever);

    printf("%f\n", static_cast<double>(sel_whatever.size()) / get_index().size());

    auto sel_Y_saff_functor = [&](const uint64_t&, const char& saff) -> bool {
        return saff == 'Y';
    };

    std::vector<int> sel_vendor_ids;
    get_data_by_sel<char>("store_and_fwd_flag", sel_Y_saff_functor, sel_vendor_ids);

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

int main()
{
    std::chrono::time_point<std::chrono::steady_clock> times[10];
    void * df  = load_data();
    times[0] = std::chrono::steady_clock::now();
    calculate_distribution_store_and_fwd_flag();
    times[1] = std::chrono::steady_clock::now();
    printf("Step 4: %ld us\n", 
        std::chrono::duration_cast<std::chrono::microseconds>(times[1] - times[0])
        .count());
}
