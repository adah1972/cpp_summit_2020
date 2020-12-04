// g++ -O0 02_puzzle_solution_wrong.cpp
// g++ -O1 02_puzzle_solution_wrong.cpp
// g++ -O2 02_puzzle_solution_wrong.cpp
//

#include <stdio.h>   // printf
#include <string.h>  // memset
#include <time.h>    // clock

template <size_t N>
void test_memset(volatile char (&buffer)[N])
{
    memset(const_cast<char*>(buffer), 0, sizeof buffer);
}

template <size_t N>
void test_plain_loop(volatile char (&buffer)[N])
{
    for (size_t j = 0; j < sizeof buffer; ++j) {
        buffer[j] = 0;
    }
}

int main()
{
    constexpr auto LOOPS = 10000000;

    {
        volatile char buffer[80];
        auto t1 = clock();
        for (auto i = 0; i < LOOPS; ++i) {
            test_memset(buffer);
        }
        auto t2 = clock();
        printf("%g\n", (t2 - t1) * 1.0 / CLOCKS_PER_SEC);
    }

    {
        volatile char buffer[80];
        auto t1 = clock();
        for (auto i = 0; i < LOOPS; ++i) {
            test_plain_loop(buffer);
        }
        auto t2 = clock();
        printf("%g\n", (t2 - t1) * 1.0 / CLOCKS_PER_SEC);
    }
}
