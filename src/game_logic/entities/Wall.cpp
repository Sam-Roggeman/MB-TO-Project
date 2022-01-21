#include "Wall.h"

Core::Wall::Wall(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position,
                         const Core::Vector2f& view_size)
    : EntityModel(std::move(camera), position, view_size)
{
}
