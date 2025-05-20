#include <thread>
#include <queue>
#include <vector>
#include <iostream>
#include <functional>
#include <mutex>
#include <future>
#include <condition_variable>

//线程池
class ThreadPool {
public:
    typedef std::function<void()> Func;
    //using Func = std::function<void()>;
    ThreadPool(int poolSize);
    virtual ~ThreadPool();
    //添加任务
    template<typename F, typename ...Args>
    auto enqueue(F&& func, Args&&... args) -> std::future<typename std::invoke_result_t<F(Args...)>> {
        using return_type = typename std::invoke_result_t<F(Args...)>;
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
            tasks_queue.emplace([task, args...](){(*task)(args...);});
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
        tasks_queue.emplace([func, args...](){
            func(args...);
        });
        cv.notify_one();
    }
private:
    //单个worker线程
    void workThread();

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