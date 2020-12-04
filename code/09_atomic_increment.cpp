// g++ -O2 -std=c++17 -pthread 09_atomic_increment.cpp
//

#include <atomic>    // std::atomic
#include <iostream>  // std::cout
#include <thread>    // std::thread

#define LOOPS 100000

std::atomic<int> a;
volatile int v;

void increment_atomic(std::atomic<int>& a)
{
    for (int i = 0; i < LOOPS; ++i) {
        ++a;
    }
}

void increment_volatile(volatile int& n)
{
    for (int i = 0; i < LOOPS; ++i) {
        ++n;
    }
}

int main()
{
    {
        std::cout << "*** Incrementing on the same atomic int\n";
        std::thread t1{increment_atomic, std::ref(a)};
        std::thread t2{increment_atomic, std::ref(a)};
        t1.join();
        t2.join();
        std::cout << "Result is " << a << std::endl;
    }
    {
        std::cout << "*** Incrementing on the same volatile int\n";
        std::thread t1{increment_volatile, std::ref(v)};
        std::thread t2{increment_volatile, std::ref(v)};
        t1.join();
        t2.join();
        std::cout << "Result is " << v << std::endl;
    }
}
