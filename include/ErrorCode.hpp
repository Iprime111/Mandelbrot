#ifndef ERROR_CODE_HPP_
#define ERROR_CODE_HPP_

enum class ErrorCode {
    NO_ERRORS = 0,
    ALLOCATION_ERROR   = 1 << 0,
    WINDOW_CLOSED      = 1 << 1,
    FONT_NOT_LOADED    = 1 << 2,
    INDEX_OUT_OF_RANGE = 1 << 3,
};

#endif
