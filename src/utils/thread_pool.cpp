#include "utils/thread_pool.h"

ThreadPool::ThreadPool(int poolSize):poolSize(poolSize), stop(false) {
    worker_threads.reserve(poolSize);
    int i = 0;
    for (; i < poolSize; i++) {
        //worker_threads[i] = workThread;
        worker_threads.emplace_back(worker_threads);
        worker_threads.emplace_back([this](){
            while(true) {
                Func task;
                {
                    std::unique_lock<std::mutex> lock(this->mtx);
                    cv.wait(lock, [this]()->bool{return this->stop || !this->tasks_queue.empty();});
                    if (this->stop || this->tasks_queue.empty()) {
                        return;
                    }
                    task = std::move(this->tasks_queue.front());
                    this->tasks_queue.pop();
                }
                try {
                    task();
                } catch (std::exception& e) {
                    std::cerr << "Exception in worker thread: " << e.what() << std::endl;
                }
            };
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        //这里局部，是为了离开作用域自动调用unlock()
        std::unique_lock<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (int i = 0; i < poolSize; i++) {
        worker_threads[i].join();
    }
}

//单个worker线程
void ThreadPool::workThread() {
    while(true) {
        Func task;
        {
            std::unique_lock<std::mutex> lock(this->mtx);
            cv.wait(lock, [this]()->bool{return this->stop || !this->tasks_queue.empty();});
            if (this->stop || this->tasks_queue.empty()) {
                return;
            }
            task = std::move(this->tasks_queue.front());
            this->tasks_queue.pop();
        }
        try {
            task();
        } catch (std::exception& e) {
            std::cerr << "Exception in worker thread: " << e.what() << std::endl;
        }
    };
}