#include "DataFrame/DataFrame.h"


extern "C" {
    void * deref_disagg_vaddr(uint64_t);
    // c functions, without any template parameter
    void __step1_get_col_unique_values(void*, void*);
    void __step2_get_data_by_sel(void*, void*);
}

// templated, c++ imples
template<typename T>
struct __v {
    T *p1, *p2, *p3;
};

template<typename T>
static inline void trans_vec(__v<T> *v) {
    v->p1 = (T *)deref_disagg_vaddr((uint64_t)(v->p1));
    v->p2 = (T *)deref_disagg_vaddr((uint64_t)(v->p2));
    v->p3 = (T *)deref_disagg_vaddr((uint64_t)(v->p3));
}

template<typename T>
size_t get_col_unique_values(const std::vector<T> & vec) {
    size_t N = vec.size();

    auto                    hash_func =
        [](const T& v) -> std::size_t  {
            return(std::hash<T>{}(v));
    };
    auto                    equal_func =
        [](const T& lhs,
           const T& rhs) -> bool  {
            return(lhs == rhs);
    };

    std::unordered_set<T,
        decltype(hash_func),
        decltype(equal_func)>   table(vec.size(), hash_func, equal_func);

    std::vector<T> result;
    for (size_t i = 0; i < N; i++)  {
        T e = vec[i];
        const auto insert_ret = table.emplace(e);
        if (insert_ret.second)
            result.push_back(e);
    }

    return(table.size());
}

void __step1_get_col_unique_values(void* arg, void* ret) {
    __v<int> *v = (__v<int> *)arg;
    trans_vec(v);
    size_t r = get_col_unique_values(* (std::vector<int> *)v);
    memcpy(ret, &r, sizeof(size_t)); 
}

static inline bool sel_vendor_functor(const int vvid, const int vid) {
    return vvid == vid;
}

template <typename K, typename T>
size_t step2_get_data_by_sel(std::vector<size_t> &indices, 
                             std::vector<K>      &filter_by,
                             int                 vendor_id,
                             std::vector<T>      &target) {
    const size_t         idx_s = indices.size();
    const size_t         col_s = filter_by.size();

    std::vector<size_t> col_indices;
    for (size_t i = 0; i < col_s; ++ i) {
        if (sel_vendor_functor(filter_by[i], vendor_id))
            col_indices.push_back(i);
    }

    std::vector<T> new_col;
    new_col.reserve(col_indices.size());

    const size_t s = col_indices.size();
    const size_t vec_size = target.size();

    for (size_t i = 0; i < s; ++ i) {
        size_t citer = col_indices[i];
        const size_type index =
            citer >= 0 ? citer : static_cast<size_t>(s) + citer;
        if (index < vec_size)
            new_col.push_back(target[index]);
        else
            break;
    }

    return new_col.size();
}

// size_t step2_get_data_by_sel(std::vector<size_t> &indices, 
//                              std::vector<int>      &filter_by,
//                              int                 vendor_id,
//                              std::vector<int>      &target);

void __step2_get_data_by_sel(void* arg, void* ret) {
    __v<size_t>  *indices   = (__v<size_t> *)arg;
    __v<int>     *filter_by = (__v<int> *) (indices + 1);
    int          *vendor_id = (int *) (filter_by + 1);
    __v<int>     *target    = (__v<int> *) (vendor_id + 1);

    trans_vec(v);
    size_t r = get_col_unique_values(* (std::vector<int> *)v);
    memcpy(ret, &r, sizeof(size_t)); 
}