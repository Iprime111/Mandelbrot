#ifndef COLOR_GRADIENTS_HPP_
#define COLOR_GRADIENTS_HPP_

#include <SFML/Graphics/Color.hpp>
#include <cstddef>

const size_t GRADIENT_SIZES [] = {5};

const sf::Color DEFAULT_GRADIENT [] = {
    sf::Color (  0,   7, 100),
    sf::Color ( 32, 107, 203),
    sf::Color (237, 255, 255),
    sf::Color (255, 170,   0),
    sf::Color (  0,   2,   0),
};

#endif
