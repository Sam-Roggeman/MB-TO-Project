#include "World.h"

namespace Core {
World::World(std::shared_ptr<IEntityModelCreator> entity_model_creator, float x_min, float x_max, float y_min,
             float y_max)
    : _entity_model_creator(std::move(entity_model_creator)), _camera(new Camera), _user_input_map(new InputMap)
{
        _camera->setRepresentationBounderies(x_min, x_max, y_min, y_max);

        _player = _entity_model_creator->createCarModel(_camera, {0, 0}, {0.2, 0.2},
                                                        "assets/car_presets/physics_preset_1.xml",
                                                        "assets/car_presets/sprite_preset_1.xml");
        _player->setInputMap(_user_input_map);

        generateGroundTiles(2);

//        initializeWalls("assets/maps/Untitled3.png", 5);
        //        loadMap("assets/maps/world.save");
}

World::~World() { saveMap("assets/maps/world_last_run.save"); }

void World::update(double t, float dt)
{
        // logic

        // updates
        updateEntities(t, dt);

        // collisions
        checkCollisions();
}

std::shared_ptr<InputMap> World::getInputMap() { return _user_input_map; }

void World::saveMap(const std::string& filepath)
{
        ofstream myfile{filepath};

        for (const std::shared_ptr<Core::Wall>& wall : _walls) {
                myfile << wall->getAbsoluteViewSize() << ';' << wall->getPosition() << std::endl;
        }

        myfile.close();
}

void World::loadMap(const std::string& filepath)
{
        string line{}, pos_string{};
        ifstream myfile(filepath);
        Vector2f viewsize{}, position{};
        if (myfile.is_open()) {
                while (getline(myfile, line)) {
                        viewsize = {std::stof(line.substr(1, line.find(','))),
                                    std::stof(line.substr(line.find(',') + 1, line.find(')')))};
                        pos_string = line.substr(line.find(';') + 1, std::string::npos);
                        position = {std::stof(pos_string.substr(pos_string.find('(') + 1, pos_string.find(','))),
                                    std::stof(pos_string.substr(pos_string.find(',') + 1, pos_string.find(')')))};
                        _walls.emplace_back(_entity_model_creator->createWallModel(_camera, position, viewsize));
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
void World::initializeWalls(const std::string& inputname, float scale)
{
        imageProcessor imageProcessor{inputname};
        unsigned int square_r = std::max(imageProcessor.getRows() / 100, 1u);
        unsigned int wallpixels{};
        Core::Vector2f wall_pos{};
        Core::Vector2f wall_size{};

        unsigned threshold = (unsigned int)((float)square_r * (float)square_r) / 2;
        for (unsigned int base_row = 0; base_row < imageProcessor.getRows() - square_r; base_row += square_r) {
                for (unsigned int base_col = 0; base_col < imageProcessor.at(base_row).size() - square_r;
                     base_col += square_r) {
                        wallpixels = 0;
                        for (unsigned int row_offset = 0; row_offset < square_r; row_offset++) {
                                for (unsigned int col_offset = 0; col_offset < square_r; col_offset++) {
                                        if (imageProcessor.isWall(row_offset + base_row, col_offset + base_col)) {
                                                wallpixels++;
                                        }
                                }
                        }
                        if (wallpixels >= threshold) {
                                wall_size = Core::Vector2f((float)square_r, (float)square_r);
                                wall_pos = Core::Vector2f((float)base_col + (float)square_r / 2,
                                                          ((float)base_row + (float)square_r / 2));
                                _walls.emplace_back(_entity_model_creator->createWallModel(
                                    _camera,
                                    scale * _camera->projectCoordinate(wall_pos, 0.0f,
                                                                       (float)imageProcessor.getColumns(),
                                                                       (float)imageProcessor.getRows(), 0.0f),
                                    scale * _camera->projectSize(wall_size, 0.0f, (float)imageProcessor.getColumns(),
                                                                 (float)imageProcessor.getRows(), 0.0f)));
                        }
                }
        }

        std::cout << _walls.size() << '\n';
        meltWalls();
        std::cout << _walls.size() << '\n';
}
#else
void World::initializeWalls(const std::string& inputname, float scale)
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
void World::updateEntities(double t, float dt)
{
        // player
        _player->update(t, dt);

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

void World::checkCollisions()
{
        for (auto& wall : _walls) {
                // player
                bool collided = checkCollision(_player, wall);

                for (auto& raycast : _player->getRaycasts()) {
                        checkCollision(raycast, wall);
                }

                // cars
                for (auto& car : _cars) {
                        checkCollision(car, wall);

                        for (auto& raycast : car->getRaycasts()) {
                                checkCollision(raycast, wall);
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
