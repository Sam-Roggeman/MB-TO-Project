#include "Game.h"

Representation::Game::Game()
    : _screen_width(RepresentationConstants::screen_width), _screen_height(RepresentationConstants::screen_height),
      _running(true), _entity_model_creator(new Representation::EntityModelCreator)
{
        _world = std::make_unique<Core::World>(_entity_model_creator, 0, _screen_width, _screen_height, 0);
        _window = std::make_unique<sf::RenderWindow>(sf::VideoMode(_screen_width, _screen_height), "SFML_test");
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
                Core::Stopwatch::getInstance().PhysicsUpdate(
                    std::bind(&Core::World::update, _world.get(), std::placeholders::_1, std::placeholders::_2));

                // fps counter
                //                _window->setTitle(std::to_string(std::lround(Core::Stopwatch::getInstance().getAverageFps())));
                _window->setTitle(_world->getInputMap()->mouse_pos_world.toString());

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
                        resizeWindow(_window->getSize().x, _window->getSize().y);
                        break;

                case sf::Event::KeyPressed:
                        handleKeyboardInput(event, true);
                        break;

                case sf::Event::KeyReleased:
                        handleKeyboardInput(event, false);
                        break;

                case sf::Event::MouseButtonPressed:
                        handleMouseInput(event, true);
                        break;

                case sf::Event::MouseButtonReleased:
                        handleMouseInput(event, false);
                        break;

                case sf::Event::MouseMoved:
                        _world->getInputMap()->mouse_pos_world = _world->getCamera()->projectCoordinateCustomToWorld(
                            {static_cast<float>(sf::Mouse::getPosition(*_window).x),
                             static_cast<float>(sf::Mouse::getPosition(*_window).y)},
                            0, static_cast<float>(_screen_width), static_cast<float>(_screen_height), 0);
                        _world->getInputMap()->mouse_pos_representation_window = {static_cast<float>(sf::Mouse::getPosition(*_window).x),
                                                                                   static_cast<float>(sf::Mouse::getPosition(*_window).y)};
                        break;

                case sf::Event::MouseWheelScrolled:
                        _world->getInputMap()->mouse_wheel_delta = event.mouseWheelScroll.delta;
                        break;

                default:
                        break;
                }
        }
}

void Representation::Game::handleKeyboardInput(const sf::Event& event, bool pressed)
{
        switch (event.key.code) {
        case sf::Keyboard::R:
                _world->getInputMap()->r = pressed;
                break;

        case sf::Keyboard::W:
                _world->getInputMap()->w = pressed;
                break;

        case sf::Keyboard::S:
                _world->getInputMap()->s = pressed;
                break;

        case sf::Keyboard::A:
                _world->getInputMap()->a = pressed;
                break;

        case sf::Keyboard::D:
                _world->getInputMap()->d = pressed;
                break;

        case sf::Keyboard::Z:
                _world->getInputMap()->z = pressed;
                break;

        case sf::Keyboard::X:
                _world->getInputMap()->x = pressed;
                break;

        case sf::Keyboard::C:
                _world->getInputMap()->c = pressed;
                break;

        case sf::Keyboard::V:
                _world->getInputMap()->v = pressed;
                break;

        case sf::Keyboard::B:
                _world->getInputMap()->b = pressed;
                break;

        case sf::Keyboard::F:
                _world->getInputMap()->f = pressed;

        default:
                break;
        }
}

void Representation::Game::handleMouseInput(const sf::Event& event, bool pressed)
{
        switch (event.mouseButton.button) {
        case sf::Mouse::Left:
                _world->getInputMap()->mouse_button_left = pressed;
                break;

        case sf::Mouse::Middle:
                _world->getInputMap()->mouse_button_middle = pressed;
                break;

        case sf::Mouse::Right:
                _world->getInputMap()->mouse_button_right = pressed;
                break;

        default:
                break;
        }
}

void Representation::Game::resizeWindow(unsigned int screen_width, unsigned int screen_height)
{
        sf::View view = _window->getDefaultView();
        view.setSize({static_cast<float>(screen_width), static_cast<float>(screen_height)});

        float _width_resize =
            static_cast<float>(screen_width) - static_cast<float>(RepresentationConstants::screen_width);
        float _height_resize =
            static_cast<float>(screen_height) - static_cast<float>(RepresentationConstants::screen_height);

        _screen_width = screen_width;
        _screen_height = screen_height;

        _window->setView(view);
        _world->getCamera()->setRepresentationBounderies(
            0 - _width_resize / 2, static_cast<float>(screen_width) - _width_resize / 2,
            static_cast<float>(screen_height) - _height_resize / 2, 0 - _height_resize / 2);
}