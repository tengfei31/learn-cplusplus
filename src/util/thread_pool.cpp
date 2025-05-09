#include <thread>
#include <queue>
#include <vector>


class ThreadPool {
    public:
    ThreadPool(int poolSize):poolSize(poolSize) {
        threads.reserve(poolSize);
        int i = 0;
        for (; i < poolSize; i++) {
            threads[i] = std::thread([this](){
                while(true) {
                    std::function<void()> task;
                    tasks.front();
                    tasks.pop();

                    task();
                };
            });
        }
    }

    private:
    int poolSize;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
};