#include "GroundTile.h"

Core::GroundTile::GroundTile(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position,
                             const Core::Vector2f& view_size)
    : EntityModel(std::move(camera), position, view_size)
{
}
void Core::GroundTile::update(double t, float dt) {
        notifyObservers();
}
