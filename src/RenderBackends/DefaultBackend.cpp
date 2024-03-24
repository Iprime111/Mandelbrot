#include <SFML/Config.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cstddef>

#include "Config.hpp"
#include "ErrorCode.hpp"
#include "RenderBackends/DefaultBackend.hpp"
#include "RenderBackends/PixelColor.hpp"

static ErrorCode UpdatePixel (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY);

static ErrorCode UpdatePixel (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY) {
    float x0 = ((float) pixelX - (float) DEFAULT_WINDOW_WIDTH  / 2) * deltaX * camera->scale + camera->centerX;
    float y0 = ((float) pixelY - (float) DEFAULT_WINDOW_HEIGHT / 2) * deltaY * camera->scale + camera->centerY;

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

    SetPixelColor (pixelArray, iterations, pixelX, pixelY);

    return ErrorCode::NO_ERRORS;
}

ErrorCode UpdateTextureDefault (sf::Uint8 *pixelArray, Camera *camera, size_t width, size_t height) {
    
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            UpdatePixel (pixelArray, camera,  x, y);
        }
    }

    return ErrorCode::NO_ERRORS;
}
