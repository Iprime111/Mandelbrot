#include <SFML/Config.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "Camera.hpp"
#include "Config.hpp"
#include "ErrorCode.hpp"
#include "RenderBackends/PixelColor.hpp"
#include "RenderBackends/ArrayOptimizationsBackend.hpp"

static const size_t OPTIMIZATION_RATE = 16;
#define Loop for (size_t idx = 0; idx < OPTIMIZATION_RATE; idx++)

static ErrorCode UpdatePixel (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber);

static ErrorCode UpdatePixel (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber) {
    float x0 = ((float) pixelX - (float) DEFAULT_WINDOW_WIDTH  / 2) * deltaX * camera->scale + camera->position.x;
    float y0 = ((float) pixelY - (float) DEFAULT_WINDOW_HEIGHT / 2) * deltaY * camera->scale + camera->position.y;

    float x0Arr [OPTIMIZATION_RATE] = {}; Loop x0Arr [idx] = x0 + deltaX * (float) idx * camera->scale;
    float y0Arr [OPTIMIZATION_RATE] = {}; Loop y0Arr [idx] = y0;

    float xN [OPTIMIZATION_RATE] = {}; Loop xN [idx] = x0Arr [idx];
    float yN [OPTIMIZATION_RATE] = {}; Loop yN [idx] = y0Arr [idx]; 

    size_t iterations [OPTIMIZATION_RATE] = {}; Loop iterations [idx] = 0;

    for (size_t n = 0; n < MAX_ITERATIONS_PER_PIXEL; n++) {
        float x2 [OPTIMIZATION_RATE] = {}; Loop x2 [idx] = xN [idx] * xN [idx];
        float y2 [OPTIMIZATION_RATE] = {}; Loop y2 [idx] = yN [idx] * yN [idx];

        float r2 [OPTIMIZATION_RATE] = {}; Loop r2 [idx] = x2 [idx] + y2 [idx];
        float xy [OPTIMIZATION_RATE] = {}; Loop xy [idx] = xN [idx] * yN [idx];

        uint8_t cmp [OPTIMIZATION_RATE] = {}; Loop cmp [idx] = (r2 [idx] <= MAX_POINT_RADIUS_SQUARE);

        uint64_t mask = 0;
        Loop mask |= (cmp [idx] << idx);

        if (!mask)
            break;

        Loop iterations [idx] += cmp [idx];

        Loop xN [idx] = x2 [idx] - y2 [idx] + x0Arr [idx];
        Loop yN [idx] = 2 * xy [idx] + y0Arr [idx];
    }

    Loop SetPixelColor (pixelArray, iterations [idx], pixelX + idx, pixelY, gradientNumber);

    return ErrorCode::NO_ERRORS;
}

ErrorCode UpdateTextureArrayOptimized (sf::Uint8 *pixelArray, Camera *camera, size_t width, size_t height, size_t gradientNumber) {
    assert (pixelArray);
    assert (camera);

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x += OPTIMIZATION_RATE) {
            UpdatePixel (pixelArray, camera, x, y, gradientNumber);
        }
    }

    return ErrorCode::NO_ERRORS;
}
