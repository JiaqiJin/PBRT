#pragma once

#include "../core/Header.h"
#include <thread>
#include "../tools/stats.h"
namespace Rendering
{
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
            uint64_t oldBits = _bits, newBits;
#else
            uint32_t oldBits = _bits, newBits;
#endif
            do {
                newBits = floatToBits(bitsToFloat(oldBits) + v);
            } while (!_bits.compare_exchange_weak(oldBits, newBits));
            // 如果oldBits与_bits不相等，则把oldBits改为_bits的值，返回false
            // 如果oldBits与_bits相等，则把_bit的值改为newBits，返回true
        }

    private:

#ifdef PALADIN_FLOAT_AS_DOUBLE
        std::atomic<uint64_t> _bits;
#else
        std::atomic<uint32_t> _bits;
#endif
    };

    /**
     * 可以理解为栅栏
     * 保证一切准备就绪之后，所有子线程尽可能的同时开始工作
     */
    class Barrier {
    public:
        Barrier(int count)
            : _count(count) {
            CHECK_GT(_count, 0);
        }

        ~Barrier() {
            CHECK_EQ(_count, 0);
        }

        void wait() {
            std::unique_lock<std::mutex> lock(_mutex);
            CHECK_GT(_count, 0);
            if (--_count == 0) {
                _cv.notify_all();
            }
            else {
                _cv.wait(lock, [this] { return _count == 0; });
            }
        }

    private:
        // 互斥锁
        std::mutex _mutex;
        // 当 std::condition_variable 对象的某个 wait 函数被调用的时候，
        // 它使用 std::unique_lock(通过 std::mutex) 来锁住当前线程。当前线程会一直被阻塞，
        // 直到另外一个线程在相同的 std::condition_variable 对象上调用了 notification 函数来唤醒当前线程。
        std::condition_variable _cv;
        int _count;
    };

    struct ParallelForLoop {

    public:
        ParallelForLoop(std::function<void(int64_t)> func1D, int64_t maxIndex,
            int chunkSize, uint64_t profilerState)
            : func1D(std::move(func1D)),
            maxIndex(maxIndex),
            chunkSize(chunkSize),
            profilerState(profilerState) {

        }

        ParallelForLoop(const std::function<void(Point2i, int)>& f, const Point2i& count,
            uint64_t profilerState)
            : func2D(f),
            maxIndex(count.x* count.y),
            chunkSize(1),
            profilerState(profilerState) {
            numX = count.x;
        }

    public:
        // 一维变量函数
        std::function<void(int64_t)> func1D;
        // 二维变量函数
        std::function<void(Point2i, int)> func2D;
        // 最大迭代次数
        const int64_t maxIndex;

        // 每次迭代的循环次数
        // 把n个任务分成若干块，每块的大小为chunkSize
        // 把n个任务分配给子线程也是按块分配
        const int chunkSize;

        uint64_t profilerState;
        // 下个迭代索引
        int64_t nextIndex = 0;
        // 当前激活worker线程
        int activeWorkers = 0;

        ParallelForLoop* next = nullptr;
        // 二维函数需要用到的属性
        int numX = -1;

        bool finished() const {
            return nextIndex >= maxIndex && activeWorkers == 0;
        }
    };

    extern thread_local int ThreadIndex;

    void parallelFor(std::function<void(int64_t)> func, int64_t count, int chunkSize = 1);


    void parallelFor2D(std::function<void(Point2i, int)> func, const Point2i& count);

    inline int numSystemCores() {
        return std::max(1u, std::thread::hardware_concurrency());
    }

    int maxThreadIndex();

    int getCurThreadIndex();

    void setThreadNum(int num);

    int getThreadNum();

    void parallelInit(int num = 0);

    void parallelCleanup();

    void mergeWorkerThreadStats();


}