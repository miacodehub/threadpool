// thread pool theory:
/*
	definition: it's a software design pattern that helps a computer achieve concurrency
		- a collection of worker threads that efficiently execute asynchronous callbacks on behalf of the application

	worker threads - a pool of threads waiting to execute tasks
	task queue - a queue where tasks (functions or functors) are placed before execution
	synchronization - proper handling of shared resources with locks and condition variables
	
	threadpool architecture :
	
	-threadpool initialization-
	spawning a thread -

	-task submission-
	callable - 
		* function
		* lambda
		* functor
		* std::packaged_task - 
		* std::future - 

	-worker threads-
		* std::condition_variable

	-synchronization-
		* std::mutex
		* std::condition_variable
		 
*/

#include <iostream>
#include <vector>
#include<string>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <filesystem>



using namespace std;

namespace fs = std::filesystem;


class ThreadPool {
public:
	ThreadPool(size_t threads);
	~ThreadPool();
	void enqueue(const fs::path& srcPath, const fs::path& destPath);
	

private:
	vector<thread> workers;
	queue<pair<fs::path,fs::path>> tasks;
	mutex queueMutex;
	condition_variable condition;
	bool stop;

	void worker();

};


//Threadpool constructor

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
	for (size_t i = 0; i < numThreads; ++i) {
		workers.emplace_back([this] {worker(); });
	}
}
ThreadPool::~ThreadPool() {
	{
		std::unique_lock<mutex> lock(queueMutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread& worker : workers) {
		worker.join();
	}
}

void ThreadPool::enqueue(const fs::path& srcPath, const fs::path& destPath) {
	{
		unique_lock<mutex> lock(queueMutex);
		tasks.emplace(srcPath, destPath);
	}
	condition.notify_one();
}

void ThreadPool::worker() {
	while (true) {
		pair<fs::path, fs::path> task;
		{
			unique_lock<mutex> lock(queueMutex);
			condition.wait(lock, [this] {return stop || !tasks.empty(); });
			if (stop && tasks.empty())
				return;
			task = std::move(tasks.front());
			tasks.pop();
		}
		// file copy operation
		std::ifstream srcFile(task.first, std::ios::binary);
		std::ofstream destFile(task.second, std::ios::binary);
		destFile << srcFile.rdbuf(); // copy contents
		std::cout << "copied " << task.first << "to " << task.second << endl;

	}
}

int main() {
	const size_t numThreads = 4;
	ThreadPool pool(numThreads);
	
	const std::string& sourceDir = "C:/Users/ramya/Desktop/source";
	const std::string& destDir = "C:/Users/ramya/Desktop/destdir";

	//iterate over all the files in the source directory
	for (const auto& entry : fs::directory_iterator(sourceDir)) {
		if (entry.is_regular_file()) {
			fs::path destPath = fs::path(destDir) / entry.path().filename();
			pool.enqueue(entry.path(), destPath); // enqueue file copy tasks
		}
	}
	//give threads time to finish
	std::this_thread::sleep_for(std::chrono::seconds(2));

	return 0;
}
/*


*/