#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cassert>
#include <cstddef>

#include "Config.hpp"
#include "ErrorCode.hpp"
#include "RenderBackends/DefaultBackend.hpp"
#include "RenderBackends/PixelColor.hpp"

static ErrorCode UpdatePixel (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber); //TODO why is there function's header if function goes right after it?

static ErrorCode UpdatePixel (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber) {
    float x0 = ((float) pixelX - (float) DEFAULT_WINDOW_WIDTH  / 2) * deltaX * camera->scale + camera->position.x;
    float y0 = ((float) pixelY - (float) DEFAULT_WINDOW_HEIGHT / 2) * deltaY * camera->scale + camera->position.y;

    float xN = x0, yN = y0;

    size_t iterations = 0;
    for (iterations = 0; iterations < MAX_ITERATIONS_PER_PIXEL; iterations++) {
        float x2 = xN * xN, y2 = yN * yN;
        float r2 = x2 + y2;
        float xy = xN * yN;

        if (r2 >= MAX_POINT_RADIUS_SQUARE)
            break;

        xN = x2 - y2 + x0;
        yN = 2 * xy + y0;
    }

    SetPixelColor (pixelArray, iterations, pixelX, pixelY, gradientNumber);

    return ErrorCode::NO_ERRORS;
}

ErrorCode UpdateTextureDefault (sf::Uint8 *pixelArray, Camera *camera, size_t gradientNumber) { // TODO this fucntion looks almost the same in all three backends. Think about how to unite them
    assert (pixelArray);
    assert (camera);
    
    for (size_t y = 0; y < DEFAULT_WINDOW_HEIGHT; y++) {
        for (size_t x = 0; x < DEFAULT_WINDOW_WIDTH; x++) {
            UpdatePixel (pixelArray, camera,  x, y, gradientNumber);
        }
    }

    return ErrorCode::NO_ERRORS;
}
