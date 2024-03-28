#ifndef COLOR_GRADIENTS_HPP_
#define COLOR_GRADIENTS_HPP_

#include <SFML/Graphics/Color.hpp>
#include <cstddef>

#define GRADIENT_SIZE(GRAD) sizeof (GRAD) / sizeof (sf::Color)

struct Gradient {
    const sf::Color *colors = {};
    const size_t     size   = 0;
};  

const sf::Color DEFAULT_GRADIENT [] = {
    sf::Color (  0,   7, 100),
    sf::Color ( 32, 107, 203),
    sf::Color (237, 255, 255),
    sf::Color (255, 170,   0),
    sf::Color (  0,   2,   0),
};

const sf::Color GREEN_GRADIENT [] = {
    sf::Color (0, 0, 0),
    sf::Color (0, 200, 0),
    sf::Color (0, 0, 0),
};

const sf::Color BLACK_WHITE_GRADIENT [] {
    sf::Color (0, 0, 0),
    sf::Color (255, 255, 255),
    sf::Color (0, 0, 0),
};

static const Gradient GRADIENTS [] = {
    {.colors = DEFAULT_GRADIENT,     .size = GRADIENT_SIZE (DEFAULT_GRADIENT)},
    {.colors = GREEN_GRADIENT,       .size = GRADIENT_SIZE (GREEN_GRADIENT)},
    {.colors = BLACK_WHITE_GRADIENT, .size = GRADIENT_SIZE (BLACK_WHITE_GRADIENT)},
};

const size_t GRADIENTS_COUNT = sizeof (GRADIENTS) / sizeof (Gradient);

#undef GRADIENT_SIZE
#endif
