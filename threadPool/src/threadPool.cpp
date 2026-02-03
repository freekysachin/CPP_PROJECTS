#include "threadPool.h"

ThreadPool::ThreadPool(int qsize) : pool_size(qsize){}
ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    cvTP.notify_all();
    for(std::thread &worker : workers){
        if(worker.joinable())
            worker.join();
    }
}

void ThreadPool::init(){
    for(int i =0; i<pool_size; i++){
        workers.emplace_back([this]{ this->workerThread(); });
    }
}

void ThreadPool::enqueueTask(std::function<void()> &&cb){
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        TPqueue.push(std::move(cb));
    }
    cvTP.notify_one(); // wake up one worker
}

void ThreadPool::workerThread(){ 
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(20));
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            // Wait until stop is signaled OR the queue is not empty
            // This prevents CPU spin-locking (High Efficiency)
            cvTP.wait(lock, [this]{
                return this->stop || !this->TPqueue.empty();
            });

            if(this->stop && this->TPqueue.empty()) return;
            task = std::move(TPqueue.front());
            TPqueue.pop();
        }
        if(task) task();
    }
    
}
