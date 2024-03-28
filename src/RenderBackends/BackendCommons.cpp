#include "RenderBackends/BackendCommons.hpp"
#include <SFML/Config.hpp>
#include <cassert>
#include <cstddef>

#include "ColorGradients.hpp"
#include "Config.hpp"
#include "RenderFrontends/RenderContext.hpp"

ErrorCode UpdateTexture (RenderContext *context, size_t batchSize, PixelUpdater updater) {
    assert (context);
    assert (updater);
    
    for (size_t y = 0; y < DEFAULT_WINDOW_HEIGHT; y++) {
        for (size_t x = 0; x < DEFAULT_WINDOW_WIDTH; x += batchSize) {
            updater (context->texturePixels, &context->camera, x, y, context->currentGradient);
        }
    }

    return ErrorCode::NO_ERRORS;
}

//NOTE smoothing algo is taken from http://www.csharphelper.com/howtos/howto_mandelbrot_smooth.html

void SetPixelColor (sf::Uint8 *pixelArray, size_t iterationCount, size_t pixelX, size_t pixelY, size_t gradientNumber) {

    if (iterationCount >= MAX_ITERATIONS_PER_PIXEL) {
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL]     = 0;
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 1] = 0;
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 2] = 0;

        return;
    }

    const Gradient *currentGradient = &GRADIENTS [gradientNumber];

    float colorIndex = (float) iterationCount / (float) MAX_ITERATIONS_PER_PIXEL * (float) currentGradient->size;
    
    int roundedColorIndex          = (int) colorIndex;
    float colorFractionalPart      = colorIndex - (float) roundedColorIndex;
    float fractionalPartComplement = 1 - colorFractionalPart;

    roundedColorIndex  = roundedColorIndex       % (int) currentGradient->size;
    int nextColorIndex = (roundedColorIndex + 1) % (int) currentGradient->size;

    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL] 
        = (sf::Uint8) (currentGradient->colors [roundedColorIndex].r * fractionalPartComplement + currentGradient->colors [nextColorIndex].r * colorFractionalPart);

    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 1] 
        = (sf::Uint8) (currentGradient->colors [roundedColorIndex].g * fractionalPartComplement + currentGradient->colors [nextColorIndex].g * colorFractionalPart);
    
    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 2] 
        = (sf::Uint8) (currentGradient->colors [roundedColorIndex].b * fractionalPartComplement + currentGradient->colors [nextColorIndex].b * colorFractionalPart);
}
