#ifndef UABA2_AP_PROJECT_IENTITYMODELCREATOR_H
#define UABA2_AP_PROJECT_IENTITYMODELCREATOR_H

#include "Car.h"
#include "Doodle.h"
#include "Wall.h"
#include "GroundTile.h"
#include <memory>

namespace Core {
class IEntityModelCreator
{
public:
        virtual std::shared_ptr<Core::Doodle> createDoodleModel(std::shared_ptr<Core::Camera> camera,
                                                                const Core::Vector2f& position,
                                                                const Core::Vector2f& view_size) = 0;

        virtual std::shared_ptr<Core::Car> createCarModel(std::shared_ptr<Core::Camera> camera,
                                                          const Core::Vector2f& position,
                                                          const Core::Vector2f& view_size) = 0;

        virtual std::shared_ptr<Core::Wall> createWallModel(std::shared_ptr<Core::Camera> camera,
                                                                const Core::Vector2f& position,
                                                                const Core::Vector2f& view_size) = 0;

        virtual std::shared_ptr<Core::GroundTile> createGroundTileModel(std::shared_ptr<Core::Camera> camera,
                                                            const Core::Vector2f& position,
                                                            const Core::Vector2f& view_size) = 0;
};
} // namespace Core

#endif // UABA2_AP_PROJECT_IENTITYMODELCREATOR_H
