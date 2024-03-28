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
#include <unistd.h>
#include <libgen.h>

#include "ColorGradients.hpp"
#include "ErrorCode.hpp"
#include "Config.hpp"
#include "RenderBackends/BackendCommons.hpp"
#include "RenderFrontends/SfmlRenderer.hpp"
#include "RenderFrontends/RenderContext.hpp"
#include "Timer.hpp"

static ErrorCode ProcessEvents (RenderContext *context);

static char *RenderStatsToString (RenderContext *context, uint64_t frameTimeValue, uint64_t frameTimeAvg, uint64_t renderTimeValue, uint64_t renderTimeAvg);

ErrorCode SfmlRenderCycle (RenderContext *context) {
    assert (context);

    size_t avgNumbers = 0;
    
    uint64_t frameTimeAvg   = 0;
    uint64_t renderTimeAvg  = 0;

    uint64_t frameTimesSum  = 0;
    uint64_t renderTimesSum = 0;

    while (context->mainWindow.isOpen ()) {
        StartTimer (FPS_TIMER);
        if (ProcessEvents (context) != ErrorCode::NO_ERRORS) {
            break;
        }

        context->mainWindow.clear ();
        
        StartTimer (RENDER_TIMER);

        uint64_t renderTimeValue = 0;

        if (context->currentBackend < CPU_BACKENDS_COUNT) {
            AVAILABLE_BACKENDS [context->currentBackend] (context->texturePixels, &context->camera, context->currentGradient);
            context->contentTexture.update (context->texturePixels);

            renderTimeValue = GetTimerValue (RENDER_TIMER);

            context->mainWindow.draw (context->contentSprite);
        } else {
            context->shader.setUniform ("CameraPosition", context->camera.position);
            context->shader.setUniform ("Scale", context->camera.scale);
            context->mainWindow.draw   (context->contentSprite, &context->shader);

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
        
        context->infoText.setString (RenderStatsToString (context, frameTimeValue, frameTimeAvg, renderTimeValue, renderTimeAvg));
        
        context->mainWindow.draw (context->infoText);
        context->mainWindow.display ();
    }
    
    return ErrorCode::NO_ERRORS;
}

static char *RenderStatsToString (RenderContext *context, uint64_t frameTimeValue, uint64_t frameTimeAvg, uint64_t renderTimeValue, uint64_t renderTimeAvg) {

    snprintf (context->infoTextBuffer, MAX_INFO_TEXT_LENGTH, INFO_TEXT_FORMAT_STRING, 
              frameTimeValue, frameTimeAvg, renderTimeValue, 
              renderTimeAvg, BACKEND_NAMES [context->currentBackend]);

    return context->infoTextBuffer;
}

static ErrorCode ProcessEvents (RenderContext *context) {
    assert (context->mainWindow);
    assert (context->camera);

    #define Key(KEY_CODE) case sf::Keyboard::Scancode::KEY_CODE:
    #define IncrementCameraField(FIELD, VALUE) context->camera.FIELD += (VALUE) * context->camera.scale; break
    #define IncrementRenderParameter(PARAMETER, MAX_VALUE) \
        if (context->PARAMETER < MAX_VALUE) {              \
            context->PARAMETER++;                          \
        } else {                                           \
            context->PARAMETER = 0;                        \
        }                                                  \
        break

    #define DecrementRenderParameter(PARAMETER, MAX_VALUE) \
        if (context->PARAMETER > 0) {                      \
            context->PARAMETER--;                          \
        } else {                                           \
            context->PARAMETER = MAX_VALUE;                \
        }                                                  \
        break


    for (sf::Event event = {}; context->mainWindow.pollEvent (event);) {
        if (event.type == sf::Event::Closed) {
            context->mainWindow.close ();
            return ErrorCode::WINDOW_CLOSED;

        } else if (event.type == sf::Event::KeyPressed) {
            switch (event.key.scancode) {
                Key (Equal) {
                    if (context->camera.scale > MIN_SCALING) {
                        IncrementCameraField (scale, -SCALE_INCREMENT);
                    }
                    break;
                }

                Key (Hyphen) {
                    if (context->camera.scale < MAX_SCALING) {
                        IncrementCameraField (scale,  SCALE_INCREMENT);
                    } 
                    break;
                }

                Key (W) Key (Up)    IncrementCameraField (position.y, -Y_SPEED);
                Key (A) Key (Left)  IncrementCameraField (position.x, -X_SPEED);
                Key (S) Key (Down)  IncrementCameraField (position.y,  Y_SPEED);
                Key (D) Key (Right) IncrementCameraField (position.x,  X_SPEED);

                Key (J) DecrementRenderParameter (currentBackend,  CPU_BACKENDS_COUNT);
                Key (K) IncrementRenderParameter (currentBackend,  CPU_BACKENDS_COUNT);
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


