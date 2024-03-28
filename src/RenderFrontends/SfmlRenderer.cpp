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

static char *InfoTextBuffer = NULL; // TODO Why.... Or maybe you would like to init it right there, without any init functions wich user may forgot to call

static ErrorCode InitText      (sf::Text *infoText, sf::Font *font);
static ErrorCode InitShader    (sf::Shader *shader);
static ErrorCode InitTexture   (sf::Texture *contentTexture, sf::Uint8 **texturePixels);
static ErrorCode ProcessEvents (sf::RenderWindow *mainWindow, Camera *camera, size_t *currentBackend, size_t *currentGradient);

static char *RenderStatsToString (uint64_t frameTimeValue, uint64_t frameTimeAvg, uint64_t renderTimeValue, uint64_t renderTimeAvg, size_t currentBackend);

#define DestroyEntities() DestroyTimers (); free (texturePixels); free (InfoTextBuffer);

#define InitWithErrorCheck(INIT_FUNCTION) \
    if ((error = INIT_FUNCTION) != ErrorCode::NO_ERRORS) {DestroyEntities (); return error;}

ErrorCode SfmlRenderCycle () {
    ErrorCode error = ErrorCode::NO_ERRORS;

    Camera camera = {.position = sf::Vector2f (0.f, 0.f), .scale = 1.f};

    // TODO I'd move this outside 'cos it has nothing common with "render" or "cycle"
    //      it's just window creation wich take place one time
    //      or move it to like "init" function
    sf::RenderWindow mainWindow (sf::VideoMode (DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT), "Mandelbrot", sf::Style::Titlebar | sf::Style::Close);
    sf::Texture contentTexture = {};
    sf::Uint8 *texturePixels   = NULL;

    InitWithErrorCheck (InitTexture (&contentTexture, &texturePixels)); //TODO Nice! You've just almost invented ctor-exceptions :) 

    sf::Sprite contentSprite (contentTexture);

    sf::Font font;      //TODO Why unitialized? Was that done on purpose?
    sf::Text infoText;
    sf::Text dataText;
    InitWithErrorCheck (InitText (&infoText, &font));

    InitWithErrorCheck (InitTimers (TIMERS_COUNT));

    sf::Shader shader;
    InitWithErrorCheck (InitShader (&shader));

    size_t currentBackend  = 0;
    size_t currentGradient = 0;

    size_t avgNumbers = 0;
    
    uint64_t frameTimeAvg   = 0;
    uint64_t renderTimeAvg  = 0;

    uint64_t frameTimesSum  = 0;
    uint64_t renderTimesSum = 0;

    //TODO: Here `SfmlRenderCycle` should be started
    while (mainWindow.isOpen ()) {
        StartTimer (FPS_TIMER);
        if (ProcessEvents (&mainWindow, &camera, &currentBackend, &currentGradient) != ErrorCode::NO_ERRORS) {
            break;
        }

        mainWindow.clear ();
        
        StartTimer (RENDER_TIMER);

        uint64_t renderTimeValue = 0;

        if (currentBackend < BACKENDS_COUNT) {  // TODO it's unclear to my what does this `if` means
                                                // Why don't you just create cyclic backend switchig?
                                                // Why is that valid to choose 4th backend out of three available?
            AVAILABLE_BACKENDS [currentBackend] (texturePixels, &camera, currentGradient);
            contentTexture.update (texturePixels);

            renderTimeValue = GetTimerValue (RENDER_TIMER);

            mainWindow.draw (contentSprite);
        } else {
            shader.setUniform ("CameraPosition", camera.position);
            shader.setUniform ("Scale", camera.scale);
            mainWindow.draw (contentSprite, &shader);

            renderTimeValue = GetTimerValue (RENDER_TIMER);
        }

        uint64_t frameTimeValue = GetTimerValue (FPS_TIMER);
    
        renderTimesSum += renderTimeValue;
        frameTimesSum  += frameTimeValue;
        avgNumbers++;

        renderTimeAvg = renderTimesSum / (uint64_t) avgNumbers;
        frameTimeAvg  = frameTimesSum  / (uint64_t) avgNumbers;

        if (avgNumbers == AVG_NUMBERS_COUNT) {
            fprintf (stderr, "Avg. frame time (ticks): %lu\nAvg render time (ticks): %lu\n\n", frameTimeAvg, renderTimeAvg);

            renderTimesSum = frameTimesSum = 0;
            avgNumbers     = 0;
        }
        
        infoText.setString (RenderStatsToString (frameTimeValue, frameTimeAvg, renderTimeValue, renderTimeAvg, currentBackend));
        
        mainWindow.draw (infoText);
        mainWindow.display ();
    }
    
    // TODO: also move deinit to main
    DestroyEntities ();

    return ErrorCode::NO_ERRORS;
}

static char *RenderStatsToString (uint64_t frameTimeValue, uint64_t frameTimeAvg, uint64_t renderTimeValue, uint64_t renderTimeAvg, size_t currentBackend) {

    snprintf (InfoTextBuffer, MAX_INFO_TEXT_LENGTH, INFO_TEXT_FORMAT_STRING, 
              frameTimeValue, frameTimeAvg, renderTimeValue, 
              renderTimeAvg, BACKEND_NAMES [currentBackend]);

    return InfoTextBuffer;
}

static inline ErrorCode ProcessEvents (sf::RenderWindow *mainWindow, Camera *camera, size_t *currentBackend, size_t *currentGradient) {
    assert (mainWindow);
    assert (camera);

    #define Key(KEY_CODE) case sf::Keyboard::Scancode::KEY_CODE:
    #define IncrementCameraField(FIELD, VALUE) camera->FIELD += (VALUE) * camera->scale; break
    #define IncrementRenderParameter(PARAMETER, MAX_VALUE) \
        if (*(PARAMETER) < MAX_VALUE) {                    \
            (*(PARAMETER))++;                              \
        } else {                                           \
            *(PARAMETER) = 0;                              \
        }                                                  \
        break

    #define DecrementRenderParameter(PARAMETER, MAX_VALUE) \
        if (*(PARAMETER) > 0) {                            \
            (*(PARAMETER))--;                              \
        } else {                                           \
            *(PARAMETER) = MAX_VALUE;                      \
        }                                                  \
        break


    for (sf::Event event = {}; mainWindow->pollEvent (event);) {
        if (event.type == sf::Event::Closed) {
            mainWindow->close ();
            return ErrorCode::WINDOW_CLOSED;

        } else if (event.type == sf::Event::KeyPressed) {
            switch (event.key.scancode) {
                Key (Equal)  if (camera->scale > MIN_SCALING) {IncrementCameraField (scale, -SCALE_INCREMENT);} break; //TODO it's better to make such a long case-statements multiline with `{}` block
                Key (Hyphen) if (camera->scale < MAX_SCALING) {IncrementCameraField (scale,  SCALE_INCREMENT);} break;

                Key (W) Key (Up)    IncrementCameraField (position.y, -Y_SPEED);
                Key (A) Key (Left)  IncrementCameraField (position.x, -X_SPEED);
                Key (S) Key (Down)  IncrementCameraField (position.y,  Y_SPEED);
                Key (D) Key (Right) IncrementCameraField (position.x,  X_SPEED);

                Key (J) DecrementRenderParameter (currentBackend,  BACKENDS_COUNT); // TODO oh, you're little vim enjoyer
                Key (K) IncrementRenderParameter (currentBackend,  BACKENDS_COUNT);
                Key (H) DecrementRenderParameter (currentGradient, GRADIENTS_COUNT - 1);
                Key (L) IncrementRenderParameter (currentGradient, GRADIENTS_COUNT - 1);
 
                default:
                    break;
            }
        }
    }

    #undef Key
    #undef IncrementCameraField
    #undef IncrementRenderParameter
    #undef DecrementRenderParameter

    return ErrorCode::NO_ERRORS;
}

static ErrorCode InitText (sf::Text *infoText, sf::Font *font) {
    assert (infoText);
    assert (font);

    if (!font->loadFromFile ("../fonts/JetBrainsMono-Bold.ttf")) { // TODO: Same issue with path. Check next TODO
        return ErrorCode::FONT_NOT_LOADED;
    }

    infoText->setFont (*font);
    infoText->setCharacterSize (INFO_TEXT_SIZE);
    infoText->setFillColor (sf::Color::Green);
    infoText->setStyle (sf::Text::Bold);
    infoText->setString ("");
    
    InfoTextBuffer = (char *) calloc (MAX_INFO_TEXT_LENGTH, sizeof (char)); // TODO is this a fucking global variable??? You wanna me to got an heart attack?

    return ErrorCode::NO_ERRORS;
}

static ErrorCode InitShader (sf::Shader *shader) {
    assert (shader);

    if (!shader->loadFromFile ("../shaders/shader.glsl", sf::Shader::Fragment)) // TODO: path like that will not always work. 
                                                                                // Try to run your program like `./build/bin/program`. 
                                                                                // Then it will try to search shader in ./build/../.... directory
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
        (*texturePixels) [alphaNumber * BYTES_PER_PIXEL + 3] = 0xff;    // TODO: weird magic constants. Create `const int` for them

    return ErrorCode::NO_ERRORS;
}

