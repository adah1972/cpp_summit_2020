// g++ -O2 -std=c++17 -pthread 11_cache_contention.cpp
//

#include <iostream>  // std::cout/endl
#include <mutex>     // std::mutex
#include <utility>   // std::swap
#include <stddef.h>  // size_t
#include "rdtsc.h"   // rdtsc

constexpr int LOOPS = 1000;
void* result;
std::mutex mutex;

template <size_t N>
void transpose1(double (&a)[N][N])
{
    size_t r, c;
    for (r = 1; r < N; r++) {
        for (c = 0; c < r; c++) {
            std::swap(a[r][c], a[c][r]);
        }
    }
}

template <size_t N>
void transpose2(double a[N][N])
{
    if (N > 64 && N % 64 == 0) {
        // Cache contentions expected. Use square blocking.
        size_t r1, r2, c1, c2;
        constexpr size_t TILESIZE = 8;  // N must be divisible by TILESIZE
        for (r1 = 0; r1 < N; r1 += TILESIZE) {
            for (c1 = 0; c1 < r1; c1 += TILESIZE) {
                for (r2 = r1; r2 < r1+TILESIZE; r2++) {
                   for (c2 = c1; c2 < c1+TILESIZE; c2++) {
                       std::swap(a[r2][c2],a[c2][r2]);
                   }
                }
             }
             // The diagonal is handled separately.
             for (r2 = r1 + 1; r2 < r1 + TILESIZE; r2++) {
                 for (c2 = r1; c2 < r2; c2++) {
                     std::swap(a[r2][c2], a[c2][r2]);
                 }
             }
        }
    } else {
        // No cache contentions. Use simple method.
        size_t r, c;
        for (r = 1; r < N; r++) {
            for (c = 0; c < r; c++) {
                std::swap(a[r][c], a[c][r]);
            }
        }
    }
}

template <size_t N>
void test1()
{
    alignas(64) double matrix[N][N];
    auto t1 = rdtsc();
    for (int i = 0; i < LOOPS; ++i) {
        std::lock_guard<std::mutex> guard{mutex};
        transpose1(matrix);
        result = static_cast<void*>(matrix);
    }
    auto t2 = rdtsc();
    std::cout << N << " (" << sizeof matrix << " bytes): "
              << (t2 - t1) * 1.0 / ((N - 1) * (N - 1) * LOOPS) << std::endl;
}

template <size_t N>
void test2()
{
    alignas(64) double matrix[N][N];
    auto t1 = rdtsc();
    for (int i = 0; i < LOOPS; ++i) {
        std::lock_guard<std::mutex> guard{mutex};
        transpose2(matrix);
        result = static_cast<void*>(matrix);
    }
    auto t2 = rdtsc();
    std::cout << N << " (" << sizeof matrix << " bytes): "
              << (t2 - t1) * 1.0 / ((N - 1) * (N - 1) * LOOPS) << std::endl;
}

int main()
{
    test1<63>();
    test1<64>();
    test1<65>();

    test1<127>();
    test1<128>();
    test1<129>();

    test1<255>();
    test1<256>();
    test1<257>();

    test1<511>();
    test1<512>();
    test1<513>();

    test2<63>();
    test2<64>();
    test2<65>();

    test2<127>();
    test2<128>();
    test2<129>();

    test2<255>();
    test2<256>();
    test2<257>();

    test2<511>();
    test2<512>();
    test2<513>();
}
