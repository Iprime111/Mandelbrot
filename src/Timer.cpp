#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include "ErrorCode.hpp"

static size_t    TimersCount = 0;
static timespec *TimersArray = NULL;


ErrorCode InitTimers (size_t timersCount) {
    TimersCount = timersCount;

    TimersArray = (timespec *) calloc (TimersCount, sizeof (timespec));
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

    clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &TimersArray [timerIndex]);

    return ErrorCode::NO_ERRORS;
}

long GetTimerValue (size_t timerIndex) {
    assert (TimersArray);
    assert (timerIndex < TimersCount);

    timespec endTimespec = {};
    clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &endTimespec);

    return 1000 * endTimespec.tv_sec + endTimespec.tv_nsec * 1e-6 - (1000 * TimersArray [timerIndex].tv_sec + TimersArray [timerIndex].tv_nsec * 1e-6);
}
