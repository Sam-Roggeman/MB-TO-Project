#ifndef UABA2_AP_PROJECT_DOODLE_H
#define UABA2_AP_PROJECT_DOODLE_H

#include "../CoreConstants.h"
#include "EntityModel.h"

namespace Core {
class Doodle : public EntityModel
{
private:
        bool _standing{false};
        bool _hit_platform{false};

        float _horizontal_movement_force;

        float _initial_jump_velocity;
        float _gravitational_acceleration;

public:
        Doodle(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size);
        ~Doodle() = default;

        void update(float t, float dt) override;

        void setHitPlatform(bool hit_platform);
};
} // namespace Core
#endif // UABA2_AP_PROJECT_DOODLE_H
