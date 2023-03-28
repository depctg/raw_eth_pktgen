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
    static constexpr uint64_t bytes = _linesize * (uint64_t)_slots;
};

template <int offset, uint64_t rbuf, uint64_t buf, int slots, int linesize, int qid>
struct DirectCache {
    using Op = CacheOp<offset, rbuf, buf, linesize, qid>;
    using Value = CacheValues<buf,slots,linesize,qid>;

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

// TODO: bool?
// bool ignore
template <typename C, typename Tlb = NoTlb,
         bool opt_writeback = true, bool opt_sync = false>
struct CacheReq {

static inline int cache_request_impl(int wr_offset, uint64_t vaddr,
        ibv_send_wr *req, bool send) {
    uint64_t tag = C::Op::tag(vaddr);
    // dprintf("Cache %lx -> %lx", vaddr, tag);

    // check tlb
    // TODO: this is not thread safe
    if (Tlb::lookup(tag))
        return Tlb::offset();

    // find slot and eviction
    int offset = C::select(tag);
    auto &token = C::Op::token(offset);

    // select will set the flags
    // valid() means we find a match
    // TODO: on set-assoc, this will cause double compare
    if (token.valid() && token.tag == tag) { // if valid return
        token.seq = qi[C::Value::qid].rid;
        // if valid and hot add to tlb
        Tlb::update(token,tag);
    } else if (opt_sync && token.sync()) {
        // DO nothing, basically we do not need to change seq
        // TODO: sync we need
    } else { // else fetch, build req (read)
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
    }

    return offset;
}

static int request(uint64_t vaddr) {
    return cache_request_impl(0, vaddr, NULL, true);
}

static int request(uint64_t vaddr, bool send) {
    return cache_request_impl(0, vaddr, NULL, send);
}

template<typename T>
static inline T * get(void * vaddr) {
    int off = request((uint64_t)vaddr);
    // dprintf("Sync cache[%d] <%d|r:%d,s:%d>", C::Value::qid,
    //         C::Op::token(off).seq, C::Op::rid(), C::Op::sid());
    poll_qid(C::Value::qid, C::Op::token(off).seq);
    return C::Op::template paddr<T>(off, (uint64_t)vaddr);
}

template<typename T>
static inline T * get_mut(void * vaddr) {
    int off = request((uint64_t)vaddr);
    // dprintf("Sync cache[%d] <%d|r:%d,s:%d>", C::Value::qid,
    //         C::Op::token(off).seq, C::Op::rid(), C::Op::sid());
    poll_qid(C::Value::qid, C::Op::token(off).seq);
    C::Op::token(off).add(Token::Dirty);
    return C::Op::template paddr<T>(off, (uint64_t)vaddr);
}

static inline void * alloc(size_t size) {
    void * target = (void *)qi[C::Value::qid].addr;
    qi[C::Value::qid].addr += size;
    return target;
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

