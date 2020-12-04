// g++ -O2 -std=c++17 -pthread 08_measure_sort.cpp profiler.cpp
//
// Compare with
//
// g++ -O0 -std=c++17 -pthread 08_measure_sort.cpp profiler.cpp
// g++ -O2 -std=c++17 -pthread -fno-inline 08_measure_sort.cpp profiler.cpp
//

#include <algorithm>   // std::copy/sort
#include <iostream>    // std::cerr
#include <mutex>       // std::mutex
#include <vector>      // std::vector
#include <stdlib.h>    // abort/qsort/rand
#include "profiler.h"  // PROFILE_CHECK

enum profiled_functions {
    PF_SORT_WITH_LESS,
    PF_SORT_WITH_FUNC,
    PF_QSORT_WITH_FUNC,
};

name_mapper name_map[] = {
    {PF_SORT_WITH_LESS, "sort_with_less"},
    {PF_SORT_WITH_FUNC, "sort_with_func"},
    {PF_QSORT_WITH_FUNC, "qsort_with_func"},
    {-1, NULL}};

static bool less_func(int x, int y)
{
    return x < y;
}

static int c_less_func(const void* a, const void* b)
{
    auto x = *reinterpret_cast<const int*>(a);
    auto y = *reinterpret_cast<const int*>(b);
    if (x < y) {
        return -1;
    } else if (x > y) {
        return 1;
    } else {
        return 0;
    }
}

void sort_with_less(std::vector<int>& v)
{
    PROFILE_CHECK(PF_SORT_WITH_LESS);
    std::sort(v.begin(), v.end(), std::less<int>());
}

void sort_with_func(std::vector<int>& v)
{
    PROFILE_CHECK(PF_SORT_WITH_FUNC);
    std::sort(v.begin(), v.end(), less_func);
}

void qsort_with_func(std::vector<int>& v)
{
    PROFILE_CHECK(PF_QSORT_WITH_FUNC);
    qsort(v.data(), v.size(), sizeof(int), c_less_func);
}

void check_vector(const std::vector<int>& v)
{
    if (v.empty()) {
        return;
    }
    for (auto i = std::size_t(0); i < v.size() - 1; ++i) {
        if (v[i] > v[i + 1]) {
            std::cerr << "Vector check failed!" << std::endl;
            abort();
        }
    }
}

std::vector<int> data;
std::vector<int> temp;
std::mutex mutex;

#define LEN 500
#define LOOPS 10000

int main()
{
    for (int i = 0; i < LEN; ++i) {
        data.push_back(rand());
    }

    {
        for (int i = 0; i < LOOPS; ++i) {
            std::lock_guard guard{mutex};
            temp.resize(data.size(), 0);
            std::copy(data.begin(), data.end(), temp.begin());
            sort_with_less(temp);
        }
        check_vector(temp);
    }

    {
        for (int i = 0; i < LOOPS; ++i) {
            std::lock_guard guard{mutex};
            temp.resize(data.size(), 0);
            std::copy(data.begin(), data.end(), temp.begin());
            sort_with_func(temp);
        }
        check_vector(temp);
    }

    {
        for (int i = 0; i < LOOPS; ++i) {
            std::lock_guard guard{mutex};
            temp.resize(data.size(), 0);
            std::copy(data.begin(), data.end(), temp.begin());
            qsort_with_func(temp);
        }
        check_vector(temp);
    }
}
