﻿#include "Parallel.h"

namespace Rendering
{
	// 线程列表
	static std::vector<std::thread> threads;
	// 
	static bool shutdownThreads = false;

	static ParallelForLoop* workList = nullptr;
	static std::mutex workListMutex;

	static std::atomic<bool> reportWorkerStats{ false };

	static std::atomic<int> reporterCount;

	static std::condition_variable reportDoneCondition;
	static std::mutex reportDoneMutex;
	static int nThread = 0;
	thread_local int ThreadIndex = 0;

	static std::condition_variable workListCondition;

	int getCurThreadIndex() {
		return ThreadIndex;
	}

	void parallelFor(std::function<void(int64_t)> func, int64_t count, int chunkSize) {
		DCHECK(threads.size() > 0 || maxThreadIndex() == 1);

		if (threads.empty() || count < chunkSize) {
			for (int64_t i = 0; i < count; ++i) {
				func(i);
			}
			return;
		}

		ParallelForLoop loop(std::move(func), count, chunkSize, CurrentProfilerState());
		{
			std::lock_guard<std::mutex> lock(workListMutex);
			loop.next = workList;
			workList = &loop;
		}

		std::unique_lock<std::mutex> lock(workListMutex);
		workListCondition.notify_all();

		while (!loop.finished()) {
			int64_t indexStart = loop.nextIndex;
			int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

			loop.nextIndex = indexEnd;

			if (loop.nextIndex == loop.maxIndex) {
				workList = loop.next;
			}

			++loop.activeWorkers;
			lock.unlock();
			// 执行[indexStart, indexEnd)区间内的索引
			for (int64_t index = indexStart; index < indexEnd; ++index) {
				uint64_t oldState = ProfilerState;
				ProfilerState = loop.profilerState;
				if (loop.func1D) {
					loop.func1D(index);
				}
				else {
					loop.func2D(Point2i(index % loop.numX, index / loop.numX), 0);
				}
				ProfilerState = oldState;
			}
			lock.lock();
			--loop.activeWorkers;
		}
	}

	void parallelFor2D(std::function<void(Point2i, int)> func, const Point2i& count) {
		DCHECK(threads.size() > 0 || maxThreadIndex() == 1);

		if (threads.empty() || count.x * count.y <= 1) {
			for (int64_t y = 0; y < count.y; ++y) {
				for (int64_t x = 0; x < count.x; ++x) {
					func(Point2i(x, y), 0);
				}
			}
			return;
		}

		ParallelForLoop loop(std::move(func), count, CurrentProfilerState());
		{
			std::lock_guard<std::mutex> lock(workListMutex);
			loop.next = workList;
			workList = &loop;
		}

		std::unique_lock<std::mutex> lock(workListMutex);
		workListCondition.notify_all();

		// 一直循环，直到所有线程都执行完毕之后才结束
		// 保证了线程的同步
		ThreadIndex = 0;
		while (!loop.finished()) {
			int64_t indexStart = loop.nextIndex;
			int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

			loop.nextIndex = indexEnd;

			if (loop.nextIndex == loop.maxIndex) {
				workList = loop.next;
			}

			++loop.activeWorkers;
			lock.unlock();
			// 执行[indexStart, indexEnd)区间内的索引
			for (int64_t index = indexStart; index < indexEnd; ++index) {
				uint64_t oldState = ProfilerState;
				ProfilerState = loop.profilerState;
				if (loop.func1D) {
					loop.func1D(index);
				}
				else {
					loop.func2D(Point2i(index % loop.numX, index / loop.numX), 0);
				}
				ProfilerState = oldState;
			}
			lock.lock();
			--loop.activeWorkers;
		}
	}

	static void workerThreadFunc(int tIndex, std::shared_ptr<Barrier> barrier) {
		ThreadIndex = tIndex;

		ProfilerWorkerThreadInit();

		// 等待，最后一个子线程调用此函数之后，全部子线程同时开始往下执行
		barrier->wait();

		//每个线程各自释放掉barrier对象
		barrier.reset();

		// 以下逻辑涉及线程同步问题，上锁
		std::unique_lock<std::mutex> lock(workListMutex);
		while (!shutdownThreads) {
			if (reportWorkerStats) {
				ReportThreadStats();
				if (--reporterCount == 0) {
					reportDoneCondition.notify_one();
				}
				workListCondition.wait(lock);
			}
			else if (!workList) {
				// 如果没有任务需要执行，则等待
				workListCondition.wait(lock);
			}
			else {
				ParallelForLoop& loop = *workList;

				int64_t indexStart = loop.nextIndex;
				int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

				// 这个循环结束的索引就是下个循环的开始，
				loop.nextIndex = indexEnd;
				if (loop.nextIndex == loop.maxIndex) {
					// 如果当前loop已经执行完毕，则执行下一个loop
					workList = loop.next;
				}

				++loop.activeWorkers;
				lock.unlock();
				// 执行[indexStart, indexEnd)区间内的索引
				for (int64_t index = indexStart; index < indexEnd; ++index) {
					uint64_t oldState = ProfilerState;
					ProfilerState = loop.profilerState;
					if (loop.func1D) {
						loop.func1D(index);
					}
					else {
						loop.func2D(Point2i(index % loop.numX, index / loop.numX), ThreadIndex);
					}
					ProfilerState = oldState;
				}
				lock.lock();
				--loop.activeWorkers;
				if (loop.finished()) {
					// 因为其他线程可能处于wait状态
					// 如果loop执行完毕，则唤醒所有线程
					workListCondition.notify_all();
				}
			}
		}
	}

	void setThreadNum(int num) {
		nThread = num;
	}

	int getThreadNum() {
		return maxThreadIndex() + 1;
	}

	int maxThreadIndex() {
		return nThread == 0 ? numSystemCores() : nThread;
	}

	void parallelInit(int num) {
		CHECK_EQ(threads.size(), 0);
		setThreadNum(num);
		int nThreads = maxThreadIndex();
		ThreadIndex = 0;

		std::shared_ptr<Barrier> barrier = std::make_shared<Barrier>(nThreads);

		for (int i = 0; i < nThreads - 1; ++i) {
			threads.push_back(std::thread(workerThreadFunc, i + 1, barrier));
		}

		barrier->wait();
	}

	void parallelCleanup() {
		if (threads.empty()) {
			return;
		}
		{
			std::lock_guard<std::mutex> lock(workListMutex);
			shutdownThreads = true;
			workListCondition.notify_all();
		}
		for (std::thread& thread : threads) {
			thread.join();
		}
		threads.erase(threads.begin(), threads.end());
		shutdownThreads = false;
	}

	void mergeWorkerThreadStats() {
		std::unique_lock<std::mutex> lock(workListMutex);
		std::unique_lock<std::mutex> doneLock(reportDoneMutex);
		// Set up state so that the worker threads will know that we would like
		// them to report their thread-specific stats when they wake up.
		reportWorkerStats = true;
		reporterCount = threads.size();

		// Wake up the worker threads.
		workListCondition.notify_all();

		// Wait for all of them to merge their stats.
		reportDoneCondition.wait(lock, []() { return reporterCount == 0; });

		reportWorkerStats = false;
	}
}