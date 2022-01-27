#ifndef UABA2_AP_PROJECT_GAME_H
#define UABA2_AP_PROJECT_GAME_H

#include "../game_logic/World.h"
#include "RepresentationConstants.h"
#include "entities/EntityModelCreator.h"
#include "entities/EntityView.h"
#include <SFML/Graphics.hpp>

namespace Representation {
class Game
{
private:
        unsigned int _screen_width;
        unsigned int _screen_height;
        bool _running;

        std::unique_ptr<sf::RenderWindow> _window;

        std::unique_ptr<Core::World> _world;
        std::shared_ptr<Representation::EntityModelCreator> _entity_model_creator;

public:
        Game();

        ~Game();

        void run();

private:
        void draw();

        void handleEvents();

        void handleKeyboardInput(const sf::Event& event, bool pressed);

        void handleMouseInput(const sf::Event& event, bool pressed);

        void resizeWindow(unsigned int screen_width, unsigned int screen_height);
};
} // namespace Representation

#endif // UABA2_AP_PROJECT_GAME_H
