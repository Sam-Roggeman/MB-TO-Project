#ifndef UABA2_AP_PROJECT_CAR_H
#define UABA2_AP_PROJECT_CAR_H

#include "../CoreConstants.h"
#include "../utils/json.hpp"
#include "EntityModel.h"
#include <fstream>

namespace Core {
class Car : public EntityModel
{
private:
        Vector2f _direction;

        float _acceleration_power;
        float _reverse_acceleration_power;
        float _braking_power;

        float _steering_angle;
        float _wheel_base;

        float _drag;
        float _friction;

        float _max_slip_velocity;
        float _min_traction;
        float _max_traction;

public:
        Car(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size);
        ~Car() = default;

        void update(double t, float dt) override;

        void reset();

        void steer(float angle_radian, float direction_sign, float dt);

        void loadPhysicsPreset(const std::string& preset_file_path);

        float getAccelerationPower() const;
        void setAccelerationPower(float value);

        float getReverseAccelerationPower() const;
        void setReverseAccelerationPower(float value);

        float getBrakingPower() const;
        void setBrakingPower(float value);

        float getSteeringAngle() const;
        void setSteeringAngle(float value);

        float getWheelBase() const;
        void setWheelBase(float value);

        float getDrag() const;
        void setDrag(float value);

        float getFriction() const;
        void setFriction(float value);

        float getMaxSlipVelocity() const;
        void setMaxSlipVelocity(float value);

        float getMinTraction() const;
        void setMinTraction(float value);

        float getMaxTraction() const;
        void setMaxTraction(float value);
};
} // namespace Core

#endif // UABA2_AP_PROJECT_CAR_H
