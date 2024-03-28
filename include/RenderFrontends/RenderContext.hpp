#ifndef RENDER_CONTEXT_HPP_
#define RENDER_CONTEXT_HPP_

#include <SFML/Config.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <cstddef>

#include "Camera.hpp"
#include "ErrorCode.hpp"

struct RenderContext {
    Camera           camera         = {};
    sf::RenderWindow mainWindow     = {};
    sf::Texture      contentTexture = {};
    sf::Uint8       *texturePixels  = {};
    sf::Font         font           = {};
    sf::Text         infoText       = {};
    sf::Sprite       contentSprite  = {};
    sf::Shader       shader         = {};

    ErrorCode error = ErrorCode::NO_ERRORS;

    char *infoTextBuffer       = NULL;
    char *applicationDirectory = NULL;
    char *filenameBuffer       = NULL;

    size_t currentBackend  = 0;
    size_t currentGradient = 0;
};

ErrorCode InitRenderContext    (RenderContext *context);
ErrorCode DestroyRenderContext (RenderContext *context);

#endif
