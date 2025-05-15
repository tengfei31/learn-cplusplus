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
    auto enqueue(F&& func, Args&&... args) -> std::future<typename std::invoke_result_t<Func(Args...)>>;
    //添加任务
    template<typename F, typename ...Args>
    void addTask(F func, Args... args);
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