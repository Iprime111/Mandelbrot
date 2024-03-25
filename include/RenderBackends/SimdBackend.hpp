#ifndef SIMD_BACKEND_HPP_
#define SIMD_BACKEND_HPP_

#include <SFML/Config.hpp>
#include <cstddef>

#include "Camera.hpp"
#include "ErrorCode.hpp"

ErrorCode UpdateTextureSimd (sf::Uint8 *pixelArray, Camera *camera, size_t width, size_t height, size_t gradientNumber);

#endif
