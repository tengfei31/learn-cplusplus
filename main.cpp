#include <iostream>
#include <thread>
#include "utils/logger.h"

extern void test_coroutine();

Logger logger;
int main()
{
    std::cout << "Hello, World!" << std::endl;
    test_coroutine();


    // 延迟确保日志写入
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}