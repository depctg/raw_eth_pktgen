#ifndef _VEC_H_
#define _VEC_H_

#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

#include "cache.h"

static uint64_t offset_vec = 0;
inline static uint64_t get_new_vec_offset(uint64_t s) { return (id_vec += s); }

static CacheTable *ctable = 0;
void RCacheVector_init_cache_table(CacheTable *c) {
    if (!ctable) ctable = c;
}

template <typename T>
class RCacheVector {
 private:
    using Index_t = uint64_t;
    using Pattern_t = int64_t;

    template <typename T>
    friend class RCacheVector;

    /*
    class Iterator {
    private:
        RCacheVector *dataframe_vec_;
        friend class RCacheVector;

        Iterator(RCacheVector<T> *_vec, );
        uint64_t get_idx() const;

    public:
        using difference_type = int64_t;
        using pointer = const T *;
        using reference = const T &;
        using iterator_category = std::random_access_iterator_tag;

        Iterator &operator++();
        Iterator operator++(int);
        Iterator &operator+=(difference_type dis);
        Iterator operator+(difference_type dis) const;
        Iterator &operator--();
        Iterator operator--(int);
        difference_type operator-(const Iterator &other) const;
        bool operator==(const Iterator &other) const;
        bool operator!=(const Iterator &other) const;
        bool operator<(const Iterator &other) const;
        bool operator<=(const Iterator &other) const;
        bool operator>(const Iterator &other) const;
        bool operator>=(const Iterator &other) const;
        T operator*() const;
    };
    */

    const uint32_t _data_size = sizeof(T);

    uint32_t _chunk_num_entries;  // #data per cache line
    uint64_t _offset;             // starting offset of the data in remote

    uint64_t _capacity;
    uint64_t _size = 0;

    std::pair<Index_t, Index_t> which_chunk(Index_t i) {
        return std::make_pair(i / _chunk_num_entries, i % _chunk_num_entries);
    }

    uint64_t where_offset(uint64_t i) {
        auto [chunk_idx, chunk_offset] = which_chunk(i);
        return chunk_idx * ctable->cache_line_size + chunk_offset*_data_size;
    }

   public:
    explicit RCacheVector(uint64_t atleast_cap = 0) {
      if (!ctable) throw "RCacheVector_init_cache_table first";

      _chunk_num_entries = ctable->cache_line_size / _data_size;
      if (!_chunk_num_entries) throw "cache line size too small";
      auto num_entries_to_alloc = (atleast_cap) ? (atleast_cap-1) / _chunk_num_entries + 1 : 0;
      _capacity = num_chunks_to_alloc * _chunk_num_entries;
      _offset =
          get_new_vec_offset(num_chunks_to_alloc * ctable->cache_line_size);
    }
    //   RCacheVector &operator=(const RCacheVector &other);
    //   RCacheVector(RCacheVector &&other);
    //   RCacheVector &operator=(RCacheVector &&other);
    ~RCacheVector() {
        // TODO: release old offset
    }

    uint64_t capacity() const { return _capacity; }
    template <typename U>
    void push_back(U &&u) {
        static_assert(std::is_same<std::decay_t<U>, std::decay_t<T>>::value, "U not same as T");

        if (unlikely(_capacity == _size + 1)) {
            resize(_capacity*2+1);
        }

        cache_write_n(ctable, _offset + where_offset(_size), &u, _data_size);

        _size += 1;

        // TODO: prefech
        // TODO: batch write to remote
    }
    void pop_back() {if(_size) _size-=1;}
    void reserve(uint64_t count) {
        if (count <= _capacity) return;
        resize(count);
    }
    void resize(uint64_t atleast_cap) {
        if (atleast_cap <= _capacity) return;
        uint64_t old_offset = _offset;

        auto num_entries_to_alloc = (atleast_cap) ? (atleast_cap-1) / _chunk_num_entries + 1 : 0;
        _capacity = num_chunks_to_alloc * _chunk_num_entries;
        _offset = get_new_vec_offset(num_chunks_to_alloc * ctable->cache_line_size);

        for (int i = 0; i < _size; ++i) {
            // TODO: better copy
            cache_write_n(ctable, _offset+where_offset(i), reinterpret_cast<*T>(cache_access(ctable, old_offset+where_offset(i))), _data_size);
        }

        // TODO: release old offset
    }

    T nth_element(uint64_t i) {
        return *reinterpret_cast<*T>(cache_access(ctable, _offset+where_offset(i)));
    }
    T front() { return nth_element(0); }
    const T back() { return nth_element(_size-1); }
    T at(uint64_t i) { return nth_element(i); }

    void update(uint64_t i, T&& t) {
        if (i >= _size) throw "update out of index";

        cache_write_n(ctable, _offset+where_offset(i), &t, _data_size);
    }

    /*
    Iterator begin();
    Iterator end();
    const Iterator cbegin() const;
    const Iterator cend() const;
    */

    // void disable_prefetch();
    // void enable_prefetch();
    // void static_prefetch(Index_t start, Index_t step, uint32_t num);
};

#endif
