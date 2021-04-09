#ifndef memory_hpp
#define memory_hpp

#include <list>
#include <cstddef>

#ifndef RENDERING_L1_CACHE_LINE_SIZE
#define RENDERING_L1_CACHE_LINE_SIZE 64
#endif

namespace Rendering 
{

    // Memory Declarations
#define ARENA_ALLOC(arena, Type) new ((arena).alloc(sizeof(Type))) Type
    void* allocAligned(size_t size);
    template <typename T>
    T* allocAligned(size_t count) {
        return (T*)allocAligned(count * sizeof(T));
    }

    void freeAligned(void*);

    class alignas(RENDERING_L1_CACHE_LINE_SIZE) MemoryArena {
    public:
        /*
        MemoryArena是内存池的一种，基于arena方式分配内存
        整个对象大体分为三个部分
            1.可用列表，用于储存可分配的内存块
            2.已用列表，储存已经使用的内存块
            3.当前内存块
        */
        MemoryArena(size_t blockSize = 262144) : _blockSize(blockSize) {

        }

        ~MemoryArena() {
            freeAligned(_currentBlock);

            for (auto& block : _usedBlocks) {
                freeAligned(block.second);
            }

            for (auto& block : _availableBlocks) {
                freeAligned(block.second);
            }
        }

        void* alloc(size_t nBytes) {

            // 16位对齐，对齐之后nBytes为16的整数倍
            nBytes = (nBytes + 15) & ~(15);
            if (_currentBlockPos + nBytes > _currentAllocSize) {
                // 如果已经分配的内存加上请求内存大于当前内存块大小

                if (_currentBlock) {
                    // 如果当前块不为空，则把当前块放入已用列表中
                    _usedBlocks.push_back(std::make_pair(_currentAllocSize, _currentBlock));
                    _currentBlock = nullptr;
                    _currentAllocSize = 0;
                }

                // 在可用列表中查找是否有尺寸大于请求内存的块
                for (auto iter = _availableBlocks.begin(); iter != _availableBlocks.end(); ++iter) {
                    if (iter->first >= nBytes) {
                        // 如果找到将当前块指针指向该块，并将该块从可用列表中移除
                        _currentAllocSize = iter->first;
                        _currentBlock = iter->second;
                        _availableBlocks.erase(iter);
                        break;
                    }
                }

                if (!_currentBlock) {
                    // 如果没有找到符合标准的内存块，则申请一块内存
                    _currentAllocSize = std::max(nBytes, _blockSize);
                    _currentBlock = allocAligned<uint8_t>(_currentAllocSize);
                }
                _currentBlockPos = 0;
            }
            void* ret = _currentBlock + _currentBlockPos;
            _currentBlockPos += nBytes;
            return ret;
        }

        template <typename T>
        T* alloc(size_t n = 1, bool runConstructor = true) {
            T* ret = (T*)alloc(n * sizeof(T));
            if (runConstructor) {
                for (size_t i = 0; i < n; ++i) {
                    new (&ret[i]) T();
                }
            }
            return ret;
        }

        void reset() {
            _currentBlockPos = 0;
            _availableBlocks.splice(_availableBlocks.begin(), _usedBlocks);
        }

        //获取已经分配的内存大小
        size_t totalAllocated() const {
            size_t total = _currentAllocSize;
            for (const auto& alloc : _usedBlocks) {
                total += alloc.first;
            }
            for (const auto& alloc : _availableBlocks) {
                total += alloc.first;
            }
            return total;
        }

    private:
        MemoryArena(const MemoryArena&) = delete;
        MemoryArena& operator=(const MemoryArena&) = delete;

        // 默认内存块大小
        const size_t _blockSize;

        // 当前块已经分配的位置
        size_t _currentBlockPos = 0;

        // 当前块的尺寸
        size_t _currentAllocSize = 0;

        // 当前块指针
        uint8_t* _currentBlock = nullptr;

        // 已经使用的内存块列表
        std::list<std::pair<size_t, uint8_t*>> _usedBlocks;

        // 可使用的内存块列表
        std::list<std::pair<size_t, uint8_t*>> _availableBlocks;
    };

    template <typename T, int logBlockSize>
    class BlockedArray {
    public:
        // BlockedArray Public Methods
        BlockedArray(int uRes, int vRes, const T* d = nullptr)
            : uRes(uRes), vRes(vRes), uBlocks(RoundUp(uRes) >> logBlockSize) {
            int nAlloc = RoundUp(uRes) * RoundUp(vRes);
            data = allocAligned<T>(nAlloc);
            for (int i = 0; i < nAlloc; ++i) new (&data[i]) T();
            if (d)
                for (int v = 0; v < vRes; ++v)
                    for (int u = 0; u < uRes; ++u) (*this)(u, v) = d[v * uRes + u];
        }
        constexpr int BlockSize() const { return 1 << logBlockSize; }
        int RoundUp(int x) const {
            return (x + BlockSize() - 1) & ~(BlockSize() - 1);
        }
        int uSize() const { return uRes; }
        int vSize() const { return vRes; }
        ~BlockedArray() {
            for (int i = 0; i < uRes * vRes; ++i) data[i].~T();
            freeAligned(data);
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

}

#endif /* memory_hpp */
