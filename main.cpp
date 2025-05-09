#include <iostream>

extern void test_coroutine();

int main()
{
    std::cout << "Hello, World!" << std::endl;
    test_coroutine();
    return 0;
}