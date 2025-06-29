#include "cpp_inc/coroutine_default_sche.h"
#include <thread>
#include <iostream>
#include "utils/logger.h"

extern Logger logger;

//这是一个全局的协程调度器
util::CoroutineDefaultSche sche;

//协程处理函数，单个任务
util::CoroutinePromise<void> handle(int params)
{
    //处理逻辑
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //模拟逻辑耗时
    co_await sche.sleepTimeout(params * 1000);
    // std::cout << "coroutine logic: " << params << std::endl;
    LOG(INFO) << "coroutine logic: " << params;
    co_return;
}

//测试C++20协程
void test_coroutine()
{

    // std::thread t([](){

        int i = 0;
        for (; i < 5; i++) {
            //注册协程任务
            // sche.readyExec(std::bind([](int params) -> util::CoroutinePromise<void>  {
            //     co_await handle(params);
            // }, i));
            sche.readyExec([params = i]() -> util::CoroutinePromise<void> {
                co_await handle(params);
            });
            LOG(INFO) << "readyExec after: " << i;
        };
        //调度协程
        while(sche.doSche(2000) == util::CoroutineDefaultSche::ST_RUN) {
            LOG(INFO) << "CoroutineDefaultSche";
            logger.Flush();
        };
    // });
    // t.join();


}

