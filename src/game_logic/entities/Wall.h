#ifndef UABA2_AP_PROJECT_PLATFORM_H
#define UABA2_AP_PROJECT_PLATFORM_H

#include "EntityModel.h"
#include <utility>

namespace Core {
class Wall : public EntityModel
{
public:
        Wall(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size);
        ~Wall() = default;
};
} // namespace Core
#endif // UABA2_AP_PROJECT_PLATFORM_H
