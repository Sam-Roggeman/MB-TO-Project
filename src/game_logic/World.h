#ifndef PHYSICS_ENGINE_WORLD_H
#define PHYSICS_ENGINE_WORLD_H

//#include "../imageclasses/imageProcessor.h"
#include "data_structures/Raycast.h"
#include "entities/IEntityModelCreator.h"
#include "entities/Wall.h"
#include "utils/InputMap.h"
#include "utils/Stopwatch.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <utility>

namespace Core {
class World
{
private:
        std::shared_ptr<Camera> _camera;

        // entities
        std::shared_ptr<IEntityModelCreator> _entity_model_creator;
        std::shared_ptr<Car> _player;
        std::vector<std::shared_ptr<Car>> _cars;
        std::vector<std::shared_ptr<Wall>> _walls;
        std::vector<std::shared_ptr<GroundTile>> _ground_tiles;

        std::shared_ptr<InputMap> _user_input_map;

public:
        void saveWorld(std::string filename)
        {
                ofstream myfile{"assets/" + filename};

                for (std::shared_ptr<Core::Wall> wall : _walls) {
                        myfile << wall->getAbsoluteViewSize() << ';' << wall->getPosition() << std::endl;
                }

                myfile.close();
        }
        void loadWorld(std::string filename)
        {
                string line{}, pos_string{};
                ifstream myfile("assets/" + filename);
                Vector2f viewsize{}, position{};
                if (myfile.is_open()) {
                        while (getline(myfile, line)) {
                                viewsize = {std::stof(line.substr(1, line.find(','))),
                                            std::stof(line.substr(line.find(',') + 1, line.find(')')))};
                                pos_string = line.substr(line.find(';') + 1, line.npos);
                                position = {
                                    std::stof(pos_string.substr(pos_string.find('(') + 1, pos_string.find(','))),
                                    std::stof(pos_string.substr(pos_string.find(',') + 1, pos_string.find(')')))};
                                _walls.emplace_back(
                                    _entity_model_creator->createWallModel(_camera, position, viewsize));
                        }
                        myfile.close();
                }

                else
                        cout << "Unable to open file";
        }
        World(std::shared_ptr<IEntityModelCreator> entity_model_creator, float x_min, float x_max, float y_min,
              float y_max);

        ~World();

        void update(double t, float dt);

        std::shared_ptr<InputMap> getInputMap();

private:
        void generateGroundTiles(float scale = 1);

        void updateEntities(double t, float dt);

        void checkCollisions();

        // void initializeWalls(const std::string& inputname);

        static bool checkCollision(const std::shared_ptr<EntityModel>& entity1,
                                   const std::shared_ptr<EntityModel>& entity2, bool collision_response = true);

        /**
         * Only works for raycast & polygon
         */
        static bool checkCollision(const std::shared_ptr<Core::Raycast>& raycast,
                                   const std::shared_ptr<EntityModel>& entity);

        static bool checkCollisionPolygonAndPolygon(const std::shared_ptr<EntityModel>& entity1,
                                                    const std::shared_ptr<EntityModel>& entity2,
                                                    bool collision_response, Vector2f& mtv);
        static bool checkCollisionPolygonAndCircle(const std::shared_ptr<EntityModel>& entity1,
                                                   const std::shared_ptr<EntityModel>& entity2, bool collision_response,
                                                   Vector2f& mtv);
        static bool checkCollisionCircleAndCircle(const std::shared_ptr<EntityModel>& entity1,
                                                  const std::shared_ptr<EntityModel>& entity2, bool collision_response,
                                                  Vector2f& mtv);

        static bool checkLinesegmentLinesegmentIntersection(const Vector2f& l1p1, const Vector2f& l1p2,
                                                            const Vector2f& l2p1, const Vector2f& l2p2,
                                                            Vector2f& intersection1, Vector2f& intersection2,
                                                            bool& is_collinear);

        static bool checkLinesegmentCircleIntersection(const Vector2f& l1p1, const Vector2f& l1p2, const Vector2f& cmp,
                                                       float cr, Vector2f& intersection1, Vector2f& intersection2,
                                                       bool& collided_twice);
        void meltWalls();
};
} // namespace Core

#endif // PHYSICS_ENGINE_WORLD_H
