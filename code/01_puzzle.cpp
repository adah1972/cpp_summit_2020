// g++ -O0 01_puzzle.cpp
// g++ -O1 01_puzzle.cpp
// g++ -O2 01_puzzle.cpp
//

#include <stdio.h>   // printf
#include <string.h>  // memset
#include <time.h>    // clock

int main()
{
    constexpr auto LOOPS = 10000000;

    {
        char buffer[80];
        auto t1 = clock();
        for (auto i = 0; i < LOOPS; ++i) {
            memset(buffer, 0, sizeof buffer);
        }
        auto t2 = clock();
        printf("%g\n", (t2 - t1) * 1.0 / CLOCKS_PER_SEC);
    }

    {
        char buffer[80];
        auto t1 = clock();
        for (auto i = 0; i < LOOPS; ++i) {
            for (size_t j = 0; j < sizeof buffer; ++j) {
                buffer[j] = 0;
            }
        }
        auto t2 = clock();
        printf("%g\n", (t2 - t1) * 1.0 / CLOCKS_PER_SEC);
    }
}
