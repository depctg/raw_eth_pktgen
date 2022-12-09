#include "remote_pool.h"
#include <vector>
#include <cstring>
#include <unordered_set>

struct __v {
    size_t *p1, *p2, *p3;
};

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

extern "C" void __step1_num_unique(void *arg, void *ret) {
    __v *v = (__v *)arg;
    v->p1 = (size_t *)deref_disagg_vaddr((uint64_t)(v->p1));
    v->p2 = (size_t *)deref_disagg_vaddr((uint64_t)(v->p2));
    v->p3 = (size_t *)deref_disagg_vaddr((uint64_t)(v->p3));
    size_t r = get_col_unique_values(* (std::vector<int> *)v);
    memcpy(ret, &r, sizeof(size_t)); 
}