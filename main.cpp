#include <iostream>
#include "utils/logger.h"

extern void test_coroutine();

int main()
{

    InitLogging();
    std::cout << "Hello, World!" << std::endl;
    test_coroutine();
    return 0;
}