#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <cassert>
#include <cstdio>
#include <ctime>

#include "ColorGradients.hpp"
#include "ErrorCode.hpp"
#include "Config.hpp"
#include "RenderBackends/BackendCommons.hpp"
#include "RenderFrontends/SfmlRenderer.hpp"
#include "Timer.hpp"

//TODO: RenderContext struct

static char *InfoTextBuffer = NULL;

static ErrorCode InitText      (sf::Text *infoText, sf::Font *font);
static ErrorCode InitShader    (sf::Shader *shader);
static ErrorCode InitTexture   (sf::Texture *contentTexture, sf::Uint8 **texturePixels);
static ErrorCode ProcessEvents (sf::RenderWindow *mainWindow, Camera *camera, size_t *currentBackend, size_t *currentGradient);

static char *RenderStatsToString (clock_t fpsTimeValue, clock_t fpsTimeAvg, clock_t renderTimeValue, clock_t renderTimeAvg, size_t currentBackend);

#define DestroyEntities() DestroyTimers (); free (texturePixels); free (InfoTextBuffer);

#define InitWithErrorCheck(INIT_FUNCTION) \
    if ((error = INIT_FUNCTION) != ErrorCode::NO_ERRORS) {DestroyEntities (); return error;}

ErrorCode SfmlRenderCycle () {
    ErrorCode error = ErrorCode::NO_ERRORS;

    Camera camera = {.position = sf::Vector2f (0.f, 0.f), .scale = 1.f};

    sf::RenderWindow mainWindow (sf::VideoMode (DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), "Mandelbrot", sf::Style::Titlebar | sf::Style::Close);
    sf::Texture contentTexture = {};
    sf::Uint8 *texturePixels   = NULL;

    InitWithErrorCheck (InitTexture (&contentTexture, &texturePixels));

    sf::Sprite contentSprite (contentTexture);

    sf::Font font;
    sf::Text infoText;
    sf::Text dataText;
    InitWithErrorCheck (InitText (&infoText, &font));

    InitWithErrorCheck (InitTimers (TIMERS_COUNT));

    sf::Shader shader;
    InitWithErrorCheck (InitShader (&shader));

    size_t currentBackend  = 0;
    size_t currentGradient = 0;

    size_t avgNumbers = 0;
    
    clock_t fpsTimeAvg         = 0;
    clock_t renderTimeAvg      = 0;

    clock_t fpsTimesSum        = 0;
    clock_t renderTimesSum     = 0;

    while (mainWindow.isOpen ()) {
        StartTimer (FPS_TIMER);
        if (ProcessEvents (&mainWindow, &camera, &currentBackend, &currentGradient) != ErrorCode::NO_ERRORS) {
            break;
        }

        mainWindow.clear ();
        
        StartTimer (RENDER_TIMER);

        if (currentBackend < BACKENDS_COUNT) {
            AVAILABLE_BACKENDS [currentBackend] (texturePixels, &camera, currentGradient);
            contentTexture.update (texturePixels);
            mainWindow.draw (contentSprite);
        } else {
            shader.setUniform ("CameraPosition", camera.position);
            shader.setUniform ("Scale", camera.scale);
            mainWindow.draw (contentSprite, &shader);
        }

        clock_t renderTimeValue = GetTimerValue (RENDER_TIMER);
        clock_t fpsTimeValue    = GetTimerValue (FPS_TIMER);
    
        renderTimesSum += renderTimeValue;
        fpsTimesSum    += fpsTimeValue;
        avgNumbers++;

        renderTimeAvg = renderTimesSum / (clock_t) avgNumbers;
        fpsTimeAvg    = fpsTimesSum    / (clock_t) avgNumbers;

        if (avgNumbers == AVG_NUMBERS_COUNT) {
            fprintf (stderr, "Avg. FPS: %ld\nAvg render time: %ld\n\n", CLOCKS_PER_SEC / fpsTimeAvg, renderTimeAvg * 1000 / CLOCKS_PER_SEC);

            renderTimesSum = fpsTimesSum = 0;
            avgNumbers     = 0;
        }
        
        infoText.setString (RenderStatsToString (fpsTimeValue, fpsTimeAvg, renderTimeValue, renderTimeAvg, currentBackend));
        
        mainWindow.draw (infoText);
        mainWindow.display ();
    }
    
    DestroyEntities ();

    return ErrorCode::NO_ERRORS;
}

static char *RenderStatsToString (clock_t fpsTimeValue, clock_t fpsTimeAvg, clock_t renderTimeValue, clock_t renderTimeAvg, size_t currentBackend) {
    clock_t fpsValue = fpsTimeValue > 0 ? CLOCKS_PER_SEC / fpsTimeValue : CLOCKS_PER_SEC;
    clock_t fpsAvg   = fpsTimeAvg > 0   ? CLOCKS_PER_SEC / fpsTimeAvg   : CLOCKS_PER_SEC;

    snprintf (InfoTextBuffer, MAX_INFO_TEXT_LENGTH, INFO_TEXT_FORMAT_STRING, 
              fpsValue, fpsAvg, renderTimeValue * 1000 / CLOCKS_PER_SEC, 
              renderTimeAvg * 1000 / CLOCKS_PER_SEC, BACKEND_NAMES [currentBackend]);

    return InfoTextBuffer;
}

static inline ErrorCode ProcessEvents (sf::RenderWindow *mainWindow, Camera *camera, size_t *currentBackend, size_t *currentGradient) {
    assert (mainWindow);
    assert (camera);

    for (sf::Event event = {}; mainWindow->pollEvent (event);) {
        if (event.type == sf::Event::Closed) {
            mainWindow->close ();
            return ErrorCode::WINDOW_CLOSED;

        } else if (event.type == sf::Event::KeyPressed) {
            switch (event.key.scancode) {
                case sf::Keyboard::Scancode::Equal:
                    if (camera->scale > MIN_SCALING)
                        camera->scale -= SCALE_INCREMENT * camera->scale;
                    break;
                
                case sf::Keyboard::Scancode::Hyphen:
                    if (camera->scale < MAX_SCALING)
                        camera->scale += SCALE_INCREMENT * camera->scale;
                    break;

                case sf::Keyboard::Scancode::W:
                case sf::Keyboard::Scancode::Up:
                    camera->position.y -= Y_SPEED * camera->scale;
                    break;

                case sf::Keyboard::Scancode::A:
                case sf::Keyboard::Scancode::Left:
                    camera->position.x -= X_SPEED * camera->scale;
                    break;

                case sf::Keyboard::Scancode::S:
                case sf::Keyboard::Scancode::Down:
                    camera->position.y += Y_SPEED * camera->scale;
                    break;

                case sf::Keyboard::Scancode::D:
                case sf::Keyboard::Scancode::Right:
                    camera->position.x += X_SPEED * camera->scale;
                    break;

                case sf::Keyboard::Scancode::J:
                    if (*currentBackend > 0)
                        (*currentBackend)--;
                    else
                        *currentBackend = BACKENDS_COUNT;
                    break;

                case sf::Keyboard::Scancode::K:
                    if (*currentBackend < BACKENDS_COUNT)
                        (*currentBackend)++;
                    else
                        *currentBackend = 0;
                    break;

                case sf::Keyboard::Scancode::H:
                    if (*currentGradient > 0)
                        (*currentGradient)--;
                    else
                        *currentGradient = GRADIENTS_COUNT - 1;
                    break;

                case sf::Keyboard::Scancode::L:
                    if (*currentGradient < GRADIENTS_COUNT - 1)
                        (*currentGradient)++;
                    else
                        *currentGradient = 0;
                    break;

                default:
                    break;
            }
        }
    }

    return ErrorCode::NO_ERRORS;
}

static ErrorCode InitText (sf::Text *infoText, sf::Font *font) {
    assert (infoText);
    assert (font);

    if (!font->loadFromFile ("../fonts/JetBrainsMono-Bold.ttf")) {
        return ErrorCode::FONT_NOT_LOADED;
    }

    infoText->setFont (*font);
    infoText->setCharacterSize (INFO_TEXT_SIZE);
    infoText->setFillColor (sf::Color::Green);
    infoText->setStyle (sf::Text::Bold);
    infoText->setString ("");
    
    InfoTextBuffer = (char *) calloc (MAX_INFO_TEXT_LENGTH, sizeof (char));

    return ErrorCode::NO_ERRORS;
}

static ErrorCode InitShader (sf::Shader *shader) {
    assert (shader);

    if (!shader->loadFromFile ("../shaders/shader.glsl", sf::Shader::Fragment))
        return ErrorCode::SHADER_LOAD_ERROR;

    shader->setUniform ("ScreenSize", sf::Vector2f ((float) DEFAULT_WINDOW_WIDTH, (float) DEFAULT_WINDOW_HEIGHT));

    return ErrorCode::NO_ERRORS;
}

static ErrorCode InitTexture (sf::Texture *contentTexture, sf::Uint8 **texturePixels) {
    assert (contentTexture);
    assert (texturePixels);

    *texturePixels = (sf::Uint8 *) calloc (DEFAULT_WINDOW_WIDTH * DEFAULT_WINDOW_HEIGHT * BYTES_PER_PIXEL, sizeof (sf::Uint8));

    if (!contentTexture->create (DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT))
        return ErrorCode::ALLOCATION_ERROR;

    for (size_t alphaNumber = 0; alphaNumber < DEFAULT_WINDOW_WIDTH * DEFAULT_WINDOW_HEIGHT; alphaNumber++)
        (*texturePixels) [alphaNumber * BYTES_PER_PIXEL + 3] = 0xff;

    return ErrorCode::NO_ERRORS;
}

