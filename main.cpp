#include <iostream>
#include <thread>
#include "utils/logger.h"

extern void test_coroutine();
extern void schedule();

Logger logger;
int main()
{
    std::cout << "Hello, World!" << std::endl;
    test_coroutine();

    //协程调度
    schedule();
    return 0;
}