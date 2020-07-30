#ifndef memory_hpp
#define memory_hpp

#include "header.h"
#include <list>
#include <cstddef>
/*
Arena base allication provide allocate objects from a large contiguos region of memory.
The entire region is release when the lifetime of all the allocated objects ends.
*/
KAWAII_BEGIN

// Memory Declarations
#define ARENA_ALLOC(arena, Type) new ((arena).Alloc(sizeof(Type))) Type
void* AllocAligned(size_t size);
template <typename T>
T* AllocAligned(size_t count) {
    return (T*)AllocAligned(count * sizeof(T));
}

void FreeAligned(void*);

class
#ifdef KAWAII_HAVE_ALIGNAS
alignas(KAWAII_L1_CACHE_LINE_SIZE)
#endif // PBRT_HAVE_ALIGNAS
MemoryArena
{
public:
    // MemoryArena Public Methods
    MemoryArena(size_t blockSize = 262144) : blockSize(blockSize) {}
    ~MemoryArena() {
        FreeAligned(currentBlock);
        for (auto& block : usedBlocks) FreeAligned(block.second);
        for (auto& block : availableBlocks) FreeAligned(block.second);
    }
    
    void* Alloc(size_t nBytes) {
        nBytes = ((nBytes + 15) & (~15));
        if (currentBlockPos + nBytes > currentAllocSize) {
            // Add current block to _usedBlocks_ list
            if (currentBlock) {
                usedBlocks.push_back(
                    std::make_pair(currentAllocSize, currentBlock));
                currentBlock = nullptr;
                currentAllocSize = 0;
            }

            // Get new block of memory for _MemoryArena_

            // Try check allocated free block from _availableBlocks_ 
            for (auto iter = availableBlocks.begin();
                iter != availableBlocks.end(); ++iter) {
                if (iter->first >= nBytes) {
                    currentAllocSize = iter->first;
                    currentBlock = iter->second;
                    availableBlocks.erase(iter);
                    break;
                }
            }
            //Block of memory of suitable size isnt avaliable
            if (!currentBlock) {
                currentAllocSize = std::max(nBytes, blockSize);
                currentBlock = AllocAligned<uint8_t>(currentAllocSize); //Current size * uint8_t
            }
            currentBlockPos = 0;
        }
        void* ret = currentBlock + currentBlockPos;
        currentBlockPos += nBytes;
        return ret;
    }
   
    //Allocated any array of object giving type
    template <typename T>
    T* Alloc(size_t n = 1, bool runConstructor = true) {
        T* ret = (T*)Alloc(n * sizeof(T));
        if (runConstructor)
            for (size_t i = 0; i < n; ++i) new (&ret[i]) T();
        return ret;
    }

    void Reset() {
        currentBlockPos = 0;
        availableBlocks.splice(availableBlocks.begin(), usedBlocks);
    }

    size_t TotalAllocated() const{
        size_t total = currentAllocSize;
        for (const auto& alloc : usedBlocks) total += alloc.first;
        for (const auto& alloc : availableBlocks) total += alloc.first;
        return total;
    }

private:

    const size_t blockSize; // 256 defalt kB if no parameter in constructor
    // offset of free location in the block and store size of the current Block
    size_t currentBlockPos = 0, currentAllocSize = 0;
    //pointer to current block of memory
    uint8_t* currentBlock = nullptr;
    //two linked list to hold pointer to block memory
    std::list<std::pair<size_t, uint8_t*>> usedBlocks, availableBlocks;
};

template<typename T, int logBlockSize>
class BlockedArray {
public:
    // BlockedArray Public Methods
    BlockedArray(int uRes, int vRes, const T* d = nullptr)
        : uRes(uRes), vRes(vRes), uBlocks(RoundUp(uRes) >> logBlockSize) {
        int nAlloc = RoundUp(uRes) * RoundUp(vRes);
        data = AllocAligned<T>(nAlloc);
        for (int i = 0; i < nAlloc; ++i) new (&data[i]) T();
        if (d)
            for (int v = 0; v < vRes; ++v)
                for (int u = 0; u < uRes; ++u) (*this)(u, v) = d[v * uRes + u];
    }
    CONSTEXPR int BlockSize() const { return 1 << logBlockSize; }
    //round up to multiple of the block size
    int RoundUp(int x) const {
        return (x + BlockSize() - 1) & ~(BlockSize() - 1);
    }
    int uSize() const { return uRes; }
    int vSize() const { return vRes; }
    ~BlockedArray() {
        for (int i = 0; i < uRes * vRes; ++i) data[i].~T();
        FreeAligned(data);
    }
    int Block(int a) const { return a >> logBlockSize; }
    int Offset(int a) const { return (a & (BlockSize() - 1)); }
    T& operator()(int u, int v) {
        int bu = Block(u), bv = Block(v);
        int ou = Offset(u), ov = Offset(v);
        int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
        offset += BlockSize() * ov + ou;
        return data[offset];
    }
    const T& operator()(int u, int v) const {
        int bu = Block(u), bv = Block(v);
        int ou = Offset(u), ov = Offset(v);
        int offset = BlockSize() * BlockSize() * (uBlocks * bv + bu);
        offset += BlockSize() * ov + ou;
        return data[offset];
    }
    void GetLinearArray(T* a) const {
        for (int v = 0; v < vRes; ++v)
            for (int u = 0; u < uRes; ++u) *a++ = (*this)(u, v);
    }

private:
    // BlockedArray Private Data
    T* data;
    const int uRes, vRes, uBlocks;
};

KAWAII_END


#endif /* memory_hpp */