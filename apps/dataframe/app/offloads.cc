#include "DataFrame/DataFrame.h"


extern "C" {
    void * deref_disagg_vaddr(uint64_t);
    extern uint64_t local_remote_delimiter;
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
static inline void remote_2_local_trans(__v<T> *v) {
    v->p1 = (T *)((uint64_t)(v->p1) + local_remote_delimiter);
    v->p2 = (T *)((uint64_t)(v->p2) + local_remote_delimiter);
    v->p3 = (T *)((uint64_t)(v->p3) + local_remote_delimiter);
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


static inline void step2_get_data_by_sel (std::vector<size_t> &indices_,
                            std::vector<int>    &vec,   
                            int                 filter_vid,
                            std::vector<int>    &passanger,
                            std::vector<int>    &newvec) {

    const size_t idx_s = indices_.size();
    const size_t col_s = vec.size();
    std::vector<size_t>  col_indices;
    newvec.reserve(col_s);

    // TODO: measure col_indices size
    // make sure this do not trigger realloc
    // and consider remotelize
    col_indices.reserve(idx_s);
    for (size_t i = 0; i < col_s; ++i)
        if (sel_vendor_functor (filter_vid, vec[i])) {
            (void) indices_[i];
            col_indices.push_back(i);
            newvec.push_back(passanger[i]);
        }

    // Target column
    return;
}


// size_t step2_get_data_by_sel(std::vector<size_t> &indices, 
//                              std::vector<int>      &filter_by,
//                              int                 vendor_id,
//                              std::vector<int>      &target);

static std::vector<int> step2_newvec;
void __step2_get_data_by_sel(void* arg, void* ret) {
    __v<size_t>  *indices   = (__v<size_t> *)arg;
    __v<int>     *filter_by = (__v<int> *) (indices + 1);
    int          *vendor_id = (int *) (filter_by + 1);
    __v<int>     *target    = (__v<int> *) (vendor_id + 1);

    trans_vec(indices);
    trans_vec(filter_by);
    trans_vec(target);

    step2_get_data_by_sel(
        * (std::vector<size_t> *) indices,
        * (std::vector<int> *) filter_by,
        * vendor_id,
        * (std::vector<int> *) target,
        step2_newvec
    );

    __v<int> *retvec = (__v<int> *) (&step2_newvec);
    remote_2_local_trans(retvec);
    * (__v<int> *) ret = *retvec; 
    // memcpy(ret, &r, sizeof(size_t)); 
}