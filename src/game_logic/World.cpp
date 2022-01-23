#include "World.h"

namespace Core {
World::World(std::shared_ptr<IEntityModelCreator> entity_model_creator, float x_min, float x_max, float y_min,
             float y_max)
    : _entity_model_creator(std::move(entity_model_creator)), _camera(new Camera), _user_input_map(new InputMap)
{
        _camera->setRepresentationBounderies(x_min, x_max, y_min, y_max);

        _player = _entity_model_creator->createCarModel(_camera, {0, 1}, {0.3, 0.3});
        _player->setInputMap(_user_input_map);

        std::shared_ptr<Wall> wall1 = _entity_model_creator->createWallModel(_camera, {1, 0.5}, {0.8, 0.8});
        _walls.insert(wall1);

        Vector2f new_pos = wall1->getPosition();
        Vector2f new_direction = {0, -1};
        std::shared_ptr<Core::Raycast> raycast = std::make_shared<Core::Raycast>(new_pos, new_direction, 0.4);
        wall1->addRaycast(raycast);

        //        std::shared_ptr<Car> car1 = _entity_model_creator->createCarModel(_camera, {-1, 0.5}, {0.3, 0.3});
        //        _cars.insert(car1);
        initializeWalls("./Inputfiles/Untitled2.png");
        std::cout << _walls.size();
}

World::~World() = default;

void World::update(double t, float dt)
{
        // logic
        if (_user_input_map->custom4) {
                _walls.begin()->get()->rotate(45 * Stopwatch::getInstance().getPhysicsDeltaTime());
                //                float new_scale = (std::cos(t) / 2) + 1.f;
                //                _walls.begin()->get()->setScale({new_scale, new_scale});
        }

        if (_user_input_map->custom5) {
                _walls.begin()->get()->scale({1, 0.999});
        }

        int i = 0;
        for (auto& raycast : _player->getRaycasts()) {
                // distance
                //                std::cout << i << " -> " << raycast->getCollisionLength() << std::endl;
                i++;
        }

        // game speed
        if (_user_input_map->custom1) {
                Stopwatch::getInstance().setPhysicsSpeed(10);
        }
        if (_user_input_map->custom2) {
                Stopwatch::getInstance().setPhysicsSpeed(0.1);
        }
        if (_user_input_map->custom3) {
                Stopwatch::getInstance().setPhysicsSpeed(1);
        }

        for (auto& raycast : _player->getRaycasts()) {
                float lengthToObstacle =
                    (raycast->isActivated() ? (raycast->getOrigin() - raycast->getCollisionPoint()).length() : 1.3f);
        }
        // updates
        updateEntities(t, dt);

        // collisions
        checkCollisions();
}

std::shared_ptr<InputMap> World::getInputMap() { return _user_input_map; }

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

        for (int i = 0; i < points.size(); i++) {
                Vector2f point1 = points[i];
                Vector2f point2;

                if (i != points.size() - 1)
                        point2 = points[i + 1];
                else
                        point2 = points[0];

                bool is_collinear;

                Vector2f intersection1;
                Vector2f intersection2;

                bool intersected = checkLineIntersection(raycast->getOrigin(), raycast->getEndpoint(), point1, point2,
                                                         intersection1, intersection2, is_collinear);

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
        if (collided_once) {
                raycast->setCollisionPoint(collision_point1);
        } else if (collided_twice) {
                // get the closest point
                if ((collision_point1 - raycast->getOrigin()).length() <
                    (collision_point2 - raycast->getOrigin()).length()) {
                        raycast->setCollisionPoint(collision_point1);
                } else {
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

bool World::checkLineIntersection(const Vector2f& l1p1, const Vector2f& l1p2, const Vector2f& l2p1,
                                  const Vector2f& l2p2, Vector2f& intersection1, Vector2f& intersection2,
                                  bool& is_collinear)
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
void World::initializeWalls(const std::string& inputname)
{
        imageProcessor imageProcessor{inputname};
        unsigned int brick_side = std::max(imageProcessor.getRows() / 25, 1u);
        unsigned int wallpixels{};
        Core::Vector2f wall_pos{};
        Core::Vector2f wall_size{};

        unsigned threshold = (unsigned int)((float)brick_side * (float)brick_side) / 2;
        for (unsigned int brickrow = 0; brickrow < imageProcessor.size() - brick_side; brickrow += brick_side) {
                for (unsigned int brick_col = 0; brick_col < imageProcessor.at(brickrow).size() - brick_side;
                     brick_col += brick_side) {
                        wallpixels = 0;
                        for (unsigned int row = 0; row < brick_side; row++) {
                                for (unsigned int col = 0; col < brick_side; col++) {
                                        if (col + brick_col >= 800) {
                                                std::cout << "aaa";
                                        }
                                        if (imageProcessor.isWall(row + brickrow, col + brick_col)) {
                                                wallpixels++;
                                        }
                                }
                        }
                        if (wallpixels >= threshold) {
                                wall_size = Core::Vector2f((float)brick_side, (float)brick_side);
                                wall_pos = Core::Vector2f((float)brickrow + (float)brick_side / 2,
                                                          (float)brick_col + (float)brick_side / 2);
                                _walls.insert(_entity_model_creator->createWallModel(
                                    _camera,
                                    _camera->projectCoordinate(wall_pos, 0.0f, (float)imageProcessor.getColumns(), 0.0f,
                                                               (float)imageProcessor.getRows()),
                                    _camera->projectSize(wall_size, 0.0f, (float)imageProcessor.getColumns(), 0.0f,
                                                         (float)imageProcessor.getRows())));
                        }
                }
        }
}
} // namespace Core
