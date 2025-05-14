#include <thread>
#include <queue>
#include <vector>
#include <condition_variable>
#include <iostream>
#include <future>
#include <functional>
#include <mutex>


class ThreadPool {
    public:
    typedef std::function<void()> Func;
    //using Func = std::function<void()>;

    ThreadPool(int poolSize):poolSize(poolSize), stop(false) {
        worker_threads.reserve(poolSize);
        int i = 0;
        for (; i < poolSize; i++) {
            //worker_threads[i] = (workThread);
            worker_threads.emplace_back(workThread);
        }
    }

    ~ThreadPool() {
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
    auto enqueue(F&& func, Args&&... args) -> std::future<typename std::invoke_result_t<Func(Args...)>> {
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
    void addTask(F func, Args... args) {
        std::unique_lock<std::mutex> lock(this->mtx);
        if (stop) {
            std::cerr << "threadPool is stoped" << std::endl;
            return;
        }
        tasks_queue.emplace(func);
        cv.notify_one();
    }

    private:

    //单个worker线程
    void workThread() {
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

    private:
    //线程池是否停止
    bool stop;
    //线程池大小
    int poolSize;
    std::mutex mtx;
    std::condition_variable cv;
    //线程池
    std::vector<std::thread> worker_threads;
    //要运行的任务队列
    std::queue<Func> tasks_queue;
};