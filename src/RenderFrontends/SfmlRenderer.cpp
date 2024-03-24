#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>

#include "ErrorCode.hpp"
#include "Config.hpp"
#include "RenderBackends/DefaultBackend.hpp"
#include "RenderBackends/ArrayOptimizationsBackend.hpp"
#include "RenderBackends/SimdBackend.hpp"
#include "Timer.hpp"

static ErrorCode InitText      (sf::Text *infoText, sf::Font *font);
static ErrorCode ProcessEvents (sf::RenderWindow *mainWindow, Camera *camera);

ErrorCode SfmlRenderCycle (size_t windowWidth, size_t windowHeight) {
    Camera camera = {.centerX = 0.f, .centerY = 0.f, .scale = 1.f};

    sf::RenderWindow mainWindow (sf::VideoMode (windowWidth, windowHeight), "Mandelbrot", sf::Style::Titlebar | sf::Style::Close);
    sf::Texture contentTexture = {};
    sf::Uint8 *texturePixels = (sf::Uint8 *) calloc (windowWidth * windowHeight * BYTES_PER_PIXEL, sizeof (sf::Uint8));

    if (!contentTexture.create (windowWidth, windowHeight))
        return ErrorCode::ALLOCATION_ERROR;

    for (size_t alphaNumber = 0; alphaNumber < windowWidth * windowHeight; alphaNumber++) {
        texturePixels [alphaNumber * BYTES_PER_PIXEL + 3] = 0xff;
    }

    sf::Sprite contentSprite (contentTexture);

    sf::Font font;
    sf::Text infoText;
    InitText (&infoText, &font);

    InitTimers (TIMERS_COUNT);

    while (mainWindow.isOpen ()) {
        StartTimer (FPS_TIMER);
        if (ProcessEvents (&mainWindow, &camera) != ErrorCode::NO_ERRORS) {
            break;
        }

        mainWindow.clear ();

        StartTimer (RENDER_TIMER);
        UpdateTextureSimd (texturePixels, &camera, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        GetTimerValue (RENDER_TIMER);

        contentTexture.update (texturePixels);
        mainWindow.draw (contentSprite);
        mainWindow.draw (infoText);

        mainWindow.display ();

        infoText.setString (std::to_string (1000 / GetTimerValue (FPS_TIMER)));
    }
    
    DestroyTimers ();
    free (texturePixels);

    return ErrorCode::NO_ERRORS;
}

static ErrorCode InitText (sf::Text *infoText, sf::Font *font) {
    if (!font->loadFromFile ("../fonts/JetBrainsMono-Bold.ttf")) {
        return ErrorCode::FONT_NOT_LOADED;
    }

    infoText->setFont (*font);
    infoText->setCharacterSize (INFO_TEXT_SIZE);
    infoText->setFillColor (sf::Color::Green);
    infoText->setStyle (sf::Text::Bold);
    infoText->setString ("aaa\nbbb");

    return ErrorCode::NO_ERRORS;
}

static ErrorCode ProcessEvents (sf::RenderWindow *mainWindow, Camera *camera) {
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
                    camera->centerY -= Y_SPEED * camera->scale;
                    break;

                case sf::Keyboard::Scancode::A:
                case sf::Keyboard::Scancode::Left:
                    camera->centerX -= X_SPEED * camera->scale;
                    break;

                case sf::Keyboard::Scancode::S:
                case sf::Keyboard::Scancode::Down:
                    camera->centerY += Y_SPEED * camera->scale;
                    break;

                    case sf::Keyboard::Scancode::D:
                    case sf::Keyboard::Scancode::Right:
                        camera->centerX += X_SPEED * camera->scale;
                        break;

                    default:
                        break;
            }
        }
    }

    return ErrorCode::NO_ERRORS;
}
