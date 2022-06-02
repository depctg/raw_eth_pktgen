#ifndef _VEC_H_
#define _VEC_H_

#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>
#include <algorithm>

#include <stdexcept>

#include "cache.h"
#include "common.h"

static uint64_t offset_vec = 0;
inline static uint64_t get_new_vec_offset(uint64_t s) { return (offset_vec += s); }

static bool ctable_inited = false;
static cache_t ctable;
static uint64_t cline_size = 0;
inline void RCacheVector_init_cache_table(cache_t c, uint64_t l) {
    if (!ctable_inited) {
        ctable_inited = true;
        ctable = c;
        cline_size = l;
    }
}

template <typename T>
class RCacheVector {
 private:
    using Index_t = uint64_t;
    using Pattern_t = int64_t;

    template <typename U>
    friend class RCacheVector;

    class CIterator {
    private:
        RCacheVector<T> *_vec;
        Index_t _i;
        friend class RCacheVector;

        CIterator(RCacheVector<T> *vec, Index_t i) : _vec(vec), _i(i) {}

    public:
        using difference_type = int64_t;
        using iterator_category = std::random_access_iterator_tag;

        CIterator &operator++() {_i += 1; return *this;}
        CIterator operator++(int) {CIterator it = CIterator(_vec, _i); _i += 1; return it;}
        CIterator &operator+=(difference_type dis) {_i += dis; return *this;};
        CIterator operator+(difference_type dis) const {return CIterator(_vec, _i+dis);}
        CIterator &operator--() {_i-=1; return *this;}
        CIterator operator--(int) {CIterator it = CIterator(_vec, _i); _i -= 1; return it;}
        difference_type operator-(const CIterator &other) const {return _i - other._i;}
        bool operator==(const CIterator &other) const {return (_vec == other._vec && _i == other._i);}
        bool operator!=(const CIterator &other) const {return !(*this == other);}
        bool operator<(const CIterator &other) const {return (_vec == other._vec && _i < other._i);}
        bool operator<=(const CIterator &other) const {return (_vec == other._vec && _i <= other._i);}
        bool operator>(const CIterator &other) const {return (_vec == other._vec && _i > other._i);}
        bool operator>=(const CIterator &other) const {return (_vec == other._vec && _i >= other._i);}
        T operator*() const {return _vec->nth_element(_i);}
    };

    class BatchIterator {
    private:
        RCacheVector<T> *_vec;
        Index_t _i;
        uint64_t _n_prefetch_lines;
        uint64_t _n_prefetch_elems;

        std::vector<std::pair<cache_token_t, bool>> _tokens;
        int _i_p;
        // Index_t _i_next;

        BatchIterator(RCacheVector<T> *vec, Index_t i, uint64_t n_prefetch_lines) : _vec(vec), _i(i), _n_prefetch_lines(n_prefetch_lines) {
            // no request for end()
            if (_i != _vec->_size) {
                if (!_n_prefetch_lines) _n_prefetch_lines = 1;
                _n_prefetch_elems = _n_prefetch_lines * _vec->_chunk_num_entries;
                _tokens.resize(_n_prefetch_lines);
                batch_prefetch();
            }
        }

        void batch_prefetch() {
            for (int i_p = 0; i_p < _n_prefetch_lines; ++i_p) {
                _tokens[i_p] = std::make_pair(cache_request(ctable, _vec->where_cache(_vec->_offset, _i + i_p * _vec->_chunk_num_entries).first), false);
            }
            _i_p = 0;
            // _i_next = ((_i + n_prefetch_lines * _vec->_chunk_num_entries - 1) / _vec->_chunk_num_entries + 1) * _vec->_chunk_num_entries;
        }

    public:
        using difference_type = int64_t;
        using iterator_category = std::forward_iterator_tag;

        BatchIterator &operator++() {
            if (_i >= _vec->_size - 1) {
                _i = _vec->_size;
                return *this;
            }
            _i += 1;
            if (_i % _vec->_chunk_num_entries) {
                _i_p += 1;
                if (_i_p == _n_prefetch_lines) {
                    batch_prefetch();
                }
            }
            return *this;
        }
        T operator*() {
            if (_i == _vec->_size) throw std::runtime_error("access out of index");
            if (!_tokens[_i_p].second) {
                cache_await(_tokens[_i_p].first);
                _tokens[_i_p].second = true;
            }
            return *(reinterpret_cast<T*>(cache_access(_tokens[_i_p].first))+(_i % _vec->_chunk_num_entries));
        }

        BatchIterator operator++(int) {throw std::runtime_error("not supported");}
        bool operator==(const BatchIterator &other) const {return (_vec == other._vec && _i == other._i);}
        bool operator!=(const BatchIterator &other) const {return !(*this == other);}
        bool operator<(const BatchIterator &other) const {return (_vec == other._vec && _i < other._i);}
        bool operator<=(const BatchIterator &other) const {return (_vec == other._vec && _i <= other._i);}
        bool operator>(const BatchIterator &other) const {return (_vec == other._vec && _i > other._i);}
        bool operator>=(const BatchIterator &other) const {return (_vec == other._vec && _i >= other._i);}

        friend class RCacheVector;
    };

    const uint32_t _data_size = sizeof(T);

    uint32_t _chunk_num_entries;  // #data per cache line, may have internal gaps
    uint64_t _offset;             // starting offset of the data in remote, should be on the boundaries

    uint64_t _capacity;
    uint64_t _size = 0;

    // returns i_chunk, i_in_chunk
    std::pair<Index_t, Index_t> which_chunk(Index_t i) {
        return std::make_pair(i / _chunk_num_entries, i % _chunk_num_entries);
    }

    // returns addr_chunk_beg, offset_i_chunk
    std::pair<intptr_t, intptr_t> where_cache(uint64_t offset, Index_t i) {
        auto [chunk_idx, chunk_offset] = which_chunk(i);
        return std::make_pair(offset+chunk_idx*cline_size, chunk_offset);
    }

    uint64_t last_chunk() {
        return which_chunk(_size-1).first;
    }

    // copy from o2 to o1
    void cache_copy(uint64_t o1, Index_t i1, uint64_t o2, Index_t i2) {
        auto [c_addr, c_offset] = where_cache(o1, i1);
        auto [old_c_addr, old_c_offset] = where_cache(o2, i2);

        auto token = cache_request(ctable, c_addr);
        auto old_token = cache_request(ctable, old_c_addr);
        cache_await(token);
        cache_await(old_token);
        T* to_read = reinterpret_cast<T*>(cache_access(old_token))+old_c_offset;
        T* to_write = reinterpret_cast<T*>(cache_access_mut(token))+c_offset;
        *to_write = *to_read;
    }

   public:
    explicit RCacheVector() : RCacheVector(0) {}

   RCacheVector(uint64_t atleast_cap) {
      if (!ctable_inited) throw std::runtime_error("RCacheVector_init_cache_table first");

      _chunk_num_entries = cline_size / _data_size;
      if (!_chunk_num_entries) throw std::runtime_error("cache line size too small");
      auto num_chunks_to_alloc = (atleast_cap) ? (atleast_cap-1) / _chunk_num_entries + 1 : 0;
      _capacity = num_chunks_to_alloc * _chunk_num_entries;
      _offset = get_new_vec_offset(num_chunks_to_alloc * cline_size);
    }

    RCacheVector &operator=(const RCacheVector &other) {
        // TODO: release old offset
        _chunk_num_entries = other._chunk_num_entries;
        _capacity = other._capacity;
        _offset = get_new_vec_offset(_capacity/_chunk_num_entries * cline_size);
        _size = other._size;

        auto old_offset = other._offset;
        for (int i = 0; i < _size; ++i) {
            // TODO: better copy
            cache_copy(_offset, i, old_offset, i);
        }
    }
    RCacheVector(RCacheVector &&other) {
        // TODO: release old offset
        _chunk_num_entries = other._chunk_num_entries;
        _offset = other._offset;
        _capacity = other._capacity;
        _size = other._size;
    }
    RCacheVector &operator=(RCacheVector &&other) {
        // TODO: release old offset
        _chunk_num_entries = other._chunk_num_entries;
        _offset = other._offset;
        _capacity = other._capacity;
        _size = other._size;
    }
    ~RCacheVector() {
        // TODO: release offset
    }

    uint32_t chunk_num_entries() const {return _chunk_num_entries;}

    uint64_t capacity() const { return _capacity; }
    uint64_t size() const {return _size;}
    bool empty() const {return !_size;}
    void push_back(T &&t) {
        if (_capacity == _size + 1) [[unlikely]] {
            resize(_capacity*2+1);
        }

        auto [c_addr, c_offset] = where_cache(_offset, _size);
        auto token = cache_request(ctable, c_addr);
        cache_await(token);
        T* to_write = reinterpret_cast<T*>(cache_access_mut(token))+c_offset;

        *to_write = t;

        _size += 1;

        // TODO: prefech
        // TODO: batch write to remote
        // TODO: smarter remote?
    }
    void push_back( const T& t ) {
        if (_capacity == _size + 1) [[unlikely]] {
            resize(_capacity*2+1);
        }

        auto [c_addr, c_offset] = where_cache(_offset, _size);
        auto token = cache_request(ctable, c_addr);
        cache_await(token);
        T* to_write = reinterpret_cast<T*>(cache_access_mut(token))+c_offset;

        *to_write = t;

        _size += 1;

        // same TODO above
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
        _capacity = num_entries_to_alloc * _chunk_num_entries;
        _offset = get_new_vec_offset(num_entries_to_alloc * cline_size);

        for (int i = 0; i < _size; ++i) {
            // TODO: better copy
            cache_copy(_offset, i, old_offset, i);
        }

        // TODO: release old offset
    }

    // return copy only
    T nth_element(Index_t i) {
        auto [c_addr, c_offset] = where_cache(_offset, i);
        auto token = cache_request(ctable, c_addr);
        cache_await(token);
        return *(reinterpret_cast<T*>(cache_access(token))+c_offset);
    }
    T front() { return nth_element(0); }
    const T back() { return nth_element(_size-1); }
    T at(Index_t i) { return nth_element(i); }

    // the only mutator
    void update(Index_t i, T&& t) {
        if (i >= _size) throw std::runtime_error("update out of index");

        auto [c_addr, c_offset] = where_cache(_offset, i);
        auto token = cache_request(ctable, c_addr);
        cache_await(token);
        T* to_write = reinterpret_cast<T*>(cache_access_mut(token))+c_offset;

        *to_write = t;
    }

    // CIterator begin() {return CIterator(this,0);}
    // CIterator end() {return CIterator(this, _size);}
    const CIterator begin() const {return CIterator(const_cast<RCacheVector<T>*>(this),0);}
    const CIterator end() const {return CIterator(const_cast<RCacheVector<T>*>(this), _size);}
    const CIterator cbegin() const {return CIterator(const_cast<RCacheVector<T>*>(this), 0);}
    const CIterator cend() const {return CIterator(const_cast<RCacheVector<T>*>(this), _size);}

    const BatchIterator batch_begin(uint64_t n_prefetch_lines = 1) const {return BatchIterator(const_cast<RCacheVector<T>*>(this),0, n_prefetch_lines);}
    const BatchIterator batch_end(uint64_t n_prefetch_lines = 1) const {return BatchIterator(const_cast<RCacheVector<T>*>(this), _size, n_prefetch_lines);}
    const BatchIterator batch_cbegin(uint64_t n_prefetch_lines = 1) const {return BatchIterator(const_cast<RCacheVector<T>*>(this), 0, n_prefetch_lines);}
    const BatchIterator batch_cend(uint64_t n_prefetch_lines = 1) const {return BatchIterator(const_cast<RCacheVector<T>*>(this), _size, n_prefetch_lines);}

    // void disable_prefetch();
    // void enable_prefetch();
    void prefetch(Index_t i, uint64_t n) {
        throw std::runtime_error("not implemented");
        if (!n) return;
        // auto chunk_idx = std::min(last_chunk(), which_chunk(i).first);
        // auto chunk_idx_end = std::min(last_chunk(), which_chunk(i+n-1).first);

        // for (auto c = chunk_idx; c <= chunk_idx_end; ++c) {
            // cache_prefetch(ctable, _offset+c*cline_size);
        // }
    }
    void prefetch_nlines(Index_t i, uint64_t n) {
        if (!n) return;
        auto clast = last_chunk();
        auto ccur = which_chunk(i).first;
        if (clast < ccur) return;

        auto cend = std::min(clast, ccur+n-1);

        for (auto c = ccur; c <= cend; ++c) {
            // auto token = cache_request(ctable, offset+c*cline_size);
            cache_request(ctable, _offset+c*cline_size);
        }
    }

    // template<typename V>
    // void batch_visitor(Index_t i, Index_t n, uint64_t prefetch_lines, V& visitor) {
    //     if (!n) return;
    //     Index_t iend = i + n;
    //     if (i > _size || iend > _size) return;

    //     // if not prefetching, use non-batch version
    //     // avoid requesting the same cache line
    //     if (prefetch_lines == 0) prefetch_lines = 1;

    //     std::array<cache_token_t, prefetch_lines> tokens;
    //     uint64_t n_prefetch = prefetch_lines * _chunk_num_entries;
    //     uint64_t times_prefetch = (n-1) / n_prefetch + 1;
    //     while (times_prefetch--) {
    //         for (int i_p = 0; i_p < prefetch_lines; ++i_p) { // prefetch the lines, at least one
    //             // i + #entries is guaranteed to cross cache line boundary
    //             auto [c_addr, c_offset] = where_cache(_offset, i+i_p*_chunk_num_entries);
    //             tokens[i_p] = cache_request(ctable, c_addr);
    //         }

    //         for (int i_p = 0; i_p < prefetch_lines; ++i_p) {
    //             cache_await(tokens[i_p]);
    //             // the first and the last iteration may not access the full cache line
    //             for (Index_t next = std::min(i - which_chunk(i).second + _chunk_num_entries, iend); i < next; ++i) {
    //                 visitor (*(reinterpret_cast<T*>(cache_access(token))+where_chunk(i).second));
    //             }
    //         }
    //     }
    // }
};

#endif
