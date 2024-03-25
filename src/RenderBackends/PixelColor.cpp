#include <SFML/Config.hpp>
#include <cstddef>

#include "ColorGradients.hpp"
#include "Config.hpp"
#include "RenderBackends/PixelColor.hpp"

void SetPixelColor (sf::Uint8 *pixelArray, size_t iterationCount, size_t pixelX, size_t pixelY, size_t gradientNumber) {

    if (iterationCount >= MAX_ITERATIONS_PER_PIXEL) {
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL]     = 0;
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 1] = 0;
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 2] = 0;

        return;
    }

    float mu = (float) iterationCount / (float) MAX_ITERATIONS_PER_PIXEL * (float) GRADIENT_SIZES [gradientNumber];
    
    int clr1 = (int) mu;
    float t2 = mu - (float) clr1;
    float t1 = 1 - t2;
    clr1 = clr1 % (int) GRADIENT_SIZES [gradientNumber];
    int clr2 = (clr1 + 1) % (int) GRADIENT_SIZES [gradientNumber];

    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL] 
        = (sf::Uint8) (GRADIENTS [gradientNumber][clr1].r * t1 + GRADIENTS [gradientNumber][clr2].r * t2);

    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 1] 
        = (sf::Uint8) (GRADIENTS [gradientNumber][clr1].g * t1 + GRADIENTS [gradientNumber][clr2].g * t2);
    
    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 2] 
        = (sf::Uint8) (GRADIENTS [gradientNumber][clr1].b * t1 + GRADIENTS [gradientNumber][clr2].b * t2);
}
