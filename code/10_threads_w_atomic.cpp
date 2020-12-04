// g++ -O2 -std=c++17 -pthread 10_threads_w_atomic.cpp
//

#include <atomic>    // std::atomic
#include <iostream>  // std::cout
#include <mutex>     // std::mutex
#include <thread>    // std::thread
#include "rdtsc.h"   // rdtsc

#define LOOPS 10000

std::atomic<int> a1;
std::atomic<int> a2;
volatile int v1;
volatile int v2;
int i1;
int i2;
std::mutex inc_lock;
std::mutex output_lock;

void increment_atomic(std::atomic<int>& a)
{
    auto t1 = rdtsc();
    for (int i = 0; i < LOOPS; ++i) {
        ++a;
    }
    auto t2 = rdtsc();
    std::lock_guard guard{output_lock};
    std::cout << "increment_atomic: " << (t2 - t1) / LOOPS
              << " cycles on average\n";
}

void increment_volatile(volatile int& n)
{
    auto t1 = rdtsc();
    for (int i = 0; i < LOOPS; ++i) {
        ++n;
    }
    auto t2 = rdtsc();
    std::lock_guard guard{output_lock};
    std::cout << "increment_volatile: " << (t2 - t1) / LOOPS
              << " cycles on average\n";
}

void increment_with_lock(int& n)
{
    auto t1 = rdtsc();
    for (int i = 0; i < LOOPS; ++i) {
        std::lock_guard guard{inc_lock};
        ++n;
    }
    auto t2 = rdtsc();
    std::lock_guard guard{output_lock};
    std::cout << "increment_with_lock: " << (t2 - t1) / LOOPS
              << " cycles on average\n";
}

int main()
{
    {
        std::cout << "*** Testing with one increment_atomic thread\n";
        std::thread t{increment_atomic, std::ref(a1)};
        t.join();
    }
    {
        std::cout << "*** Testing with two increment_atomic threads\n";
        std::thread t1{increment_atomic, std::ref(a1)};
        std::thread t2{increment_atomic, std::ref(a2)};
        t1.join();
        t2.join();
    }
    {
        std::cout << "*** Testing with one increment_volatile thread\n";
        std::thread t{increment_volatile, std::ref(v1)};
        t.join();
    }
    {
        std::cout << "*** Testing with two increment_volatile threads\n";
        std::thread t1{increment_volatile, std::ref(v1)};
        std::thread t2{increment_volatile, std::ref(v2)};
        t1.join();
        t2.join();
    }
    {
        std::cout << "*** Testing with one increment_volatile thread and "
                     "one increment_atomic thread\n";
        std::thread t1{increment_volatile, std::ref(v1)};
        std::thread t2{increment_atomic, std::ref(a2)};
        t1.join();
        t2.join();
    }
    {
        std::cout << "*** Testing with one increment_with_lock thread\n";
        std::thread t{increment_with_lock, std::ref(i1)};
        t.join();
    }
    {
        std::cout << "*** Testing with two increment_with_lock threads\n";
        std::thread t1{increment_with_lock, std::ref(i1)};
        std::thread t2{increment_with_lock, std::ref(i2)};
        t1.join();
        t2.join();
    }
}
