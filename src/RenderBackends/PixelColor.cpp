#include <SFML/Config.hpp>
#include <cstddef>

#include "ColorGradients.hpp"
#include "Config.hpp"
#include "RenderBackends/PixelColor.hpp"

void SetPixelColor (sf::Uint8 *pixelArray, size_t iterationCount, size_t pixelX, size_t pixelY) {

    if (iterationCount >= MAX_ITERATIONS_PER_PIXEL) {
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL]     = 0;
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 1] = 0;
        pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 2] = 0;

        return;
    }

    float mu = (float) iterationCount / (float) MAX_ITERATIONS_PER_PIXEL * GRADIENT_SIZES [0];
    
    int clr1 = (int)mu;
    double t2 = mu - clr1;
    double t1 = 1 - t2;
    clr1 = clr1 % GRADIENT_SIZES [0];
    int clr2 = (clr1 + 1) % GRADIENT_SIZES [0];

    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL] 
        = (sf::Uint8) (DEFAULT_GRADIENT [clr1].r * t1 + DEFAULT_GRADIENT [clr2].r * t2);

    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 1] 
        = (sf::Uint8) (DEFAULT_GRADIENT [clr1].g * t1 + DEFAULT_GRADIENT [clr2].g * t2);
    
    pixelArray [(pixelX + pixelY * DEFAULT_WINDOW_WIDTH) * BYTES_PER_PIXEL + 2] 
        = (sf::Uint8) (DEFAULT_GRADIENT [clr1].b * t1 + DEFAULT_GRADIENT [clr2].b * t2);
}
