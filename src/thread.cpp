#include <functional>
#include <iostream>
#include <ostream>
#include <thread>

static std::atomic<int> count = 0;

void work(){
    for (int i = 0; i < 1000000; i++) count+=1;
}


int test_thread(){
    std::thread t1(work);
    std::thread t2(work);
    t1.join();
    t2.join();

    std::cout << "count=" << count << std::endl;

    return 0;
}
