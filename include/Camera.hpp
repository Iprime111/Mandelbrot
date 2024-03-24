#ifndef CAMERA_HPP_
#define CAMERA_HPP_
    
#include <SFML/System/Vector2.hpp>

struct Camera {
    sf::Vector2f position = {};
    float        scale    = 1.f;
}; 

#endif
