// cl /EHsc /O2 /std:c++17 05_test_windows_timer.cpp
//

#include <chrono>           // std clocks
#include <iostream>         // std::cout
#include <intrin.h>         // __rdtsc
#include <stdint.h>         // uint64_t
#include <time.h>           // clock
#include <windows.h>        // Windows clock functions
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

#if defined(_WIN32) && !defined(_WIN64)
template <typename T>
void measure_accuracy(T (__stdcall *fn)(), const char* unit)
{
    unsigned count = 0;
    clock_t last;
    clock_t next;
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
#endif

ULONGLONG GetPerformanceCounter()
{
    LARGE_INTEGER result;
    QueryPerformanceCounter(&result);
    return result.QuadPart;
}

ULONGLONG GetSystemTime()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return ft.dwHighDateTime * 10'000'000 + ft.dwLowDateTime;
}

ULONGLONG GetSystemTimePrecise()
{
    FILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);
    return ft.dwHighDateTime * 10'000'000 + ft.dwLowDateTime;
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
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    std::cout << "Performance frequency is " << freq.QuadPart << std::endl;

    const char* clock_unit = [] {
        if (CLOCKS_PER_SEC == 1'000'000) {
            return "us";
        } else if (CLOCKS_PER_SEC == 1000) {
            return "ms";
        } else {
            std::cerr << "Unexpected CLOCKS_PER_SEC value" << std::endl;
            exit(1);
        }
    }();
    const char* performance_counter_unit = [&freq] {
        if (freq.QuadPart == 10'000'000) {
            return "(100 ns)";
        } else {
            return "(unknown, see performance frequency above)";
        }
    }();

    TEST(clock_t, clock, clock_unit);
    TEST(DWORD, GetTickCount, "ms");
    TEST(ULONGLONG, GetPerformanceCounter, performance_counter_unit);
    TEST(ULONGLONG, GetSystemTime, "(100 ns)");
    TEST(ULONGLONG, GetSystemTimePrecise, "(100 ns)");
    TEST(uint64_t, get_time_system_clock, "ns");
    TEST(uint64_t, get_time_steady_clock, "ns");
    TEST(uint64_t, get_time_high_resolution_clock, "ns");
    TEST(uint64_t, rdtsc, "cycles");
}
