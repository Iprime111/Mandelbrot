#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include "RenderBackends/BackendCommons.hpp"
#include <cstddef>

// TODO I think it's better to put path to shaders here like
// const char shaders_dir[] = "%s/shaders/dir"
// where %s should be replaced with path to project's directory

const size_t DEFAULT_WINDOW_WIDTH     = 800; 
const size_t DEFAULT_WINDOW_HEIGHT    = 600;
const size_t BYTES_PER_PIXEL          = 4;
const size_t MAX_ITERATIONS_PER_PIXEL = 256;
const float  MAX_POINT_RADIUS_SQUARE  = 100.f;

const float  SCALE_INCREMENT          = 0.1f;
const float  X_SPEED                  = 0.05f;
const float  Y_SPEED                  = 0.05f;

const float  MAX_SCALING              = 100.0f;
const float  MIN_SCALING              = 0.001f;

const size_t MAX_INFO_TEXT_LENGTH       = 256;
const size_t INFO_TEXT_SIZE             = 12;
const char   INFO_TEXT_FORMAT_STRING [] = "Frame time / Avg. frame time (ticks): %lu / %lu\nRender time / Avg. render time (ticks): %lu / %lu\nRender method: %s";

static const char *BACKEND_NAMES [BACKENDS_COUNT + 1] = {"Naive CPU", "Array optimized CPU", "SIMD optimized CPU", "GPU"};

const size_t TIMERS_COUNT             = 2;
const size_t FPS_TIMER                = 0;
const size_t RENDER_TIMER             = 1;

const size_t AVG_NUMBERS_COUNT        = 300;

const float deltaX = 1 / ((float) DEFAULT_WINDOW_WIDTH);
const float deltaY = 1 / ((float) DEFAULT_WINDOW_WIDTH);

#endif
