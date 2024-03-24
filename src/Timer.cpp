#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include "ErrorCode.hpp"

static size_t    TimersCount = 0;
static uint32_t *TimersArray = NULL;

static uint32_t GetTickCount ();

ErrorCode InitTimers (size_t timersCount) {
    TimersCount = timersCount;

    TimersArray = (uint32_t *) calloc (TimersCount, sizeof (uint32_t));
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

    TimersArray [timerIndex] = GetTickCount ();

    return ErrorCode::NO_ERRORS;
}

uint32_t GetTimerValue (size_t timerIndex) {
    assert (TimersArray);
    assert (timerIndex < TimersCount);

    return GetTickCount () - TimersArray [timerIndex];
}

static uint32_t GetTickCount () {
        struct timespec ts;

        clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
        return (1000 * ts.tv_sec + ts.tv_nsec / 1000000);
}
