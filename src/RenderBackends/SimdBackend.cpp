#include <SFML/Config.hpp>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "Camera.hpp"
#include "Config.hpp"
#include "ErrorCode.hpp"
#include "RenderBackends/AvxDataTypes.hpp"
#include "RenderBackends/PixelColor.hpp"
#include "RenderBackends/SimdBackend.hpp"

static float Indices [OPTIMIZATION_RATE] = {15.f, 14.f, 13.f, 12.f, 11.f, 10.f, 9.f, 8.f, 7.f, 6.f, 5.f, 4.f, 3.f, 2.f, 1.f, 0.f};

static ErrorCode UpdatePixel (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber);

static ErrorCode UpdatePixel (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber) {
    float x0 = ((float) pixelX - (float) DEFAULT_WINDOW_WIDTH  / 2) * deltaX * camera->scale + camera->position.x;
    float y0 = ((float) pixelY - (float) DEFAULT_WINDOW_HEIGHT / 2) * deltaY * camera->scale + camera->position.y;

    avxf_t x0Arr = avxf_t (x0) + avxf_t (deltaX) * avxf_t (Indices) * avxf_t (camera->scale);
    avxf_t y0Arr (y0);
    
    avxf_t xN (x0Arr);
    avxf_t yN (y0Arr);

    avxu_t iterations ((uint32_t) 0);

    for (size_t n = 0; n < MAX_ITERATIONS_PER_PIXEL; n++) {
        avxf_t x2 = xN * xN;
        avxf_t y2 = yN * yN;
        avxf_t xy = xN * yN;
        avxf_t r2 = x2 + y2;           

        __mmask16 mask = r2 <= avxf_t (MAX_POINT_RADIUS_SQUARE); 

        if (!mask)
            break;

        iterations = iterations + avxu_t (mask);

        xN = x2 - y2 + x0Arr;
        yN = xy + xy + y0Arr;
    }

    for (size_t idx = 0; idx < OPTIMIZATION_RATE; idx++) SetPixelColor (pixelArray, iterations.array [idx], pixelX + idx, pixelY, gradientNumber);

    return ErrorCode::NO_ERRORS;
}

ErrorCode UpdateTextureSimd (sf::Uint8 *pixelArray, Camera *camera, size_t gradientNumber) {
    assert (pixelArray);
    assert (camera);
    
    for (size_t y = 0; y < DEFAULT_WINDOW_HEIGHT; y++) {
        for (size_t x = 0; x < DEFAULT_WINDOW_WIDTH; x += OPTIMIZATION_RATE) {
            UpdatePixel (pixelArray, camera, x, y, gradientNumber);
        }
    }

    return ErrorCode::NO_ERRORS;
}
