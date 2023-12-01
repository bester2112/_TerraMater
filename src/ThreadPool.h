#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <condition_variable>
#include <functional>
#include <queue>

class ThreadPool {
public:
	static ThreadPool &getInstance();

	ThreadPool(ThreadPool const &) = delete;
	void operator=(ThreadPool const &) = delete;

	void submit(std::function<void()> task);

	void start();
	void stop();

private:
	ThreadPool() {}
	~ThreadPool() {}

	int threadCount = std::thread::hardware_concurrency() - 2;

	std::condition_variable eventHappened;

	std::mutex eventMutex;

	bool stopped = false;

	std::vector<std::thread> threads;

	std::queue<std::function<void()>> tasks;
};

#endif // !THREADPOOL_H
