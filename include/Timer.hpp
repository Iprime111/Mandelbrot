#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <cstddef>
#include <ctime>

#include "ErrorCode.hpp"

ErrorCode InitTimers    (size_t timersCount);
ErrorCode DestroyTimers ();
ErrorCode StartTimer    (size_t timerIndex);
long      GetTimerValue (size_t timerIndex);

#endif  
