#ifndef parallel_hpp
#define parallel_hpp

#include "header.h"
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

PALADIN_BEGIN

class AtomicFloat {
public:

    explicit AtomicFloat(Float v = 0) {
        _bits = floatToBits(v);
    }

    operator Float() const {
        return bitsToFloat(_bits);
    }

    Float operator=(Float v) {
        _bits = floatToBits(v);
        return v;
    }
    void add(Float v) {
#ifdef PALADIN_FLOAT_AS_DOUBLE
        uint64_t oldBits = bits, newBits;
#else
        uint32_t oldBits = _bits, newBits;
#endif
        do {
            newBits = floatToBits(bitsToFloat(oldBits) + v);
        } while (!_bits.compare_exchange_weak(oldBits, newBits));
    }

private:
    // AtomicFloat Private Data
#ifdef PALADIN_FLOAT_AS_DOUBLE
    std::atomic<uint64_t> _bits;
#else
    std::atomic<uint32_t> _bits;
#endif
};


PALADIN_END

#endif