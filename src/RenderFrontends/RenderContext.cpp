#include <SFML/Config.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <cassert>
#include <cstdio>
#include <libgen.h>
#include <unistd.h>

#include "RenderFrontends/RenderContext.hpp"
#include "Config.hpp"
#include "ErrorCode.hpp"

static ErrorCode InitText    (RenderContext *context);
static ErrorCode InitShader  (RenderContext *context);
static ErrorCode InitTexture (RenderContext *context);

static char *GetApplicationDirectory ();

static const size_t    ALPHA_CHANNEL_SHIFT = 3;
static const sf::Uint8 ALPHA_HIGH_VALUE    = 0xff;

#define InitWithErrorCheck(INIT_FUNCTION) \
    if ((context->error = INIT_FUNCTION) != ErrorCode::NO_ERRORS) {DestroyRenderContext (context); return context->error;}

ErrorCode InitRenderContext (RenderContext *context) {
    assert (context);

    context->error = ErrorCode::NO_ERRORS;
    context->camera = {.position = sf::Vector2f (0.f, 0.f), .scale = 1.f};

    context->mainWindow.create (sf::VideoMode (DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), "Mandelbrot", sf::Style::Titlebar | sf::Style::Close);

    context->texturePixels = NULL;
    InitWithErrorCheck (InitTexture (context));

    context->contentSprite = sf::Sprite (context->contentTexture);

    context->applicationDirectory = GetApplicationDirectory ();
    context->filenameBuffer = (char *) calloc (FILENAME_MAX, sizeof (char));
    InitWithErrorCheck (InitText   (context));
    InitWithErrorCheck (InitShader (context));

    context->currentBackend = context->currentGradient = 0;

    return ErrorCode::NO_ERRORS;
}

ErrorCode DestroyRenderContext (RenderContext *context) {
    if (!context)
        return ErrorCode::NO_ERRORS;

    free (context->texturePixels); 
    free (context->infoTextBuffer);
    free (context->filenameBuffer);
    free (context->applicationDirectory);

    context->texturePixels        = NULL;
    context->infoTextBuffer       = NULL;
    context->filenameBuffer       = NULL;
    context->applicationDirectory = NULL;

    context->currentGradient = context->currentBackend = 0;

    context->error = ErrorCode::WINDOW_CLOSED;

    return ErrorCode::NO_ERRORS;
}

static char *GetApplicationDirectory () {
    char *applicationPath = (char *) calloc (FILENAME_MAX, sizeof (char));
    
    ssize_t count = readlink ("/proc/self/exe", applicationPath, FILENAME_MAX);
    
    return dirname (applicationPath);
}

static ErrorCode InitText (RenderContext *context) {
    assert (context);
    assert (context->infoText);
    assert (context->font);
    assert (context->filenameBuffer);

    snprintf (context->filenameBuffer, FILENAME_MAX, FONT_PATH, context->applicationDirectory);

    if (!context->font.loadFromFile (context->filenameBuffer)) {
        return ErrorCode::FONT_NOT_LOADED;
    }

    context->infoText.setFont (context->font);
    context->infoText.setCharacterSize (INFO_TEXT_SIZE);
    context->infoText.setFillColor (sf::Color::Green);
    context->infoText.setStyle (sf::Text::Bold);
    context->infoText.setString ("");
    
    context->infoTextBuffer = (char *) calloc (MAX_INFO_TEXT_LENGTH, sizeof (char));

    return ErrorCode::NO_ERRORS;
}

static ErrorCode InitShader (RenderContext *context) {
    assert (context);
    assert (context->shader);
    assert (context->filenameBuffer);

    snprintf (context->filenameBuffer, FILENAME_MAX, SHADER_PATH, context->applicationDirectory);

    if (!context->shader.loadFromFile (context->filenameBuffer, sf::Shader::Fragment))
        return ErrorCode::SHADER_LOAD_ERROR;

    context->shader.setUniform ("ScreenSize", sf::Vector2f ((float) DEFAULT_WINDOW_WIDTH, (float) DEFAULT_WINDOW_HEIGHT));

    return ErrorCode::NO_ERRORS;
}

static ErrorCode InitTexture (RenderContext *context) {
    assert (contentTexture);
    assert (texturePixels);

    context->texturePixels = (sf::Uint8 *) calloc (DEFAULT_WINDOW_WIDTH * DEFAULT_WINDOW_HEIGHT * BYTES_PER_PIXEL, sizeof (sf::Uint8));

    if (!context->contentTexture.create (DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT))
        return ErrorCode::ALLOCATION_ERROR;

    for (size_t alphaNumber = 0; alphaNumber < DEFAULT_WINDOW_WIDTH * DEFAULT_WINDOW_HEIGHT; alphaNumber++)
        context->texturePixels [alphaNumber * BYTES_PER_PIXEL + ALPHA_CHANNEL_SHIFT] = ALPHA_HIGH_VALUE;

    return ErrorCode::NO_ERRORS;
}

