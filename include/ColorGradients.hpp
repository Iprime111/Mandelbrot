#ifndef COLOR_GRADIENTS_HPP_
#define COLOR_GRADIENTS_HPP_

#include <SFML/Graphics/Color.hpp>
#include <cstddef>

#define GRADIENT_SIZE(GRAD) sizeof (GRAD) / sizeof (sf::Color)

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

const size_t GRADIENT_SIZES [] = {GRADIENT_SIZE (DEFAULT_GRADIENT), GRADIENT_SIZE (GREEN_GRADIENT), 
                                  GRADIENT_SIZE (BLACK_WHITE_GRADIENT)};

static const sf::Color *GRADIENTS [] = {
    DEFAULT_GRADIENT,
    GREEN_GRADIENT,
    BLACK_WHITE_GRADIENT,
};

const size_t GRADIENTS_COUNT = sizeof (GRADIENTS) / sizeof (sf::Color *);

#undef GRADIENT_SIZE
#endif
