#include "Game.h"

Representation::Game::Game()
    : _screen_width(RepresentationConstants::screen_width), _screen_height(RepresentationConstants::screen_height),
      _window(new sf::RenderWindow(sf::VideoMode(_screen_width, _screen_height), "SFML_test",
                                   sf::Style::Titlebar | sf::Style::Close)),
      _running(true), _entity_model_creator(new Representation::EntityModelCreator)
{
        _world = std::make_unique<Core::World>(_entity_model_creator, 0, _screen_width, _screen_height, 0);
}

Representation::Game::~Game() = default;

void Representation::Game::run()
{
        while (_running) {
                // sleep
                Core::Stopwatch::getInstance().sleep_frame();

                // sfml events
                handleEvents();

                // physics update
                Core::Stopwatch::getInstance().PhysicsUpdate(std::bind(&Core::World::update, _world.get(), std::placeholders::_1, std::placeholders::_2));

                // fps counter
                _window->setTitle(std::to_string(std::lround(Core::Stopwatch::getInstance().getAverageFps())));

                // draw
                draw();
        }
}

void Representation::Game::draw()
{
        _window->clear(sf::Color(127, 128, 118));

        // draw entityviews
        for (const auto& entity_view : _entity_model_creator->getEntityViews()) {
                _window->draw(entity_view->getSprite());

                // draw hitboxes
                if (RepresentationConstants::draw_hitbox) {
                        _window->draw(entity_view->getHitbox());
                }

                // draw entity _raycasts
                if (RepresentationConstants::draw_raycast) {
                        _window->draw(entity_view->getRaycasts());
                }
        }

        // draw UIviews todo

        _window->display();
}

void Representation::Game::handleEvents()
{
        sf::Event event{};
        while (_window->pollEvent(event)) {
                switch (event.type) {
                case sf::Event::Closed:
                        _window->close();
                        _running = false;
                        break;

                case sf::Event::Resized:
                        _screen_width = _window->getSize().x;
                        _screen_height = _window->getSize().y;
                        break;

                case sf::Event::KeyPressed:
                        handleInput(event, true);
                        break;

                case sf::Event::KeyReleased:
                        handleInput(event, false);
                        break;

                default:
                        break;
                }
        }
}

void Representation::Game::handleInput(const sf::Event& event, bool pressed)
{
        switch (event.key.code) {
        case sf::Keyboard::R:
                _world->getInputMap()->reset = pressed;
                break;

        case sf::Keyboard::W:
                _world->getInputMap()->up = pressed;
                break;

        case sf::Keyboard::S:
                _world->getInputMap()->down = pressed;
                break;

        case sf::Keyboard::A:
                _world->getInputMap()->left = pressed;
                break;

        case sf::Keyboard::D:
                _world->getInputMap()->right = pressed;
                break;

        case sf::Keyboard::C:
                _world->getInputMap()->custom1 = pressed;
                break;

        default:
                break;
        }
}
