// g++ -O2 -std=c++17 -pthread 06_measure_overhead.cpp profiler.cpp
//

#include <chrono>      // std::chrono::high_resolution_clock
#include <iostream>    // std::cout
#include <mutex>       // std::mutex
#include <stdint.h>    // uint64_t
#include "profiler.h"  // PROFILE_CHECK
#include "rdtsc.h"     // rdtsc

#define LOOPS 1000

uint64_t now;
std::mutex mutex;

uint64_t get_time_high_resolution_clock()
{
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               now.time_since_epoch())
        .count();
}

void measure_clock_overhead_batch()
{
    auto t1 = rdtsc();
    for (int i = 0; i < LOOPS; ++i) {
        now = get_time_high_resolution_clock();
    }
    auto t2 = rdtsc();
    std::cout << "high_resolution_clock takes " << (t2 - t1) / LOOPS
              << " cycles on average" << std::endl;
}

enum profiled_functions {
    PF_MEASURE_CLOCK_OVERHEAD_SINGLE,
};

name_mapper name_map[] = {
    {PF_MEASURE_CLOCK_OVERHEAD_SINGLE, "measure_clock_overhead_single"},
    {-1, NULL}};

void measure_clock_overhead_single()
{
    PROFILE_CHECK(PF_MEASURE_CLOCK_OVERHEAD_SINGLE);
    now = get_time_high_resolution_clock();
}

int main()
{
    // Warm up the cache
    now = get_time_high_resolution_clock();

    measure_clock_overhead_batch();
    for (int i = 0; i < LOOPS; ++i) {
        std::lock_guard guard{mutex};
        measure_clock_overhead_single();
    }
}
