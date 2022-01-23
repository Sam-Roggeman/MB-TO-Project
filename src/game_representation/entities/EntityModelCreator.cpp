#include "EntityModelCreator.h"
#include "PlayerView.h"

Representation::EntityModelCreator::EntityModelCreator() {}

std::shared_ptr<Core::Doodle> Representation::EntityModelCreator::createDoodleModel(
    std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size)
{
        // entity model
        std::shared_ptr<Core::Doodle> player_model(new Core::Doodle(camera, position, view_size));
        player_model->getHitbox()->setRectangleHitbox(0.2, 0.3);
        player_model->setStatic(false);

        Core::Vector2f direction{0, -1};
        std::shared_ptr<Core::Raycast> raycast =
            std::make_shared<Core::Raycast>(player_model->getPosition(), direction, 0.2);

        player_model->addRaycast(raycast);

        // entity view
        std::shared_ptr<Representation::EntityView> player_view(new Representation::PlayerView(player_model));
        std::weak_ptr<Representation::EntityView> player_view_weak = player_view;
        _player_views.push_back(player_view_weak);

        player_view->addTexture("assets/sprites/doodle/doodle-jump-right.png");
        player_view->addTexture("assets/sprites/doodle/doodle-jump-left.png");
        player_view->addTexture("assets/sprites/doodle/doodle-right.png");
        player_view->addTexture("assets/sprites/doodle/doodle-left.png");

        player_view->setTexture(0);

        // link model and view through the observer pattern
        player_model->addObserver(player_view);

        return player_model;
}

std::shared_ptr<Core::Wall> Representation::EntityModelCreator::createWallModel(std::shared_ptr<Core::Camera> camera,
                                                                                const Core::Vector2f& position,
                                                                                const Core::Vector2f& view_size)
{
        // entity model
        std::shared_ptr<Core::Wall> platform_model(new Core::Wall(camera, position, view_size));

        // entity view
        std::shared_ptr<Representation::EntityView> platform_view(new Representation::EntityView(platform_model));
        std::weak_ptr<Representation::EntityView> platform_view_weak = platform_view;
        _wall_views.push_back(platform_view_weak);

        // entity view textures & animations
        platform_view->addTexture("assets/textures/cobble_stone.png");

        platform_view->setTexture(0);

        // link model and view through the observer pattern
        platform_model->addObserver(platform_view);

        return platform_model;
}

std::shared_ptr<Core::Car> Representation::EntityModelCreator::createCarModel(std::shared_ptr<Core::Camera> camera,
                                                                              const Core::Vector2f& position,
                                                                              const Core::Vector2f& view_size)
{
        // entity model
        std::shared_ptr<Core::Car> car_model(new Core::Car(camera, position, view_size));
        car_model->getHitbox()->setRectangleHitbox(view_size.x / 2.5f, view_size.y * 0.85f);
        car_model->setStatic(false);
        car_model->setBackPivotPoint({position.x, position.y - (view_size.y / 2)});
        car_model->setFrontPivotPoint({position.x, position.y + (view_size.y / 2)});

        unsigned int raycast_count = 5;

        float angle_step = 180.f / static_cast<float>(raycast_count - 1);
        // raycasts
        for (unsigned int i = 0; i < raycast_count; i++) {
                Core::Vector2f direction{-1, 0};
                std::shared_ptr<Core::Raycast> raycast = std::make_shared<Core::Raycast>(car_model->getPosition(), direction, 1.30);
                raycast->rotate(-angle_step * static_cast<float>(i));
                car_model->addRaycast(raycast);
        }

        // entity view
        std::shared_ptr<Representation::EntityView> car_view(new Representation::EntityView(car_model));
        std::weak_ptr<Representation::EntityView> car_view_weak = car_view;
        _car_views.push_back(car_view_weak);

        // entity view textures & animations
        car_view->addTexture("assets/sprites/car/Audi.png");

        car_view->setTexture(0);

        // link model and view through the observer pattern
        car_model->addObserver(car_view);

        return car_model;
}

std::vector<std::shared_ptr<Representation::EntityView>> Representation::EntityModelCreator::getEntityViews()
{
        std::vector<std::shared_ptr<Representation::EntityView>> entity_views;

        // walls
        std::vector<std::shared_ptr<Representation::EntityView>> wall_views = getEntityViews(_wall_views);
        entity_views.insert(entity_views.end(), wall_views.begin(), wall_views.end());

        // cars
        std::vector<std::shared_ptr<Representation::EntityView>> car_views = getEntityViews(_car_views);
        entity_views.insert(entity_views.end(), car_views.begin(), car_views.end());

        // players
        std::vector<std::shared_ptr<Representation::EntityView>> player_views = getEntityViews(_player_views);
        entity_views.insert(entity_views.end(), player_views.begin(), player_views.end());

        return entity_views;
}

std::vector<std::shared_ptr<Representation::EntityView>> Representation::EntityModelCreator::getEntityViews(
    std::vector<std::weak_ptr<Representation::EntityView>>& entity_views)
{
        std::vector<std::shared_ptr<Representation::EntityView>> shared_entity_views;

        std::vector<std::weak_ptr<Representation::EntityView>> new_entity_views;
        for (const auto& entity_view : entity_views) {
                std::shared_ptr<Representation::EntityView> platform_view_shared = entity_view.lock();
                if (platform_view_shared) {
                        shared_entity_views.push_back(platform_view_shared);
                        new_entity_views.push_back(entity_view);
                }
        }
        entity_views = new_entity_views;

        return shared_entity_views;
}
