#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include "ErrorCode.hpp"

static size_t    TimersCount = 0;
static clock_t *TimersArray = NULL;


ErrorCode InitTimers (size_t timersCount) {
    TimersCount = timersCount;

    TimersArray = (clock_t *) calloc (TimersCount, sizeof (clock_t));
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

    TimersArray [timerIndex] = clock ();

    return ErrorCode::NO_ERRORS;
}

clock_t GetTimerValue (size_t timerIndex) {
    assert (TimersArray);
    assert (timerIndex < TimersCount);

    return clock () - TimersArray [timerIndex];
}
