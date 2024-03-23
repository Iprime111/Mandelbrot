#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <cstddef>

const size_t DEFAULT_WINDOW_WIDTH     = 800; 
const size_t DEFAULT_WINDOW_HEIGHT    = 600;
const size_t BYTES_PER_PIXEL          = 4;
const size_t MAX_ITERATIONS_PER_PIXEL = 256;
const float  MAX_POINT_RADIUS_SQUARE  = 100.f;

const float  SCALE_INCREMENT          = 0.1f;
const float  X_SPEED                  = 0.05f;
const float  Y_SPEED                  = 0.05f;

const float MAX_SCALING               = 100.0f;
const float MIN_SCALING               = 0.001f;

const float deltaX = 1 / ((float) DEFAULT_WINDOW_WIDTH);
const float deltaY = 1 / ((float) DEFAULT_WINDOW_HEIGHT);

#endif
