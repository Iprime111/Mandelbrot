#ifndef BACKEND_COMMONS_HPP_
#define BACKEND_COMMONS_HPP_

#include <SFML/Config.hpp>
#include <cstddef>

#include "ErrorCode.hpp"
#include "Camera.hpp"
#include "RenderBackends/ArrayOptimizationsBackend.hpp"
#include "RenderBackends/DefaultBackend.hpp"
#include "RenderBackends/SimdBackend.hpp"
#include "RenderFrontends/RenderContext.hpp"

typedef ErrorCode (* PixelUpdater) (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber);

struct Backend {
    PixelUpdater updater   = NULL;
    size_t       batchSize = 0;
};

const Backend AVAILABLE_BACKENDS [] = {
    {.updater = UpdatePixelDefault,        .batchSize = 1},
    {.updater = UpdatePixelArrayOptimized, .batchSize = 16},
    {.updater = UpdatePixelSimd,           .batchSize = 16}
};

const size_t CPU_BACKENDS_COUNT = sizeof (AVAILABLE_BACKENDS) / sizeof (Backend);

void SetPixelColor (sf::Uint8 *pixelArray, size_t iterationCount, size_t pixelX, size_t pixelY, size_t gradientNumber);
ErrorCode UpdateTexture (RenderContext *context, size_t batchSize, PixelUpdater updater);

#endif
