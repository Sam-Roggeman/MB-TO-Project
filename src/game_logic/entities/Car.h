#ifndef UABA2_AP_PROJECT_CAR_H
#define UABA2_AP_PROJECT_CAR_H

#include "../CoreConstants.h"
#include "EntityModel.h"

namespace Core {
class Car : public EntityModel
{
private:
        Vector2f _direction;
        float _acceleration_power;
        float _braking_power;

        bool _is_rotating_left;
        bool _is_rotating_right;
        Vector2f _pivot_point;

public:
        Car(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size);
        ~Car() = default;

        void update(double t, float dt) override;

};
} // namespace Core

#endif // UABA2_AP_PROJECT_CAR_H
