#include "cpp_inc/coroutine_default_sche.h"
#include <thread>
#include <iostream>
#include <glog/logging.h>

void SetupLogging(const std::string& logDir)
{
    // 设置日志输出目录
    FLAGS_log_dir = logDir;
    // google::SetLogDestination(google::INFO, "./logs/my_log_");

    // 设置日志级别
    FLAGS_stderrthreshold = google::INFO;

    // 设置日志文件大小为10MB
    FLAGS_max_log_size = 10 * 1024;  // 10MB

    // 禁用日志文件名中的机器名后缀
    FLAGS_alsologtostderr = true;
    FLAGS_logtostderr = false;

    // 自动移除旧日志 day （apt旧版本没有）
    // google::EnableLogCleaner(3);
}

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
    // 设置日志目录
    std::string logDir = "./logs";

    // 初始化日志
    SetupLogging(logDir);

    // 初始化 Glog
    google::InitGoogleLogging("./logs");

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
            // std::cout << "readyExec after: " << i << std::endl;
            LOG(INFO) << "readyExec after: " << i << std::endl;
        };
        //调度协程
        while(sche.doSche(2000) == util::CoroutineDefaultSche::ST_RUN);
    // });
    // t.join();


}

