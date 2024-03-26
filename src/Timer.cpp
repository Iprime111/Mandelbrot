#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include "ErrorCode.hpp"

static size_t    TimersCount = 0;
static uint64_t *TimersArray = NULL;

inline uint64_t GetTime ();

ErrorCode InitTimers (size_t timersCount) {
    TimersCount = timersCount;

    TimersArray = (uint64_t *) calloc (TimersCount, sizeof (uint64_t));
    if (!TimersArray)
        return ErrorCode::ALLOCATION_ERROR;

    return ErrorCode::NO_ERRORS;
}

ErrorCode DestroyTimers () {
    free (TimersArray);
    TimersArray = NULL;
    TimersCount = 0;

    return ErrorCode::NO_ERRORS;
}

ErrorCode StartTimer (size_t timerIndex) {
    assert (TimersArray);
    assert (timerIndex < TimersCount);

    TimersArray [timerIndex] = GetTime ();

    return ErrorCode::NO_ERRORS;
}

long GetTimerValue (size_t timerIndex) {
    assert (TimersArray);
    assert (timerIndex < TimersCount);

    return GetTime () - TimersArray [timerIndex];
}

inline uint64_t GetTime () {
    uint32_t axReg = 0, dxReg = 0;

    asm volatile ("rdtsc" : "=a" (axReg), "=d" (dxReg));
    
    return ((uint64_t) dxReg << 32) | axReg;
}
