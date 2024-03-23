#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "ErrorCode.hpp"
#include "Config.hpp"

ErrorCode SfmlRenderCycle (size_t windowWidth, size_t windowHeight) {
    sf::RenderWindow mainWindow (sf::VideoMode (windowWidth, windowHeight), "Mandelbrot");
    sf::Texture contentTexture = {};
    sf::Uint8 *texturePixels = (sf::Uint8 *) calloc (windowWidth * windowHeight * BYTES_PER_PIXEL, sizeof (sf::Uint8));

    if (!contentTexture.create (windowWidth, windowHeight))
        return ErrorCode::ALLOCATION_ERROR;

    sf::Sprite contentSprite (contentTexture);

    while (mainWindow.isOpen ())
    {
        for (sf::Event event = {}; mainWindow.pollEvent (event);)
        {
            if (event.type == sf::Event::Closed)
            {
                mainWindow.close ();
            }
        }


        contentTexture.update (texturePixels);
        mainWindow.draw (contentSprite);

        mainWindow.display ();
    }

    return ErrorCode::NO_ERRORS;
}
