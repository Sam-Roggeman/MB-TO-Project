#ifndef UABA2_AP_PROJECT_CAR_H
#define UABA2_AP_PROJECT_CAR_H

#include "../CoreConstants.h"
#include "EntityModel.h"

namespace Core {
class Car : public EntityModel
{
private:
        Vector2f _back_pivot_point;
        Vector2f _front_pivot_point;

        float _acceleration_power;
        float _braking_power;
        float _rotation_angle;

public:
        Car(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size);
        ~Car() = default;

        void update(float t, float dt) override;

        void setPosition(const Vector2f& position) override;

        void move(const Vector2f &vector) override;

        void setRotation(float rotation) override;

        void rotate(float angle) override;

        void rotate(float angle, const Core::Vector2f& pivot_point) override;

        void scale(const Vector2f& scale) override;

        void setScale(const Vector2f& scale) override;

        void setBackPivotPoint(const Vector2f& pivot_point);

        void setFrontPivotPoint(const Vector2f& pivot_point);

};
} // namespace Core

#endif // UABA2_AP_PROJECT_CAR_H
