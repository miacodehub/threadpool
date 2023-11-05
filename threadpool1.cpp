//gather threads from cpu
// reassign tasks based how much each core is occupied .. um i dont wanna fuck up my computer .. so maybe just a simulation?-- so some kind of performance optimization algorithm


/*
::pseudocode::
get number of cores on a machine - multi 
get threads on a particular process?
manage them to create better efficiency
...
create a simulator app basically that does a tiny bit of performance optimizations and displays what could be .. and there's gotta be some way to constantly update the result
*/
#include <iostream>
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>

class ThreadPool {
  private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks; // whats that in the middle?
    std::mutex queueMutex;// whats this do?
    bool stop = false;
  public:
    ThreadPool(int numThreads) {
      for(int i=0; i<numThreads; ++i){
        threads.emplace_back([this] {
          while(true) {
            std::function<void()> task;
            {
              std::unique_lock<std::mutex> lock(queueMutex);
              if(stop && tasks.empty()) {
                return;
              }
              /*
              ^^^^^^
              ||||||
              within the locked section, thread checks if it should exit.. if the stop is set to true and the task queue is empty, thread exits 
              ------------------
              */
              if(!tasks.empty()){
                task = tasks.front();
                tasks.pop();
              }
              /*
              get a task //
              */
            } // lock is released here
            if(task){
              task(); // if there's a valid task, perform it..
            }
          }
        }

        );
      }
    }
// below: we define a template function "enqueue" to add tasks to the thread pool.. it takes a callable object f (like a function or lambda) as its argument
    template <class F>
    void enqueue(F&& f) {
      {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(std::forward<F>(f));
      }
    }

    ~ThreadPool() {
      {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop=true;
      }
      for(std::thread& worker : threads) {
        worker.join();// wtf? 
      }
    }
};

int main() {
  ThreadPool pool(4); // creates 4 threads?
  for(int i=0; i<8; ++i){
    pool.enqueue([i] {
      std::cout << "task " << i << " executed by thread" << std::this_thread::get_id() << std::endl;
    }

    );
  }
  return 0;
}
/*
ahem.. what does this do?
> what does <functional> do?
> emplace_back?
> this: captures current instance of the "threadpool" class to access its members
> while (true) // starts an infinite loop
> std::function<void()> task // can hold any callable object like a function that takes no arguments and returns nothing 
> std::unique_lock<std::mutex> lock(queueMutex) // ensures only one thread accesses the task queue
> what is worker.join() for a thread& !?!?!??!?!?? ensures that the destrcutor does not return until all threads have completed their tasks and terminated
>  .// no
how are we filling this task queue?



*/


/*
what if each thread handled a tiny task instead of just printing shit
like 
parallelism in multicore cpu
*/













