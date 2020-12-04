// g++ -O0 03_puzzle_solution.cpp
// g++ -O1 03_puzzle_solution.cpp
// g++ -O2 03_puzzle_solution.cpp
//

#include <stdio.h>               // printf
#include <string.h>              // memset
#include <time.h>                // clock
#include "dont_optimize_away.h"  // dont_optimize_away

#ifdef __GNUC__
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

template <size_t N>
NOINLINE void test_memset(char (&buffer)[N])
{
    memset(buffer, 0, sizeof buffer);
}

template <size_t N>
NOINLINE void test_plain_loop(char (&buffer)[N])
{
    for (size_t j = 0; j < sizeof buffer; ++j) {
        buffer[j] = 0;
    }
}

int main()
{
    constexpr auto LOOPS = 10000000;

    {
        char buffer[80];
        auto t1 = clock();
        for (auto i = 0; i < LOOPS; ++i) {
            test_memset(buffer);
            dont_optimize_away(buffer);
        }
        auto t2 = clock();
        printf("%g\n", (t2 - t1) * 1.0 / CLOCKS_PER_SEC);
    }

    {
        char buffer[80];
        auto t1 = clock();
        for (auto i = 0; i < LOOPS; ++i) {
            test_plain_loop(buffer);
            dont_optimize_away(buffer);
        }
        auto t2 = clock();
        printf("%g\n", (t2 - t1) * 1.0 / CLOCKS_PER_SEC);
    }
}
