#include "ThreadPool.h"

#include <thread>
#include <iostream>

ThreadPool &ThreadPool::getInstance() {
	static ThreadPool instance;

	return instance;
}

void ThreadPool::submit(std::function<void()> task) {
	{
		std::unique_lock<std::mutex> lock(eventMutex);
		tasks.emplace(std::move(task));
	}

	eventHappened.notify_one();
}

void ThreadPool::start() {
	std::cout << "Thread count: " << threadCount << std::endl;

	for (int i = 0; i < threadCount; i++) {
		threads.emplace_back([this] {
			while (true) {
				std::function<void()> task;

				{
					std::unique_lock<std::mutex> lock(eventMutex);

					eventHappened.wait(lock, [this] {return stopped || !tasks.empty(); });

					if (stopped) {
						break;
					}

					task = std::move(tasks.front());
					tasks.pop();
				}

				task();
			}
			});
	}
}

void ThreadPool::stop() {
	{
		std::unique_lock<std::mutex> lock{ eventMutex };
		stopped = true;
	}

	eventHappened.notify_all();

	for (auto &thread : threads) {
		thread.join();
	}
}
