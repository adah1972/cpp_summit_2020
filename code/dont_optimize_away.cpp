#include "dont_optimize_away.h"
#include <stdio.h>    // putchar
#ifdef _WIN32
#include <process.h>  // _getpid
#else
#include <unistd.h>   // getpid
#endif

#ifdef _WIN32
static auto pid = _getpid();
#else
static auto pid = getpid();
#endif

void fake_reference(char* ptr)
{
    if (pid == 0) {
        putchar(*ptr);
    }
}
