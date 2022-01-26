#include "EntityModelCreator.h"
#include "PlayerView.h"

Representation::EntityModelCreator::EntityModelCreator() {}

std::shared_ptr<sf::Texture> Representation::EntityModelCreator::loadTexture(const std::string& file_path)
{
        std::shared_ptr<sf::Texture> new_texture = std::make_shared<sf::Texture>();
        _loaded_textures.push_back(new_texture);

        if (!new_texture->loadFromFile(file_path)) {
                std::cerr << "Couldn't load the texture!" << std::endl;
        }

        return new_texture;
}

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

        player_view->addTexture(loadTexture("assets/sprites/doodle/doodle-jump-right.png"));
        player_view->addTexture(loadTexture("assets/sprites/doodle/doodle-jump-left.png"));
        player_view->addTexture(loadTexture("assets/sprites/doodle/doodle-right.png"));
        player_view->addTexture(loadTexture("assets/sprites/doodle/doodle-left.png"));

        player_view->setTexture(1);

        // link model and view through the observer pattern
        player_model->addObserver(player_view);
        player_model->notifyObservers();

        return player_model;
}

std::shared_ptr<Core::Wall> Representation::EntityModelCreator::createWallModel(std::shared_ptr<Core::Camera> camera,
                                                                                const Core::Vector2f& position,
                                                                                const Core::Vector2f& view_size)
{
        // entity model
        std::shared_ptr<Core::Wall> wall_model(new Core::Wall(camera, position, view_size));

        // entity view
        std::shared_ptr<Representation::EntityView> wall_view(new Representation::EntityView(wall_model));
        std::weak_ptr<Representation::EntityView> wall_view_weak = wall_view;
        _wall_views.push_back(wall_view_weak);

        // entity view textures & animations
        wall_view->addTexture(loadTexture("assets/textures/cobble_stone.png"));

        wall_view->setTexture(0);

        // link model and view through the observer pattern
        wall_model->addObserver(wall_view);
        wall_model->notifyObservers();

        return wall_model;
}

std::shared_ptr<Core::Car> Representation::EntityModelCreator::createCarModel(std::shared_ptr<Core::Camera> camera,
                                                                              const Core::Vector2f& position,
                                                                              const Core::Vector2f& view_size)
{
        // entity model
        std::shared_ptr<Core::Car> car_model(new Core::Car(camera, position, view_size));
        car_model->getHitbox()->setRectangleHitbox(view_size.x / 2.5f, view_size.y * 0.85f);
        car_model->setStatic(false);

        unsigned int raycast_count = 5;

        float angle_step = M_PI / static_cast<float>(raycast_count - 1);
        // raycasts
        for (unsigned int i = 0; i < raycast_count; i++) {
                Core::Vector2f direction{-1, 0};
                std::shared_ptr<Core::Raycast> raycast =
                    std::make_shared<Core::Raycast>(car_model->getPosition(), direction, 1.30);
                raycast->rotate(-angle_step * static_cast<float>(i));
                car_model->addRaycast(raycast);
        }

        // entity view
        std::shared_ptr<Representation::EntityView> car_view(new Representation::EntityView(car_model));
        std::weak_ptr<Representation::EntityView> car_view_weak = car_view;
        _car_views.push_back(car_view_weak);

        // entity view textures & animations
        car_view->addTexture(loadTexture("assets/sprites/car/Audi.png"));

        car_view->setTexture(0);

        // link model and view through the observer pattern
        car_model->addObserver(car_view);
        car_model->notifyObservers();

        return car_model;
}

std::shared_ptr<Core::Car> Representation::EntityModelCreator::createCarModel(std::shared_ptr<Core::Camera> camera,
                                                                              const Core::Vector2f& position,
                                                                              const Core::Vector2f& view_size,
                                                                              const std::string& preset_file_path,
                                                                              const std::string& preset_file_path2)
{
        // entity model
        std::shared_ptr<Core::Car> car_model(new Core::Car(camera, position, view_size));
        car_model->getHitbox()->setRectangleHitbox(view_size.x / 2.5f, view_size.y * 0.85f);
        car_model->setStatic(false);

        car_model->loadPhysicsPreset(preset_file_path);

        unsigned int raycast_count = 5;

        float angle_step = M_PI / static_cast<float>(raycast_count - 1);
        // raycasts
        for (unsigned int i = 0; i < raycast_count; i++) {
                Core::Vector2f direction{-1, 0};
                std::shared_ptr<Core::Raycast> raycast =
                    std::make_shared<Core::Raycast>(car_model->getPosition(), direction, 1.30);
                raycast->rotate(-angle_step * static_cast<float>(i));
                car_model->addRaycast(raycast);
        }

        // entity view
        std::shared_ptr<Representation::EntityView> car_view(new Representation::EntityView(car_model));
        std::weak_ptr<Representation::EntityView> car_view_weak = car_view;
        _car_views.push_back(car_view_weak);

        // entity view textures & animations
        std::map<std::string, std::string> values;
        SLR slr = SLR("assets/SLR_json/Xml2.json");
        slr.ParseXML(values, preset_file_path2);

        car_view->addTexture(loadTexture("assets/sprites/car/" + values["Sprite"]));

        car_view->setTexture(0);

        // link model and view through the observer pattern
        car_model->addObserver(car_view);
        car_model->notifyObservers();

        return car_model;
}

std::shared_ptr<Core::GroundTile> Representation::EntityModelCreator::createGroundTileModel(
    std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size)
{
        // entity model
        std::shared_ptr<Core::GroundTile> ground_tile_model(new Core::GroundTile(camera, position, view_size));

        // entity view
        std::shared_ptr<Representation::EntityView> ground_tile_view(new Representation::EntityView(ground_tile_model));
        std::weak_ptr<Representation::EntityView> ground_tile_view_weak = ground_tile_view;
        _ground_tiles_views.push_back(ground_tile_view_weak);
        ground_tile_model->getHitbox()->setRectangleHitbox(0, 0);

        // entity view textures & animations
        ground_tile_view->addTexture(loadTexture("assets/textures/grass_tile_1.jpg"));

        ground_tile_view->setTexture(0);

        // link model and view through the observer pattern
        ground_tile_model->addObserver(ground_tile_view);
        ground_tile_model->notifyObservers();

        return ground_tile_model;
}

std::vector<std::shared_ptr<Representation::EntityView>> Representation::EntityModelCreator::getEntityViews()
{
        std::vector<std::shared_ptr<Representation::EntityView>> entity_views;

        // order of these functions indicate draw order

        // ground_tiles
        std::vector<std::shared_ptr<Representation::EntityView>> ground_tiles_views =
            getEntityViews(_ground_tiles_views);
        entity_views.insert(entity_views.end(), ground_tiles_views.begin(), ground_tiles_views.end());

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
