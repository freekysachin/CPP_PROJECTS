#pragma once

#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>

class ThreadPool
{
private:

    std::atomic<bool> stop;

    int pool_size = 4;
    std::mutex queue_mutex;
    std::condition_variable cvTP;
    std::queue<std::function<void()>> TPqueue;
    
    std::vector<std::thread> workers;
    void workerThread();
public:
    ThreadPool(/* args */) = default;
    explicit ThreadPool(int qsize);

    ~ThreadPool();

    void init();
    void enqueueTask(std::function<void()> &&cb);
};

