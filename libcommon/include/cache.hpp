#ifndef _CACHE_HPP_
#define _CACHE_HPP_

#include <infiniband/verbs.h>
#include <stdint.h>

#include "common.h"
#include "rdmaop.hpp"
#include "cache_token.hpp"
#include "pqueue.hpp"
#include "tlb.hpp"
#include "queue.h"

// Common cache operations
template <int offset, uint64_t rbuf, uint64_t buf, uint64_t linesize, int qid,
          uint16_t reqwr_opts = REQWR_OPT_QUEUE_UPDATE>
struct CacheOp {
    static constexpr uint64_t tagmask = ~(linesize - 1);
    static inline uint64_t tag(uint64_t vaddr) {
        return vaddr & tagmask;
    }

    // meta data
    // static Token _tokens[slots];
    static inline Token &token(int off) { return tokens[off+offset]; }

    // translate
    template <typename T>
    static inline T * paddr(int off, uint64_t vaddr) {
        return (T*)(_rbuf + buf + off * linesize + vaddr % linesize);
    }
    
    // cache info
    static inline uint16_t rid() { return qi[qid].rid; }
    static inline uint16_t sid() { return qi[qid].sid; }

    // rdma
    // TODO: wrid
    static inline void rdma(int i, int off, int tag, ibv_wr_opcode opcode,
            ibv_send_wr * next, bool with_seq) {
        // COULD be an option. do this later
        uint64_t wrid = 0;
        if (with_seq) {
            wrid = reqwr_opts | qid;
            uint32_t sid = ++qi[qid].sid;
            tokens[off+offset].seq = sid;
            wrid |= (sid << 16);
        }
        build_rdma_wr(i, wrid, buf + off * linesize, rbuf + tag, 
                linesize, opcode, next);
    };
};

// This set of caches are built for single threaded. 
template <uint64_t _buf_offset, int _slots, int _linesize, int _qid>
struct CacheValues {
    static constexpr int linesize = _linesize;
    static constexpr int qid = _qid;
    static constexpr int slots = _slots;
    static constexpr uint64_t bytes = _linesize * (uint64_t)_slots;
};

template <int offset, uint64_t rbuf, uint64_t buf, int slots, int linesize, int qid>
struct DirectCache {
    using Op = CacheOp<offset, rbuf, buf, linesize, qid>;
    using Value = CacheValues<buf,slots,linesize,qid>;

    static constexpr uint64_t raddr = rbuf;

    static inline int select(uint64_t vaddr) {
        int res = (vaddr / linesize) % slots;
        return res;
    }
};

template <int offset, uint64_t rbuf, uint64_t buf, int slots, int linesize, int qid,
          int num_ways>
struct SetAssocativeCache {
    using Op = CacheOp<offset, rbuf, buf, linesize, qid>;
    using Value = CacheValues<buf,slots,linesize,qid>;

    static constexpr int waysize = linesize * num_ways;
    static PQueue<uint8_t, num_ways> queues[slots] = {0}; 

    static inline int select(uint64_t vaddr) {
        int wayslot = (vaddr / waysize) % (slots / num_ways) * num_ways;
        auto pqueue = queues[wayslot];
        int i, target = num_ways;
        for (i = 0; i < num_ways; i++) {
            auto &token = Op::token(wayslot + i);
            if (token.valid()) {
                if (token.tag == Op::tag(vaddr)) break;
            } else
                target = i;
        }
        if (i != num_ways) { // find a number
            pqueue.repush(i);
            return i;
        } else { // eviction
            if (target != num_ways)
                return target;
            else return pqueue.pop();
        }
    }
};

// acquire buffer
struct NoAcquireBuffer {
    static constexpr bool enable = false;
};

template <typename C, int offset, int entries, uint64_t laddr, uint64_t raddr>
struct AcquireBuffer {
    static constexpr bool enable = true;
    using Op = CacheOp<offset,raddr,laddr,C::Value::linesize,C::Value::qid>;

    template <int i>
    static inline void acquire(uint64_t vaddr) {
        uint64_t tag = Op::tag(vaddr);
        auto & token = Op::token(i);

        int coffset = C::select(tag);
        auto & ctoken = C::Op::token(i);

#if 1
        // check if already acquired..
        if (ctoken.acquire())
            printf("ERROR: Conflict Acquire!\n");
#endif
        ctoken.add(Token::Acquire);
        ctoken.meta2 = i;
        token.tag = tag;
        token.meta2 = coffset;

        // when we acquire we are sure we won't names
        Op::rdma(0, i, tag, IBV_WR_RDMA_READ, NULL, true);
        struct ibv_send_wr *badwr = NULL;
        ibv_post_send(qp, _pwr, &badwr);
    }

    template <int i>
    static inline void release() {
        auto & token = Op::token(i);
        int coffset = token.meta2;
        auto & ctoken = C::Op::token(i);

        ctoken.sub(Token::Acquire);

        if (token.dirty()) {
            Op::rdma(0, i, token.tag, IBV_WR_RDMA_WRITE, NULL, true);
            struct ibv_send_wr *badwr = NULL;
            ibv_post_send(qp, _pwr, &badwr);
        }
    }

    // 1. could be quired by id -> using A::Op::paddr(i, vaddr)
};

// TODO: bool?
// bool ignore
template <typename C, typename A = NoAcquireBuffer, typename Tlb = NoTlb,
         bool opt_writeback = true, bool opt_sync = false>
struct CacheReq {

static inline int cache_request_impl(int wr_offset, uint64_t tag, int offset,
        ibv_send_wr *req, bool send) {
    auto &token = C::Op::token(offset);

    struct ibv_send_wr *badwr = NULL;
    req = _pwr + wr_offset;
    C::Op::rdma(wr_offset, offset, tag, IBV_WR_RDMA_READ, NULL, send);

    if constexpr (opt_writeback) {
        // only enable if we want write back
        if (token.dirty()) { // build req 0 (write)
            C::Op::rdma(wr_offset + 1, offset, token.tag,
                    IBV_WR_RDMA_WRITE, req, false);
            req = _pwr + wr_offset + 1;

            // update b
            Tlb::invalid(tag);
        }
    }

    // the associated id is send id
    if (send) {
        // should already updated in RDMA function
        int ret = ibv_post_send(qp, req, &badwr);
        // dprintf("RDMA post send ret %d", ret); 
    }

    // update tag
    token.tag = tag;
    // Update flags, we have 2 mode, sync or not 
    if constexpr (!opt_sync)
        C::Op::token(offset).set(Token::Valid);
    else {
        C::Op::token(offset).set(Token::Sync);
    }

    return offset;
}

static void request_poll(int offset, uint64_t tag) {
    cache_request_impl(0, tag, offset, NULL, true);
    poll_qid(C::Value::qid, C::Op::token(offset).seq);
}

static int request(int offset, uint64_t tag) {
    return cache_request_impl(0, tag, offset, NULL, true);
}

static int request(int offset, uint64_t tag, bool send) {
    return cache_request_impl(0, tag, offset, NULL, send);
}


// only work in mode
template<typename T>
static inline T * get(void * vaddr) {
    uint64_t tag = C::Op::tag((uint64_t)vaddr);
    // TODO: this is not thread safe
    // if (Tlb::lookup(tag))
        // return Tlb::offset();

    int off = C::select(tag);
    auto &token = C::Op::token(off);

    auto ret = C::Op::template paddr<T>(off, (uint64_t)vaddr);
    if (token.valid() && token.tag == tag) {
        // Tlb::update(token,tag);
        return ret;
    } 

    if constexpr (A::enable) {
        if (token.acquire() && A::Op::token(token.meta2).tag == tag) {
            return A::Op::template paddr<T>(token.meta2, (uint64_t)vaddr);
        }
    }

    // TODO: flag for sync mode

    request_poll(off, tag);

    // printf("Sync cache[%d] <%d|r:%d,s:%d>\n", C::Value::qid,
    //         C::Op::token(off).seq, C::Op::rid(), C::Op::sid());
    // poll_qid(C::Value::qid, C::Op::token(off).seq);
    return ret;
}

template<typename T>
static inline T * get_mut(void * vaddr) {
    uint64_t tag = C::Op::tag((uint64_t)vaddr);
    int off = C::select(tag);
    auto &token = C::Op::token(off);

    auto ret = C::Op::template paddr<T>(off, (uint64_t)vaddr);

    if (token.valid() && token.tag == tag) {
        token.add(Token::Dirty);
        return ret;
    }

    if constexpr (A::enable) {
        auto & atoken = A::Op::token(token.meta2);
        if (token.acquire() && atoken.tag == tag) {
            atoken.add(Token::Dirty);
            return A::Op::template paddr<T>(token.meta2, (uint64_t)vaddr);
        }
    }

    request_poll(off, tag);

    token.add(Token::Dirty);
    return ret;
}

static inline void * alloc(size_t size) {
    void * target = (void *)(qi[C::Value::qid].addr + (uint64_t)peermr.addr);
    // void * target = (void *)qi[C::Value::qid].addr;
    qi[C::Value::qid].addr += size;
    return target;
}

static void flush(uint64_t lower_bound, uint64_t upper_bound) {
    int inflight = 0;
    for (int i = 0; i < C::Value::slots; i++) {
        auto &token = C::Op::token(i);
        if (token.valid() && token.tag >= lower_bound && token.tag <= upper_bound) {
            if (token.dirty()) {
                C::Op::rdma(0, i, token.tag, IBV_WR_RDMA_WRITE, NULL, false);
                struct ibv_send_wr *badwr = NULL;
                ibv_post_send(qp, _pwr, &badwr);
                if (++inflight > 32) {
                    // poll
                    poll_qid(C::Value::qid, qi[C::Value::qid].sid - 16);
                }
            }
            token.clear();
        }
    }
    // We will poll later
}

#if 0
template <typename C, typename Tlb, int size,
         bool opt_writeback = true, bool opt_tlb = false>
void cache_request_batch(const uint64_t *arr, int *offsets) {
    for (int i = 0; i < size; i++){
        // TODO: chain the requests!
        offset[i] = cache_request_impl<C,Tlb,opt_writeback, opt_tlb>(i*2);
    }
}
#endif

};

// lifetime and lightning values: non-persistent
// Create a function handling lightinig values. 

#endif

