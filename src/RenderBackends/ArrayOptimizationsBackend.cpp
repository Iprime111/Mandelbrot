#include <SFML/Config.hpp>
#include <cstddef>
#include <cstdint>

#include "Camera.hpp"
#include "Config.hpp"
#include "ErrorCode.hpp"
#include "RenderBackends/ArrayOptimizationsBackend.hpp"

static const size_t OPTIMIZATION_RATE = 16;
#define Foreach(CALLBACK) do {for (size_t idx = 0; idx < OPTIMIZATION_RATE; idx++) {CALLBACK;}} while (0)


ErrorCode UpdatePixelArrayOptimized (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber) {
    float x0 = ((float) pixelX - (float) DEFAULT_WINDOW_WIDTH  / 2) * deltaX * camera->scale + camera->position.x;
    float y0 = ((float) pixelY - (float) DEFAULT_WINDOW_HEIGHT / 2) * deltaY * camera->scale + camera->position.y;

    float x0Arr [OPTIMIZATION_RATE] = {}; Foreach (x0Arr [idx] = x0 + deltaX * (float) idx * camera->scale);
    float y0Arr [OPTIMIZATION_RATE] = {}; Foreach (y0Arr [idx] = y0);

    float xN [OPTIMIZATION_RATE] = {}; Foreach (xN [idx] = x0Arr [idx]);
    float yN [OPTIMIZATION_RATE] = {}; Foreach (yN [idx] = y0Arr [idx]);

    size_t iterations [OPTIMIZATION_RATE] = {}; Foreach (iterations [idx] = 0);

    for (size_t n = 0; n < MAX_ITERATIONS_PER_PIXEL; n++) {
        float x2 [OPTIMIZATION_RATE] = {}; Foreach (x2 [idx] = xN [idx] * xN [idx]);
        float y2 [OPTIMIZATION_RATE] = {}; Foreach (y2 [idx] = yN [idx] * yN [idx]);

        float r2 [OPTIMIZATION_RATE] = {}; Foreach (r2 [idx] = x2 [idx] + y2 [idx]);
        float xy [OPTIMIZATION_RATE] = {}; Foreach (xy [idx] = xN [idx] * yN [idx]);

        uint8_t cmp [OPTIMIZATION_RATE] = {}; Foreach (cmp [idx] = (r2 [idx] <= MAX_POINT_RADIUS_SQUARE));

        int32_t mask = 0;
        Foreach (mask |= (cmp [idx] << idx));

        if (!mask)
            break;

        Foreach (iterations [idx] += cmp [idx]);

        Foreach (xN [idx] = x2 [idx] - y2 [idx] + x0Arr [idx]);
        Foreach (yN [idx] = 2 * xy [idx] + y0Arr [idx]);
    }

    Foreach (SetPixelColor (pixelArray, iterations [idx], pixelX + idx, pixelY, gradientNumber));



    return ErrorCode::NO_ERRORS;
}


