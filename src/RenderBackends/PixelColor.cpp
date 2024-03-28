#include <SFML/Config.hpp>
#include <cstddef>

#include "ColorGradients.hpp"
#include "Config.hpp"
#include "RenderBackends/PixelColor.hpp"

//NOTE smoothing algo (along with original variable naming) is taken from http://www.csharphelper.com/howtos/howto_mandelbrot_smooth.html
void SetPixelColor (sf::Uint8 *pixelArray, size_t iterationCount, size_t pixelX, size_t pixelY, size_t gradientNumber) {

    if (iterationCount >= MAX_ITERATIONS_PER_PIXEL) {
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL]     = 0;
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 1] = 0;
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 2] = 0;

        return;
    }

    const Gradient *currentGradient = &GRADIENTS [gradientNumber];

    float mu = (float) iterationCount / (float) MAX_ITERATIONS_PER_PIXEL * (float) currentGradient->size;
    
    int clr1 = (int) mu;
    float t2 = mu - (float) clr1;
    float t1 = 1 - t2;
    clr1 = clr1 % (int) currentGradient->size;
    int clr2 = (clr1 + 1) % (int) currentGradient->size;

    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL] 
        = (sf::Uint8) (currentGradient->colors [clr1].r * t1 + currentGradient->colors [clr2].r * t2);

    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 1] 
        = (sf::Uint8) (currentGradient->colors [clr1].g * t1 + currentGradient->colors [clr2].g * t2);
    
    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 2] 
        = (sf::Uint8) (currentGradient->colors [clr1].b * t1 + currentGradient->colors [clr2].b * t2);
}
