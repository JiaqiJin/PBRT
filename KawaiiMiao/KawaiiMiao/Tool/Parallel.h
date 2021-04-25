#pragma once

#include "../Core/Rendering.h"

#include <mutex>
#include <atomic>
#include <functional>
#include <condition_variable>

#include <tbb/tbb/spin_mutex.h>
#include <tbb/tbb/parallel_for.h>

RENDER_BEGIN

class AtomicFloat
{
public:

	explicit AtomicFloat(Float v = 0) { bits = floatToBits(v); }

	operator Float() const { return bitsToFloat(bits); }

	Float operator=(Float v)
	{
		bits = floatToBits(v);
		return v;
	}

	void add(Float v)
	{
#ifdef RENDER_DOUBLE_AS_FLOAT
		uint64_t oldBits = bits, newBits;
#else
		uint32_t oldBits = bits, newBits;
#endif
		do
		{
			newBits = floatToBits(bitsToFloat(oldBits) + v);
		} while (!bits.compare_exchange_weak(oldBits, newBits));
	}

private:
	// AtomicFloat Private Data
#ifdef RENDER_DOUBLE_AS_FLOAT
	std::atomic<uint64_t> bits;
#else
	std::atomic<uint32_t> bits;
#endif
};

// Simple one-use barrier; ensures that multiple threads all reach a
// particular point of execution before allowing any of them to proceed
// past it.
//
// Note: this should be heap allocated and managed with a shared_ptr, where
// all threads that use it are passed the shared_ptr. This ensures that
// memory for the Barrier won't be freed until all threads have
// successfully cleared it.
class Barrier
{
public:
	Barrier(int count) : m_count(count) { CHECK_GT(count, 0); }
	~Barrier() { CHECK_EQ(m_count, 0); }
	void wait();

private:
	std::mutex m_mutex;
	std::condition_variable m_cv;
	int m_count;
};

using FilmMutexType = tbb::spin_mutex;

//Execution policy tag.
enum class ExecutionPolicy { ASERIAL, APARALLEL };

//parallel for loop with automic chunking
template <typename Function>
void parallelFor(size_t beginIndex, size_t endIndex,
	const Function& function, ExecutionPolicy policy = ExecutionPolicy::APARALLEL);

//parallel for loop with manual chunking
template <typename Function>
void parallelFor(size_t beginIndex, size_t endIndex, size_t grainSize,
	const Function& function, ExecutionPolicy policy = ExecutionPolicy::APARALLEL);

template <typename Function>
void parallelFor(size_t start, size_t end, const Function& func, ExecutionPolicy policy)
{
	if (start > end)
		return;
	if (policy == ExecutionPolicy::APARALLEL)
	{
		tbb::parallel_for(start, end, func);
	}
	else
	{
		for (auto i = start; i < end; ++i)
			func(i);
	}
}

template <typename Function>
void parallelFor(size_t start, size_t end, size_t grainSize, const Function& func, ExecutionPolicy policy)
{
	if (start > end)
		return;
	if (policy == ExecutionPolicy::APARALLEL)
	{
		tbb::parallel_for(tbb::blocked_range<size_t>(start, end, grainSize),
			func, tbb::simple_partitioner());
	}
	else
	{
		tbb::blocked_range<size_t> range(start, end, grainSize);
		func(range);
	}
}

inline int numSystemCores() { return std::max(1u, std::thread::hardware_concurrency()); }

RENDER_END