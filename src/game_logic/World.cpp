#include "World.h"
#include "utils/Random.h"
namespace Core {
World::World(std::shared_ptr<IEntityModelCreator> entity_model_creator, float x_min, float x_max, float y_min,
             float y_max)
    : _entity_model_creator(std::move(entity_model_creator)), _camera(new Camera), _user_input_map(new UserInputMap),
      _generation(0), _generation_time(0), _time_limit(CoreConstants::time_limit),
      _spawn_location(CoreConstants::spawn_location), _spawn_direction(CoreConstants::spawn_direction),
      _previous_focus_state(false)
{
        // set the camera
        _camera->setRepresentationBounderies(x_min, x_max, y_min, y_max);

        // load the map
        generateGroundTiles(5);
#ifdef WIN32
        //        generateMapFromImage("assets/maps/Untitled3.png", 5);
        loadMap("assets/maps/world_.save");
#else
        loadMap("assets/maps/world_.save");
//        generateTestMap();
#endif
        // spawn the player
        if (CoreConstants::generate_player) {
                _player = _entity_model_creator->createCarModel(_camera, _spawn_location, {0.2, 0.2},
                                                                "assets/car_presets/physics_preset_1.xml",
                                                                "assets/car_presets/sprite_preset_1.xml");
                _player->setCameraFocus(true);
        }

        _populationCars = 3; // MOET onEVEN ZIJN!!!!!!!!!!!!!!!!!!!!!!!!!!!
        _mutationRate = 60;

        generateCars(_spawn_location, _spawn_direction.normalized(), _populationCars,
                     "assets/car_presets/physics_preset_1.xml", "assets/car_presets/sprite_preset_1.xml");
}

World::~World() { saveMap("assets/maps/world_last_run.save"); }

void World::update(double t, float dt)
{
        // player control
        controlPlayer(t, dt);

        // gamespeed
        controlGameSpeed(t, dt);

        // camera
        updateCamera(t, dt);

        // AI
        updateAI(t, dt);

        // updates
        updateEntities(t, dt);

        // collisions
        checkCollisions(t, dt);
}

std::shared_ptr<UserInputMap> World::getInputMap() { return _user_input_map; }

std::shared_ptr<Camera> World::getCamera() { return _camera; }

void World::saveMap(const std::string& filepath) const
{
        ofstream myfile{filepath};

        // start
        myfile << "Start:\t\t\t\t\t\t\t\t#(Position.x,Position.y);(Direction.x,Direction.y)" << std::endl;
        myfile << _spawn_location << ';' << _spawn_direction << std::endl;

        // finishline
        myfile << "FinishLine:\t\t\t\t\t\t\t#(Position.x,Position.y);(Direction.x,Direction.y)" << std::endl;
        myfile << _finish_line->getPosition() << ';' << _finish_line->getRaycast(0)->getDirection() << std::endl;

        // checkpoints
        myfile << "CheckPoints:\t\t\t\t\t\t#(Position.x,Position.y);(Direction.x,Direction.y);(Length)" << std::endl;
        for (const std::shared_ptr<Core::Checkpoint>& checkpoint : _checkpoints) {
                myfile << checkpoint->getPosition() << ';' << checkpoint->getRaycast(0)->getDirection() << ";("
                       << checkpoint->getRaycast(0)->getLength() << ')' << std::endl;
        }

        // walls
        myfile << "Walls:\t\t\t\t\t\t\t\t#(Position.x,Position.y);(ViewSize.x,ViewSize.y)" << std::endl;
        for (const std::shared_ptr<Core::Wall>& wall : _walls) {
                myfile << wall->getPosition() << ';' << wall->getAbsoluteViewSize() << std::endl;
        }
        myfile.close();
}

void World::loadMap(const std::string& filepath)
{
        string line{}, viewsize_string{}, direction_string{}, length_string{};
        ifstream myfile(filepath);
        Vector2f viewsize{}, position{}, direction{};
        float length{};

        // sorted common case -> least common
        enum loading_state
        {
                loading_walls,
                loading_checkpoints,
                loading_finish,
                loading_start,
        };

        loading_state current_loadingstate{};

        if (myfile.is_open()) {
                while (getline(myfile, line)) {
                        if (line.find("Start:") < line.size()) {
                                current_loadingstate = loading_start;
                                continue;
                        } else if (line.find("FinishLine:") < line.size()) {
                                current_loadingstate = loading_finish;
                                continue;
                        } else if (line.find("CheckPoints:") < line.size()) {
                                current_loadingstate = loading_checkpoints;
                                continue;
                        } else if (line.find("Walls:") < line.size()) {
                                current_loadingstate = loading_walls;
                                continue;
                        }
                        switch (current_loadingstate) {
                        case loading_walls:
                                position = {std::stof(line.substr(1, line.find(','))),
                                            std::stof(line.substr(line.find(',') + 1, line.find(')')))};
                                viewsize_string = line.substr(line.find(';') + 1, std::string::npos);
                                viewsize = {std::stof(viewsize_string.substr(viewsize_string.find('(') + 1,
                                                                             viewsize_string.find(','))),
                                            std::stof(viewsize_string.substr(viewsize_string.find(',') + 1,
                                                                             viewsize_string.find(')')))};
                                _walls.emplace_back(
                                    _entity_model_creator->createWallModel(_camera, position, viewsize));
                                break;
                        case loading_checkpoints:
                                position = {std::stof(line.substr(1, line.find(','))),
                                            std::stof(line.substr(line.find(',') + 1, line.find(')')))};
                                direction_string = line.substr(line.find(';') + 1, std::string::npos);
                                length_string =
                                    direction_string.substr(direction_string.find(';') + 1, std::string::npos);
                                direction = {std::stof(direction_string.substr(direction_string.find('(') + 1,
                                                                               direction_string.find(','))),
                                             std::stof(direction_string.substr(direction_string.find(',') + 1,
                                                                               direction_string.find(')')))};
                                length = std::stof(
                                    length_string.substr(length_string.find('(') + 1, length_string.find(')')));
                                _checkpoints.emplace_back(_entity_model_creator->createCheckpointModel(
                                    _camera, position, {}, direction.normalized(), length));
                                _checkpoints.at(_checkpoints.size() - 1)->setShowRaycast(true);
                                break;
                        case loading_finish:
                                position = {std::stof(line.substr(1, line.find(','))),
                                            std::stof(line.substr(line.find(',') + 1, line.find(')')))};
                                direction_string = line.substr(line.find(';') + 1, std::string::npos);
                                direction = {std::stof(direction_string.substr(direction_string.find('(') + 1,
                                                                               direction_string.find(','))),
                                             std::stof(direction_string.substr(direction_string.find(',') + 1,
                                                                               direction_string.find(')')))};
                                _finish_line = _entity_model_creator->createCheckpointModel(
                                    _camera, position, {}, direction.normalized(), direction.length());
                                _finish_line->setShowRaycast(true);
                                break;
                        case loading_start:
                                position = {std::stof(line.substr(1, line.find(','))),
                                            std::stof(line.substr(line.find(',') + 1, line.find(')')))};
                                direction_string = line.substr(line.find(';') + 1, std::string::npos);
                                direction = {std::stof(direction_string.substr(direction_string.find('(') + 1,
                                                                               direction_string.find(','))),
                                             std::stof(direction_string.substr(direction_string.find(',') + 1,
                                                                               direction_string.find(')')))};
                                _spawn_location = position;
                                _spawn_direction = direction;
                                break;
                        }
                }
                myfile.close();
        }

        else
                cout << "Unable to open file" << std::endl;
}

void World::generateGroundTiles(float scale)
{
        // ground tile size
        Vector2f size(1, 1);

        float width = _camera->getCamerawidth() * scale;
        float height = _camera->getCameraheight() * scale;

        Vector2f current_position = _camera->getPosition() - Vector2f(width / 2, height / 2);
        float y_reset = current_position.y;

        for (int i = 0; i <= std::lround(width / size.x); i++) {
                for (int j = 0; j <= std::lround(height / size.y); j++) {
                        std::shared_ptr<GroundTile> ground_tile =
                            _entity_model_creator->createGroundTileModel(_camera, current_position, size);
                        _ground_tiles.push_back(ground_tile);

                        current_position.y += size.y;
                }

                current_position.y = y_reset;
                current_position.x += size.x;
        }
}

#ifdef WIN32
void World::generateMapFromImage(const std::string& inputname, float scale)
{
        imageProcessor imageProcessor{inputname};
        {
                unsigned int square_r = std::max(imageProcessor.getRows() / 30, 1u);
                unsigned int wallpixels{};
                Core::Vector2f wall_pos{}, wall_size{};
                unsigned threshold = (unsigned int)((float)square_r * (float)square_r) / 50;

                // generate walls
                for (unsigned int base_row = 0; base_row < imageProcessor.getRows() - square_r; base_row += square_r) {
                        for (unsigned int base_col = 0; base_col < imageProcessor.at(base_row).size() - square_r;
                             base_col += square_r) {
                                wallpixels = 0;
                                for (unsigned int row_offset = 0; row_offset < square_r; row_offset++) {
                                        for (unsigned int col_offset = 0; col_offset < square_r; col_offset++) {
                                                if (imageProcessor.isWall(row_offset + base_row,
                                                                          col_offset + base_col)) {
                                                        wallpixels++;
                                                }
                                        }
                                }
                                if (wallpixels >= threshold) {
                                        wall_size = Core::Vector2f((float)square_r, (float)square_r);
                                        wall_size.y = -wall_size.y;
                                        wall_pos = Core::Vector2f((float)base_col + (float)square_r / 2,
                                                                  ((float)base_row + (float)square_r / 2));
                                        _walls.emplace_back(_entity_model_creator->createWallModel(
                                            _camera,
                                            scale * _camera->projectCoordinateCustomToWorld(
                                                        wall_pos, 0.0f, (float)imageProcessor.getColumns(),
                                                        (float)imageProcessor.getRows(), 0.0f),
                                            scale * _camera->projectSize(wall_size, 0.0f,
                                                                         (float)imageProcessor.getColumns(), 0.0f,
                                                                         (float)imageProcessor.getRows())));
                                }
                        }
                }
        }
        meltWalls();
        {
                Core::Vector2f checkpoint_pos{}, checkpoint_vector{};
                std::shared_ptr<Checkpoint> new_checkpoint{};

                // checkpoints
                for (const std::pair<Vector2f, Vector2f>& check_pts_pair : imageProcessor.getCheckPoints()) {
                        checkpoint_vector = check_pts_pair.second - check_pts_pair.first;
                        checkpoint_vector.y = -checkpoint_vector.y;

                        checkpoint_pos = scale * _camera->projectCoordinateCustomToWorld(
                                                     check_pts_pair.first, 0.0f, (float)imageProcessor.getColumns(),
                                                     (float)imageProcessor.getRows(), 0.0f);
                        checkpoint_vector =
                            scale * _camera->projectSize(checkpoint_vector, 0.0f, (float)imageProcessor.getColumns(),
                                                         0.0f, (float)imageProcessor.getRows());
                        new_checkpoint = _entity_model_creator->createCheckpointModel(
                            _camera, checkpoint_pos, {}, checkpoint_vector.normalized(), checkpoint_vector.length());
                        new_checkpoint->setShowRaycast(true);
                        _checkpoints.push_back(new_checkpoint);
                }
        }
        {
                // finish line
                std::shared_ptr<Checkpoint> new_finish;
                Vector2f finish_point{imageProcessor.getEndlineV1()};
                Vector2f finish_vector{};
                finish_vector = imageProcessor.getEndlineV2() - finish_point;
                finish_vector.y = -finish_vector.y;

                finish_point = scale * _camera->projectCoordinateCustomToWorld(finish_point, 0.0f,
                                                                               (float)imageProcessor.getColumns(),
                                                                               (float)imageProcessor.getRows(), 0.0f);
                finish_vector = scale * _camera->projectSize(finish_vector, 0.0f, (float)imageProcessor.getColumns(),
                                                             0.0f, (float)imageProcessor.getRows());
                new_finish = _entity_model_creator->createCheckpointModel(
                    _camera, finish_point, {}, finish_vector.normalized(), finish_vector.length());
                new_finish->setShowRaycast(true);
                _finish_line = new_finish;
        }
        {
                Vector2f direction{imageProcessor.getDirection()};
                _spawn_location = scale * _camera->projectCoordinateCustomToWorld(
                                              imageProcessor.getStartPoint(), 0.0f, (float)imageProcessor.getColumns(),
                                              (float)imageProcessor.getRows(), 0.0f);
                direction.y = -direction.y;
                _spawn_direction = (scale * _camera->projectSize(direction, 0.0f, (float)imageProcessor.getColumns(),
                                                                 (float)imageProcessor.getRows(), 0.0f))
                                       .normalized();
        }
}
#else
void World::generateMapFromImage(const std::string& inputname, float scale)
{
        std::cerr << "Couldn't initialize walls." << std::endl;
}
#endif

void World::meltWalls()
{
        meltColumns();
        meltRows();
}

void World::meltRows()
{
        bool melted_wall = false;
        std::shared_ptr<Wall> wall1_ptr{}, wall2_ptr{};
        Vector2f diff{};
        float lower_middle_border{}, higher_middle_border{}, highest_border{}, lowest_border{};

        for (unsigned int wall1_ind = 0; wall1_ind < _walls.size() - 1; wall1_ind++) {
                //                wall1_it = std::next(_walls.begin(),wall1_ind);
                //                std::cout << "wall1 index: " << wall1_ind
                //                <<"\n\tcenter:\t\t"<<wall1_it->get()->getPosition()<<
                //                "\n\tviewsize:\t"<<wall1_it->get()->getAbsoluteViewSize() << "\n"; std::cout << "right
                //                border: " <<
                //                wall1_it->get()->getPosition().x+wall1_it->get()->getAbsoluteViewSize().x/2 << "\nleft
                //                border: " <<
                //                wall1_it->get()->getPosition().x-wall1_it->get()->getAbsoluteViewSize().x/2 << "\n";
                for (unsigned int wall2_ind = wall1_ind + 1; wall2_ind < _walls.size(); wall2_ind++) {
                        //                        wall2_it = std::next(_walls.begin(),wall2_ind);
                        //                        std::cout << "wall2 index: " << wall2_ind
                        //                        <<"\n\tcenter:\t\t"<<wall2_it->get()->getPosition()<<
                        //                        "\n\tviewsize:\t"<<wall2_it->get()->getAbsoluteViewSize() << "\n";
                        wall1_ptr = *std::next(_walls.begin(), wall1_ind);
                        wall2_ptr = *std::next(_walls.begin(), wall2_ind);

                        diff = {wall2_ptr->getPosition().x - wall1_ptr->getPosition().x,
                                wall1_ptr->getPosition().y - wall2_ptr->getPosition().y};

                        lower_middle_border =
                            std::min(wall1_ptr->getPosition().y + wall1_ptr->getAbsoluteViewSize().y / 2.0f,
                                     wall2_ptr->getPosition().y + wall2_ptr->getAbsoluteViewSize().y / 2.0f);
                        higher_middle_border =
                            std::max(wall1_ptr->getPosition().y - wall1_ptr->getAbsoluteViewSize().y / 2.0f,
                                     wall2_ptr->getPosition().y - wall2_ptr->getAbsoluteViewSize().y / 2.0f);
                        //                        if (wall2_ind == 8){
                        //                                std::cout << "";
                        //                        }
                        //                        std::cout << "right border: " <<
                        //                        wall2_ptr->getPosition().x+wall2_ptr->getAbsoluteViewSize().x/2 <<
                        //                        "\nleft border: " <<
                        //                        wall2_ptr->getPosition().x-wall2_ptr->getAbsoluteViewSize().x/2 <<
                        //                        "\n";
                        // check for row
                        if (std::abs(diff.x) < 0.00001f &&
                            std::abs(lower_middle_border - higher_middle_border) < 0.00001f) {
                                //                                std::cout << "\t\t\tmerged: " << wall1_ind << " and "
                                //                                <<wall2_ind<<"\n";

                                lowest_border =
                                    std::min(wall1_ptr->getPosition().y - wall1_ptr->getAbsoluteViewSize().y / 2.0f,
                                             wall2_ptr->getPosition().y - wall2_ptr->getAbsoluteViewSize().y / 2.0f);
                                highest_border =
                                    std::max(wall1_ptr->getPosition().y + wall1_ptr->getAbsoluteViewSize().y / 2.0f,
                                             wall2_ptr->getPosition().y + wall2_ptr->getAbsoluteViewSize().y / 2.0f);

                                wall1_ptr->setScale(
                                    {wall1_ptr->getScale().x, wall2_ptr->getScale().y + wall1_ptr->getScale().y});
                                wall1_ptr->setPosition(
                                    {wall1_ptr->getPosition().x, (lowest_border + highest_border) / 2.0f});
                                _walls.erase(std::find(_walls.begin(), _walls.end(), wall2_ptr));
                                melted_wall = true;
                                //                                std::cout << std::endl;
                        }
                        // check for column
                        //                        if (){
                        //                                melted_wall = true;
                        //                        }
                }
                //                std::cout << std::endl;
        }
        if (melted_wall)
                meltRows();
}
void World::meltColumns()
{
        bool melted_wall = false;
        std::shared_ptr<Wall> wall1_ptr{}, wall2_ptr{};
        Vector2f diff{};
        float lower_middle_border{}, higher_middle_border{}, highest_border{}, lowest_border{};

        for (unsigned int wall1_ind = 0; wall1_ind < _walls.size() - 1; wall1_ind++) {
                //                wall1_it = std::next(_walls.begin(),wall1_ind);
                //                std::cout << "wall1 index: " << wall1_ind
                //                <<"\n\tcenter:\t\t"<<wall1_it->get()->getPosition()<<
                //                "\n\tviewsize:\t"<<wall1_it->get()->getAbsoluteViewSize() << "\n"; std::cout << "right
                //                border: " <<
                //                wall1_it->get()->getPosition().x+wall1_it->get()->getAbsoluteViewSize().x/2 << "\nleft
                //                border: " <<
                //                wall1_it->get()->getPosition().x-wall1_it->get()->getAbsoluteViewSize().x/2 << "\n";
                for (unsigned int wall2_ind = wall1_ind + 1; wall2_ind < _walls.size(); wall2_ind++) {
                        //                        wall2_it = std::next(_walls.begin(),wall2_ind);
                        //                        std::cout << "wall2 index: " << wall2_ind
                        //                        <<"\n\tcenter:\t\t"<<wall2_it->get()->getPosition()<<
                        //                        "\n\tviewsize:\t"<<wall2_it->get()->getAbsoluteViewSize() << "\n";
                        wall1_ptr = *std::next(_walls.begin(), wall1_ind);
                        wall2_ptr = *std::next(_walls.begin(), wall2_ind);

                        diff = {wall2_ptr->getPosition().x - wall1_ptr->getPosition().x,
                                wall1_ptr->getPosition().y - wall2_ptr->getPosition().y};

                        lower_middle_border =
                            std::min(wall1_ptr->getPosition().x + wall1_ptr->getAbsoluteViewSize().x / 2.0f,
                                     wall2_ptr->getPosition().x + wall2_ptr->getAbsoluteViewSize().x / 2.0f);
                        higher_middle_border =
                            std::max(wall1_ptr->getPosition().x - wall1_ptr->getAbsoluteViewSize().x / 2.0f,
                                     wall2_ptr->getPosition().x - wall2_ptr->getAbsoluteViewSize().x / 2.0f);
                        //                        if (wall2_ind == 8){
                        //                                std::cout << "";
                        //                        }
                        //                        std::cout << "right border: " <<
                        //                        wall2_ptr->getPosition().x+wall2_ptr->getAbsoluteViewSize().x/2 <<
                        //                        "\nleft border: " <<
                        //                        wall2_ptr->getPosition().x-wall2_ptr->getAbsoluteViewSize().x/2 <<
                        //                        "\n";
                        // check for row
                        if (std::abs(diff.y) < 0.0001f &&
                            std::abs(lower_middle_border - higher_middle_border) < 0.0001f) {
                                //                                std::cout << "\t\t\tmerged: " << wall1_ind << " and "
                                //                                <<wall2_ind<<"\n";

                                lowest_border =
                                    std::min(wall1_ptr->getPosition().x - wall1_ptr->getAbsoluteViewSize().x / 2.0f,
                                             wall2_ptr->getPosition().x - wall2_ptr->getAbsoluteViewSize().x / 2.0f);
                                highest_border =
                                    std::max(wall1_ptr->getPosition().x + wall1_ptr->getAbsoluteViewSize().x / 2.0f,
                                             wall2_ptr->getPosition().x + wall2_ptr->getAbsoluteViewSize().x / 2.0f);

                                wall1_ptr->setScale(
                                    {wall1_ptr->getScale().x + wall2_ptr->getScale().x, wall1_ptr->getScale().y});
                                wall1_ptr->setPosition(
                                    {(lowest_border + highest_border) / 2.0f, wall1_ptr->getPosition().y});
                                _walls.erase(std::find(_walls.begin(), _walls.end(), wall2_ptr));
                                melted_wall = true;
                                //                                std::cout << std::endl;
                        }
                        // check for column
                        //                        if (){
                        //                                melted_wall = true;
                        //                        }
                }
                //                std::cout << std::endl;
        }
        if (melted_wall)
                meltColumns();
}

void World::generateSquareWallEnclosure(const Vector2f& origin, float size, float wall_thickness)
{
        std::shared_ptr<Wall> left_wall = _entity_model_creator->createWallModel(
            _camera, {origin.x - (size / 2), origin.y}, {wall_thickness, size + wall_thickness});
        _walls.push_back(left_wall);

        std::shared_ptr<Wall> right_wall = _entity_model_creator->createWallModel(
            _camera, {origin.x + (size / 2), origin.y}, {wall_thickness, size + wall_thickness});
        _walls.push_back(right_wall);

        std::shared_ptr<Wall> down_wall = _entity_model_creator->createWallModel(
            _camera, {origin.x, origin.y - (size / 2)}, {size - wall_thickness, wall_thickness});
        _walls.push_back(down_wall);

        std::shared_ptr<Wall> up_wall = _entity_model_creator->createWallModel(
            _camera, {origin.x, origin.y + (size / 2)}, {size - wall_thickness, wall_thickness});
        _walls.push_back(up_wall);
}

void World::generateCars(const Vector2f& position, const Vector2f& direction, unsigned int amount,
                         const std::string& physics_preset, const std::string& sprite_preset)
{
        for (unsigned int i = 0; i < amount; i++) {
                if (physics_preset.empty() && sprite_preset.empty()) {
                        std::shared_ptr<Core::Car> new_car = _entity_model_creator->createCarModel(
                            _camera, position, {0.2, 0.2}, physics_preset, sprite_preset);
                        new_car->reset(position, direction);
                        new_car->setAIControlled(true);
                        new_car->setCheckpointCount(_checkpoints.size());
                        _cars.push_back(new_car);
                } else {
                        std::shared_ptr<Core::Car> new_car = _entity_model_creator->createCarModel(
                            _camera, position, {0.2, 0.2}, physics_preset, sprite_preset);
                        new_car->reset(position, direction);
                        new_car->setAIControlled(true);
                        new_car->setCheckpointCount(_checkpoints.size());
                        _cars.push_back(new_car);
                }
        }
}

void World::generateTestMap()
{
        // ground
        generateGroundTiles(2);

        // walls
        generateSquareWallEnclosure({0, 0}, 5, 0.5);
        generateSquareWallEnclosure({0, 0}, 2, 0.2);

        // checkpoints

        float raycast_length = 1.7;
        Vector2f view_size = {0, 0};

        bool show_raycast = false;

        // right wall

        // first checkpoint
        std::shared_ptr<Checkpoint> new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {2.5, 0.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {2.5, 1.25}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(20));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint = _entity_model_creator->createCheckpointModel(_camera, {2.5, 2.5}, view_size, {-1, 0},
                                                                      raycast_length * 1.25f);
        new_checkpoint->rotate(CoreUtils::toRadian(45));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        // top wall
        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {1.25, 2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(90 - 20));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {0.5, 2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(90));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {0, 2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(90));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {-0.5, 2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(90));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {-1.25, 2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(90 + 20));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        // left wall
        new_checkpoint = _entity_model_creator->createCheckpointModel(_camera, {-2.5, 2.5}, view_size, {-1, 0},
                                                                      raycast_length * 1.25f);
        new_checkpoint->rotate(CoreUtils::toRadian(180 - 45));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {-2.5, 1.25}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(180 - 20));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {-2.5, 0.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(180));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {-2.5, 0}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(180));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {-2.5, -0.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(180));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {-2.5, -1.25}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(180 + 20));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint = _entity_model_creator->createCheckpointModel(_camera, {-2.5, -2.5}, view_size, {-1, 0},
                                                                      raycast_length * 1.25f);
        new_checkpoint->rotate(CoreUtils::toRadian(180 + 45));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        // down wall
        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {-1.25, -2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(-(90 + 20)));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {-0.5, -2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(-90));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {0, -2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(-90));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {0.5, -2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(-90));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {1.25, -2.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(-(90 - 20)));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        // last raycast on the right side
        new_checkpoint = _entity_model_creator->createCheckpointModel(_camera, {2.5, -2.5}, view_size, {-1, 0},
                                                                      raycast_length * 1.25f);
        new_checkpoint->rotate(CoreUtils::toRadian(-45));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {2.5, -1.25}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(-20));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {2.5, -0.5}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {2.5, -1.25}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->rotate(CoreUtils::toRadian(-20));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        new_checkpoint = _entity_model_creator->createCheckpointModel(_camera, {2.5, -2.5}, view_size, {-1, 0},
                                                                      raycast_length * 1.25f);
        new_checkpoint->rotate(CoreUtils::toRadian(-45));
        new_checkpoint->setShowRaycast(show_raycast);
        _checkpoints.push_back(new_checkpoint);

        // finish_line
        new_checkpoint =
            _entity_model_creator->createCheckpointModel(_camera, {2.5, 0}, view_size, {-1, 0}, raycast_length);
        new_checkpoint->setShowRaycast(show_raycast);
        _finish_line = new_checkpoint;
}

void World::updateAI(double t, float dt)
{
        // AI
        _generation_time += dt;
        bool all_cars_dead_or_finished = true;
        for (auto& car : _cars) {
                if (!(car->isDead() || car->reachedFinish())) {
                        all_cars_dead_or_finished = false;
                }
        }

        // time limit
        if (_generation_time > _time_limit) {
                for (auto& car : _cars) {
                        car->calculateFitness(true);
                }
                all_cars_dead_or_finished = true;
                std::cout << "Time Out!" << std::endl;
        }

        if (all_cars_dead_or_finished) {

                _generation++;
                std::cout << "Generation " << _generation << std::endl;
                std::shared_ptr<Car> aux = _cars[_populationCars - 1];
                _cars.pop_back();
                sort(_cars.begin(), _cars.end(), [](const std::shared_ptr<Car>& lhs, const std::shared_ptr<Car>& rhs) {
                        return lhs->getFitness() < rhs->getFitness();
                });

                for (int i = 2; i < _populationCars - 1; i += 2) {
                        _cars[i]->getBrain().uniformCrossOverWeights(_cars[0]->getBrain(), _cars[1]->getBrain(),
                                                                     _cars[i + 1]->getBrain());
                        _cars[i]->getBrain().uniformCrossOverBiases(_cars[0]->getBrain(), _cars[1]->getBrain(),
                                                                    _cars[i + 1]->getBrain());
                }

                for (int i = 2; i < _populationCars - 1; i++) {
                        for (int j = 0; j < _mutationRate; j++) {
                                _cars[i]->getBrain().mutateOneWeightGene(aux->getBrain());
                                aux->getBrain().mutateOneWeightGene(_cars[i]->getBrain());
                                _cars[i]->getBrain().mutateOneBiasesGene(aux->getBrain());
                                aux->getBrain().mutateOneBiasesGene(_cars[i]->getBrain());
                        }
                }

                _cars.push_back(aux);

                // for (int i = nBest; i < _populationCars-2; i++) {
                //         Car* carParrent1 = bestCars[floor(Random::uniformReal(0,1)*((float) bestCars.size()))];
                //         Car* carParrent2 = bestCars[floor(Random::uniformReal(0,1)*((float) bestCars.size()))];

                //         if (Random::uniformReal(0,1) > 0.5f) {
                //                 Car* temp = carParrent1;
                //                 carParrent1 = carParrent2;
                //                 carParrent2 = temp;
                //         }

                //         _cars[i]->getBrain() = carParrent1->getBrain().crossover(carParrent2->getBrain()); //check
                //         crossover _cars[i]->getBrain().mutate(mr);

                // }
                // for (int i = 0; i < 2; i++) {
                //         Car* carP = bestCars[floor(Random::uniformReal(0,1)*((float) bestCars.size()))];
                //         _cars[nPopulation-i-1]->getBrain() = carP->getBrain();
                //         _cars[nPopulation-i-1]->getBrain().mutate(mr);
                // }

                // reset
                for (auto& car : _cars) {
                        car->reset(_spawn_location, _spawn_direction);
                }
                _generation_time = 0;

                // neural network mutation
        }
}

void World::updateCamera(double t, float dt)
{
        // camera zoom
        if (_user_input_map->mouse_wheel_delta != 0) {
                float zoom = -2.f;
                Vector2f new_x_bounderies = _camera->getXBounderies();
                new_x_bounderies.x -= zoom * _camera->getCamerawidth() * dt * _user_input_map->mouse_wheel_delta;
                new_x_bounderies.y += zoom * _camera->getCamerawidth() * dt * _user_input_map->mouse_wheel_delta;

                Vector2f new_y_bounderies = _camera->getYBounderies();
                new_y_bounderies.x -= zoom * _camera->getCameraheight() * dt * _user_input_map->mouse_wheel_delta;
                new_y_bounderies.y += zoom * _camera->getCameraheight() * dt * _user_input_map->mouse_wheel_delta;

                _camera->setCameraBounderies(new_x_bounderies.x, new_x_bounderies.y, new_y_bounderies.x,
                                             new_y_bounderies.y);
                _user_input_map->mouse_wheel_delta = 0;
        }

        if (_camera->getFocused())
                return;

        // camera move
        if (_user_input_map->mouse_button_middle) {
                if (!move_camera) {
                        move_camera = true;
                        _middle_mouse_lock_pos = _user_input_map->mouse_pos_representation_window;
                        _previous_camera_pos = _camera->getPosition();

                        return;
                }
                float camera_move_scale = 2;

                Vector2f move_vector =
                    _camera->projectCoordinateRepresentationToWorld(_user_input_map->mouse_pos_representation_window) -
                    _camera->projectCoordinateRepresentationToWorld(_middle_mouse_lock_pos);

                move_vector *= -2;

                _camera->setPosition(_previous_camera_pos + move_vector);

        } else {
                move_camera = false;
                _middle_mouse_lock_pos.clear();
        }
}

void World::controlPlayer(double t, float dt)
{
        if (CoreConstants::generate_player) {
                // player movement
                _player->getInputMap()->up = _user_input_map->w;
                _player->getInputMap()->down = _user_input_map->s;
                _player->getInputMap()->left = _user_input_map->a;
                _player->getInputMap()->right = _user_input_map->d;
                _player->getInputMap()->reset = _user_input_map->r;

                // camera focus
                if (_user_input_map->f) {
                        if (!_previous_focus_state) {
                                _player->setCameraFocus(!_player->getCameraFocus());
                                _previous_focus_state = true;
                        }
                } else {
                        _previous_focus_state = false;
                }
        }
}

void World::controlGameSpeed(double t, float dt)
{
        // game speed
        if (_user_input_map->z) {
                Stopwatch::getInstance().setPhysicsSpeed(1);
        }
        if (_user_input_map->x) {
                Stopwatch::getInstance().setPhysicsSpeed(2);
        }
        if (_user_input_map->c) {
                Stopwatch::getInstance().setPhysicsSpeed(4);
        }
        if (_user_input_map->v) {
                Stopwatch::getInstance().setPhysicsSpeed(10);
        }
}

void World::updateEntities(double t, float dt)
{
        // player
        if (CoreConstants::generate_player) {
                _player->update(t, dt);
        }

        // cars
        for (auto& car : _cars) {
                car->update(t, dt);
        }

        // walls
        for (auto& wall : _walls) {
                wall->update(t, dt);
        }

        // ground_tiles
        for (auto& ground_tile : _ground_tiles) {
                ground_tile->update(t, dt);
        }
}

void World::checkCollisions(double t, float dt)
{
        // walls
        for (auto& wall : _walls) {
                // player
                if (CoreConstants::generate_player) {
                        checkCollision(_player, wall);

                        for (auto& raycast : _player->getRaycasts()) {
                                checkCollision(raycast, wall);
                        }
                }

                // cars
                for (auto& car : _cars) {
                        if (checkCollision(car, wall)) {
                                car->onHit();
                        }

                        for (auto& raycast : car->getRaycasts()) {
                                checkCollision(raycast, wall);
                        }

                        // checkpoints
                        for (auto& checkpoint : _checkpoints) {
                                std::shared_ptr<Raycast> raycast = checkpoint->getRaycast(0);
                                if (checkCollision(raycast, car)) {
                                        car->onHitCheckpoint(checkpoint->getID());
                                        raycast->clear();
                                }
                        }

                        // finish line
                        if (_finish_line) {
                                std::shared_ptr<Raycast> raycast = _finish_line->getRaycast(0);
                                if (checkCollision(raycast, car)) {
                                        car->checkReachedFinish(_checkpoints.size());
                                        raycast->clear();
                                }
                        }
                }
        }
}

bool World::checkCollision(const std::shared_ptr<EntityModel>& entity1, const std::shared_ptr<EntityModel>& entity2,
                           bool collision_response)
{
        // todo
        // collision_response velocity

        // minimum translation vector
        Vector2f mtv;

        bool colliding;

        if (entity1->getHitbox()->isCircle() && entity2->getHitbox()->isCircle()) {
                colliding = checkCollisionCircleAndCircle(entity1, entity2, collision_response, mtv);
        } else if (entity1->getHitbox()->isCircle() || entity2->getHitbox()->isCircle()) {
                colliding = checkCollisionPolygonAndCircle(entity1, entity2, collision_response, mtv);
        } else {
                colliding = checkCollisionPolygonAndPolygon(entity1, entity2, collision_response, mtv);
        }

        if (!colliding)
                return false;

        if (!collision_response)
                return true;

        float mtd1_amount{0};
        float mtd2_amount{0};
        if (entity1->getStatic() && entity2->getStatic()) {
                return true;
        } else if (entity1->getStatic()) {
                mtd1_amount = 0;
                mtd2_amount = 1;
        } else if (entity2->getStatic()) {
                mtd1_amount = 1;
                mtd2_amount = 0;
        } else {
                mtd1_amount = 0.5;
                mtd2_amount = 0.5;
        }

        Vector2f mtv1 = mtv * mtd1_amount;
        Vector2f mtv2 = mtv * mtd2_amount * -1;

        if (!mtv1.empty()) {
                entity1->move(mtv1);
                entity1->notifyObservers();
        }
        if (!mtv2.empty()) {
                entity2->move(mtv2);
                entity2->notifyObservers();
        }
        return true;
}

bool World::checkCollision(const std::shared_ptr<Core::Raycast>& raycast, const std::shared_ptr<EntityModel>& entity)
{
        std::vector<Vector2f> points = entity->getHitbox()->getPoints();

        Vector2f collision_point1;
        Vector2f collision_point2;
        bool collided_once = false;
        bool collided_twice = false;

        bool intersected{false};
        Vector2f intersection1;
        Vector2f intersection2;

        // circle intersection
        if (entity->getHitbox()->isCircle()) {
                intersected = checkLinesegmentCircleIntersection(
                    raycast->getOrigin(), raycast->getEndpoint(), entity->getHitbox()->getOrigin(),
                    entity->getHitbox()->getRadius().x, intersection1, intersection2, collided_twice);

                if (intersected && !collided_twice)
                        collided_once = true;
        }

        // polygon intersection
        else {
                for (int i = 0; i < points.size(); i++) {
                        Vector2f point1 = points[i];
                        Vector2f point2;

                        if (i != points.size() - 1)
                                point2 = points[i + 1];
                        else
                                point2 = points[0];

                        bool is_collinear;

                        intersected = false;
                        intersection1.clear();
                        intersection2.clear();

                        intersected = checkLinesegmentLinesegmentIntersection(
                            raycast->getOrigin(), raycast->getEndpoint(), point1, point2, intersection1, intersection2,
                            is_collinear);

                        if (!intersected)
                                continue;

                        if (is_collinear) {
                                collision_point1 = intersection1;
                                collision_point2 = intersection2;
                                collided_twice = true;
                                break;
                        } else if (collided_once) {
                                collision_point2 = intersection1;
                                collided_once = false;
                                collided_twice = true;
                                break;
                        } else {
                                collision_point1 = intersection1;
                                collided_once = true;
                        }
                }
        }

        if (collided_once) {
                if (!raycast->isActivated() ||
                    (collision_point1 - raycast->getOrigin()).length() < raycast->getCollisionLength())
                        raycast->setCollisionPoint(collision_point1);
        } else if (collided_twice) {
                // get the closest point
                if ((collision_point1 - raycast->getOrigin()).length() <
                    (collision_point2 - raycast->getOrigin()).length()) {
                        if (!raycast->isActivated() ||
                            (collision_point1 - raycast->getOrigin()).length() < raycast->getCollisionLength())
                                raycast->setCollisionPoint(collision_point1);
                } else {
                        if (!raycast->isActivated() ||
                            (collision_point2 - raycast->getOrigin()).length() < raycast->getCollisionLength())
                                raycast->setCollisionPoint(collision_point2);
                }
        } else {
                return false;
        }

        raycast->setActivated(true);

        return true;
}

bool World::checkCollisionPolygonAndPolygon(const std::shared_ptr<EntityModel>& entity1,
                                            const std::shared_ptr<EntityModel>& entity2, bool collision_response,
                                            Vector2f& mtv)
{
        std::vector<Vector2f> h1_points = entity1->getHitbox()->getPoints();
        std::vector<Vector2f> h2_points = entity2->getHitbox()->getPoints();

        float min_translation_distance = std::numeric_limits<float>::infinity();

        std::vector<std::vector<Vector2f>> entity_points = {h1_points, h2_points};

        // loops through the sides of the polygons
        for (const auto& points : entity_points) {
                for (int i = 0; i < points.size(); i++) {
                        Vector2f point1 = points[i];
                        Vector2f point2;

                        if (i != points.size() - 1)
                                point2 = points[i + 1];
                        else
                                point2 = points[0];

                        // calculates the normal vector of the side
                        Vector2f axis = {-(point2.y - point1.y), point2.x - point1.x};
                        axis.normalize();

                        // projects the points of h1 on the axis
                        float h1min = axis.dotProduct(h1_points[0]);
                        float h1max = h1min;

                        for (const auto& point : h1_points) {
                                float dot = axis.dotProduct(point);
                                h1min = std::min(h1min, dot);
                                h1max = std::max(h1max, dot);
                        }

                        // projects the points of h2 on the axis
                        float h2min = axis.dotProduct(h2_points[0]);
                        float h2max = h2min;

                        for (const auto& point : h2_points) {
                                float dot = axis.dotProduct(point);
                                h2min = std::min(h2min, dot);
                                h2max = std::max(h2max, dot);
                        }

                        // found a gap
                        if ((h1min - h2max > 0) || (h2min - h1max > 0)) {
                                return false;
                        }

                        if (collision_response) {
                                // vector magic
                                float translation_distance;
                                if (h1max > h1min)
                                        translation_distance = h2max - h1min;
                                else
                                        translation_distance = h1max - h2min;

                                if (translation_distance < min_translation_distance) {
                                        min_translation_distance = translation_distance;
                                        mtv = axis * translation_distance;
                                }
                        }
                }
        }

        return true;
}

bool World::checkCollisionPolygonAndCircle(const std::shared_ptr<EntityModel>& entity1,
                                           const std::shared_ptr<EntityModel>& entity2, bool collision_response,
                                           Vector2f& mtv)
{
        std::shared_ptr<Core::Hitbox> circle;
        std::shared_ptr<Core::Hitbox> polygon;

        float min_translation_distance = std::numeric_limits<float>::infinity();

        float mtv_sign{1};

        if (entity1->getHitbox()->isCircle()) {
                circle = entity1->getHitbox();
                polygon = entity2->getHitbox();
                mtv_sign = -1;
        } else {
                circle = entity2->getHitbox();
                polygon = entity1->getHitbox();
        }

        float closest_squared_distance = std::numeric_limits<float>::infinity();
        Vector2f closest_point;

        std::vector<Vector2f> points = polygon->getPoints();
        for (int i = 0; i < points.size(); i++) {
                // searches closest point to circle
                float squared_distance = static_cast<float>(std::pow(points[i].x - circle->getOrigin().x, 2) +
                                                            std::pow(points[i].y - circle->getOrigin().y, 2));

                if (squared_distance < closest_squared_distance) {
                        closest_squared_distance = squared_distance;
                        closest_point = points[i];
                }

                Vector2f point1 = points[i];
                Vector2f point2;

                if (i != points.size() - 1)
                        point2 = points[i + 1];
                else
                        point2 = points[0];

                // calculates the normal vector of the side
                Vector2f axis = {-(point2.y - point1.y), point2.x - point1.x};
                axis.normalize();

                // projects the points of the polygon on the axis
                float h1min = axis.dotProduct(points[0]);
                float h1max = h1min;

                for (const auto& point : points) {
                        float dot = axis.dotProduct(point);
                        h1min = std::min(h1min, dot);
                        h1max = std::max(h1max, dot);
                }

                // projects the points of the circle on the axis
                float projected_origin = axis.dotProduct(circle->getOrigin());

                float h2min = projected_origin - circle->getRadius().x;
                float h2max = projected_origin + circle->getRadius().x;

                // found a gap
                if ((h1min - h2max > 0) || (h2min - h1max > 0)) {
                        return false;
                }

                if (collision_response) {
                        // vector magic
                        float translation_distance;
                        if (h1max > h1min)
                                translation_distance = h2max - h1min;
                        else
                                translation_distance = h1max - h2min;

                        if (translation_distance < min_translation_distance) {
                                min_translation_distance = translation_distance;
                                mtv = axis * translation_distance * mtv_sign;
                        }
                }
        }

        // circle origin & closest vertex of polygon axis
        Vector2f axis = closest_point - circle->getOrigin();
        axis.normalize();

        // projects the points of the polygon on the axis
        float h1min = axis.dotProduct(points[0]);
        float h1max = h1min;

        for (const auto& point : points) {
                float dot = axis.dotProduct(point);
                h1min = std::min(h1min, dot);
                h1max = std::max(h1max, dot);
        }

        // projects the points of the circle on the axis
        float projected_origin = axis.dotProduct(circle->getOrigin());

        float h2min = projected_origin - circle->getRadius().x;
        float h2max = projected_origin + circle->getRadius().x;

        // found a gap
        if ((h1min - h2max > 0) || (h2min - h1max > 0)) {
                return false;
        }

        if (collision_response) {
                // vector magic
                float translation_distance;
                if (h1max > h1min)
                        translation_distance = h2max - h1min;
                else
                        translation_distance = h1max - h2min;

                if (translation_distance < min_translation_distance) {
                        mtv = axis * translation_distance * mtv_sign;
                }
        }

        return true;
}

bool World::checkCollisionCircleAndCircle(const std::shared_ptr<EntityModel>& entity1,
                                          const std::shared_ptr<EntityModel>& entity2, bool collision_response,
                                          Vector2f& mtv)
{
        Vector2f o1 = entity1->getHitbox()->getOrigin();
        Vector2f o2 = entity2->getHitbox()->getOrigin();

        float squared_distance = static_cast<float>(std::pow(o1.x - o2.x, 2) + std::pow(o1.y - o2.y, 2));

        // checks if circles overlap
        if (!((std::pow(entity1->getHitbox()->getRadius().x - entity2->getHitbox()->getRadius().x, 2) <=
               squared_distance) &&
              squared_distance <=
                  std::pow(entity1->getHitbox()->getRadius().x + entity2->getHitbox()->getRadius().x, 2))) {
                return false;
        }

        // minimum translation vector
        if (collision_response) {
                float mtd = entity1->getHitbox()->getRadius().x + entity2->getHitbox()->getRadius().x -
                            std::sqrt(squared_distance);
                Vector2f axis = (o1 - o2);
                axis.normalize();
                mtv = axis * mtd;
        }

        return true;
}

bool World::checkLinesegmentLinesegmentIntersection(const Vector2f& l1p1, const Vector2f& l1p2, const Vector2f& l2p1,
                                                    const Vector2f& l2p2, Vector2f& intersection1,
                                                    Vector2f& intersection2, bool& is_collinear)
{
        const Vector2f& p = l1p1;
        const Vector2f& q = l2p1;

        Vector2f r = l1p2 - l1p1;
        Vector2f s = l2p2 - l2p1;

        Vector2f q_p = q - p;

        float rxs = r.crossProduct(s);

        float q_pxs = q_p.crossProduct(s);
        float q_pxr = q_p.crossProduct(r);

        // lines are collinear
        if (rxs == 0 && q_pxr == 0) {
                float rdotr = r.dotProduct(r);
                float sdotr = s.dotProduct(r);

                float t0{0};
                float t1{0};

                // interval [t0, t1]
                if (sdotr >= 0) {
                        t0 = q_p.dotProduct(r) / rdotr;
                        t1 = t0 + (s.dotProduct(r) / rdotr);
                }
                // interval [t1, t0]
                else {
                        t1 = (q_p.dotProduct(r) / rdotr);
                        t0 = t1 + (s.dotProduct(r) / rdotr);
                }

                // if the interval intersects [0, 1]
                if ((t0 >= 0 && t0 <= 1) || (t1 >= 0 && t1 <= 1)) {
                        if (t0 >= 0)
                                intersection1 = p + t0 * r;
                        else
                                intersection1 = p;

                        if (t1 <= 1)
                                intersection2 = p + t1 * r;
                        else
                                intersection2 = p + r;

                        is_collinear = true;
                        return true;
                }
        }
        // lines are parallel
        else if (rxs == 0) {
                return false;
        }
        // lines intersect
        else {
                float t = q_pxs / rxs;
                float u = q_pxr / rxs;

                if (0 <= t && t <= 1 && 0 <= u && u <= 1) {
                        intersection1 = p + t * r;
                        is_collinear = false;
                        return true;
                }
        }

        // lines are not parallel but do not intersect
        return false;
}

bool World::checkLinesegmentCircleIntersection(const Vector2f& l1p1, const Vector2f& l1p2, const Vector2f& cmp,
                                               float cr, Vector2f& intersection1, Vector2f& intersection2,
                                               bool& collided_twice)
{
        Vector2f d = l1p2 - l1p1;
        Vector2f f = l1p1 - cmp;

        float a = d.dotProduct(d);
        float b = 2 * f.dotProduct(d);
        float c = f.dotProduct(f) - cr * cr;

        float discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
                return false;

        discriminant = sqrt(discriminant);

        float t1 = (-b - discriminant) / (2 * a);
        float t2 = (-b + discriminant) / (2 * a);

        bool collided_once{false};

        if (t1 >= 0 && t1 <= 1) {
                intersection1 = l1p1 + d * t1;
                collided_once = true;
        }

        if (t2 >= 0 && t2 <= 1) {
                if (!collided_once) {
                        intersection1 = l1p1 + d * t2;
                        collided_once = true;
                } else {
                        intersection2 = l1p1 + d * t2;
                        collided_twice = true;
                }
        }

        return collided_once;
}
} // namespace Core
