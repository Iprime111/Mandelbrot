#ifndef BACKEND_COMMONS_HPP_
#define BACKEND_COMMONS_HPP_

#include <SFML/Config.hpp>
#include <cstddef>

#include "ErrorCode.hpp"
#include "Camera.hpp"
#include "RenderBackends/ArrayOptimizationsBackend.hpp"
#include "RenderBackends/DefaultBackend.hpp"
#include "RenderBackends/SimdBackend.hpp"

typedef ErrorCode (* TextureUpdater) (sf::Uint8 *texturePixels, Camera *camera, size_t gradientNumber);

const TextureUpdater AVAILABLE_BACKENDS [] = {UpdateTextureDefault, UpdateTextureArrayOptimized, UpdateTextureSimd};
const size_t CPU_BACKENDS_COUNT = sizeof (AVAILABLE_BACKENDS) / sizeof (TextureUpdater);

#endif
