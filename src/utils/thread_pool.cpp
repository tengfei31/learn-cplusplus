#include "utils/thread_pool.h"

ThreadPool::ThreadPool(int poolSize):poolSize(poolSize), stop(false) {
    worker_threads.reserve(poolSize);
    int i = 0;
    for (; i < poolSize; i++) {
        //worker_threads[i] = (workThread);
        worker_threads.emplace_back(workThread);
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

//添加任务
template<typename F, typename ...Args>
auto ThreadPool::enqueue(F&& func, Args&&... args) -> std::future<typename std::invoke_result_t<Func(Args...)>> {
    using return_type = typename std::invoke_result_t<Func(Args...)>;
    auto task = std::make_shared<std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(func), std::forward<Args>(args)...)
    );
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(this->mtx);
        if (stop) {
            //std::cerr << "threadPool is stoped" << std::endl;
            //return std::future<return_type>();
            throw std::runtime_error("threadPool is stoped");
        }
        tasks_queue.emplace([task](){(*task)()});
    }
    cv.notify_one();

    return res;
}

//添加任务
template<typename F, typename ...Args>
void ThreadPool::addTask(F func, Args... args) {
    std::unique_lock<std::mutex> lock(this->mtx);
    if (stop) {
        std::cerr << "threadPool is stoped" << std::endl;
        return;
    }
    tasks_queue.emplace(func);
    cv.notify_one();
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