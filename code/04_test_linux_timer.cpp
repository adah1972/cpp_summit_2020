// g++ -O2 -std=c++17 04_test_linux_timer.cpp
//

#include <chrono>           // std clocks
#include <iostream>         // std::cout
#include <stdint.h>         // uint64_t
#include <time.h>           // clock/clock_gettime
#include <x86intrin.h>      // __rdtsc
#include <sys/time.h>       // gettimeofday
#include "stats_counter.h"  // stats_counter

template <typename T>
void measure_accuracy(T (*fn)(), const char* unit)
{
    unsigned count = 0;
    T last;
    T next;
    const int times = 100;
    stats_counter<uint64_t> counter{times};
    for (last = fn(); count < times; last = next) {
        next = fn();
        if (next != last) {
            ++count;
            counter.add(next - last);
        }
    }
    counter.print_result(std::cout, unit);

    auto r1 = __rdtsc();
    for (int i = 0; i < times; ++i) {
        fn();
    }
    auto r2 = __rdtsc();
    std::cout << "Average call duration is " << (r2 - r1) * 1.0 / times
              << " cycles" << std::endl;

    std::cout << std::endl;
}

uint64_t get_time_of_day()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1'000'000 + tv.tv_usec;
}

uint64_t get_time_monotonic_raw()
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return ts.tv_sec * 1'000'000'000 + ts.tv_nsec;
}

uint64_t get_time_system_clock()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               now.time_since_epoch())
        .count();
}

uint64_t get_time_steady_clock()
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               now.time_since_epoch())
        .count();
}

uint64_t get_time_high_resolution_clock()
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               now.time_since_epoch())
        .count();
}

uint64_t rdtsc()
{
    return __rdtsc();
}

#define TEST(type, func, unit) \
    std::cout << "*** Testing accuracy of " #func "\n"; \
    measure_accuracy<type>(func, unit)

int main()
{
    const char* clock_unit = [] {
        if (CLOCKS_PER_SEC == 1'000'000) {
            return "µs";
        } else if (CLOCKS_PER_SEC == 1000) {
            return "ms";
        } else {
            std::cerr << "Unexpected CLOCKS_PER_SEC value" << std::endl;
            exit(1);
        }
    }();
    TEST(clock_t, clock, clock_unit);
    TEST(uint64_t, get_time_of_day, "µs");
    TEST(uint64_t, get_time_monotonic_raw, "ns");
    TEST(uint64_t, get_time_system_clock, "ns");
    TEST(uint64_t, get_time_steady_clock, "ns");
    TEST(uint64_t, get_time_high_resolution_clock, "ns");
    TEST(uint64_t, rdtsc, "cycles");
}
