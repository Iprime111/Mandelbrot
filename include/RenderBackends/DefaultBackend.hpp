#ifndef DEFAULT_BACKEND_HPP_
#define DEFAULT_BACKEND_HPP_

#include <SFML/Config.hpp>
#include <cstddef>

#include "Camera.hpp"
#include "ErrorCode.hpp"

ErrorCode UpdatePixelDefault (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber);

#endif
