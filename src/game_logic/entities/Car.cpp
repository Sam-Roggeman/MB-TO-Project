#include "Car.h"

Core::Car::Car(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size)
    : EntityModel(std::move(camera), position, view_size)
{
        _direction = {0, 1};
        _mass = 1500;

        _acceleration_power = 2000;
        _reverse_acceleration_power = 1000;
        _braking_power = 6000;

        _steering_angle = CoreUtils::toRadian(45);
        _wheel_base = view_size.y * 0.9f;

        _drag = 0.015;
        _friction = 0.2;

        _max_slip_velocity = 4;
        _min_traction = 1;
        _max_traction = 0.05;
}

void Core::Car::update(double t, float dt)
{
        // reset
        if (_input_map->reset) {
                reset();
        }

        // direction
        float velocity_dot_direction = _velocity.dotProduct(_direction);

        bool going_forwards = velocity_dot_direction > 0;
        float direction_sign = 1;
        if (!going_forwards) {
                direction_sign = -1;
        }

        // friction & drag
        Vector2f friction_force = _velocity * _friction;
        Vector2f drag_force = _velocity * _velocity.length() * _drag;
        _acceleration -= friction_force + drag_force;

        // steering & traction
        float alpha = _velocity.length() / _max_slip_velocity;
        alpha = 1.f - (1.f - alpha) * (1.f - alpha);
        alpha = std::clamp(alpha, 0.f, 1.f);

        float current_angle = _steering_angle * CoreUtils::lerp(_min_traction, _max_traction, alpha);

        if (_input_map->left > 0) {
                steer(current_angle * _input_map->left, direction_sign, dt);
        }
        if (_input_map->right > 0) {
                steer(-current_angle * _input_map->right, direction_sign, dt);
        }

        // acceleration
        if (_input_map->up > 0) {
                if (going_forwards) {
                        _force += _direction * _acceleration_power * _input_map->up;
                } else {
                        // braking
                        _force += _direction * _braking_power * _input_map->up;
                }
        }
        if (_input_map->down > 0) {
                if (!going_forwards) {
                        _force -= _direction * _reverse_acceleration_power * _input_map->down;
                } else {
                        // braking
                        _force -= _direction * _braking_power * _input_map->down;
                }
        }

        // raycasts
        for (auto& raycast : _raycasts) {
                raycast->setActivated(false);
        }

        // camera
        _camera->setPosition(_position);

        Core::EntityModel::update(t, dt);
}

void Core::Car::reset()
{
        setPosition({0, 0});
        _velocity.clear();
        _force.clear();

        _direction = {0, 1};
        setRotation(0);
}

void Core::Car::steer(float angle_radian, float direction_sign, float dt)
{
        Vector2f back_wheels = _position - _wheel_base / 2 * _direction;
        Vector2f front_wheels = _position + _wheel_base / 2 * _direction;

        back_wheels += dt * _velocity;
        front_wheels += dt * _velocity.rotated(angle_radian, {0, 0});

        _direction = (front_wheels - back_wheels).normalized();
        _velocity = direction_sign * _velocity.length() * _direction;

        float new_direction_rotation = atan2f(_direction.y, _direction.x) - static_cast<float>(M_PI) / 2.f;
        setRotation(new_direction_rotation);
}

void Core::Car::loadPhysicsPreset(const std::string& preset_file_path)
{
        std::ifstream input(preset_file_path);
        nlohmann::json j;
        input >> j;

        _acceleration_power = j.find("acceleration_power")->get<float>();
        _reverse_acceleration_power = j.find("reverse_acceleration_power")->get<float>();
        _braking_power = j.find("braking_power")->get<float>();

        _steering_angle = CoreUtils::toRadian(j.find("steering_angle")->get<float>());
        _wheel_base = _view_size.y * j.find("wheel_base")->get<float>();

        _drag = j.find("drag")->get<float>();
        _friction = j.find("friction")->get<float>();

        _max_slip_velocity = j.find("max_slip_velocity")->get<float>();
        _min_traction = j.find("min_traction")->get<float>();
        _max_traction = j.find("max_traction")->get<float>();
}

float Core::Car::getAccelerationPower() const { return _acceleration_power; }
void Core::Car::setAccelerationPower(float value) { _acceleration_power = value; }

float Core::Car::getReverseAccelerationPower() const { return _reverse_acceleration_power; }
void Core::Car::setReverseAccelerationPower(float value) { _reverse_acceleration_power = value; }

float Core::Car::getBrakingPower() const { return _braking_power; }
void Core::Car::setBrakingPower(float value) { _braking_power = value; }

float Core::Car::getSteeringAngle() const { return _steering_angle; }
void Core::Car::setSteeringAngle(float value) { _steering_angle = value; }

float Core::Car::getWheelBase() const { return _wheel_base; }
void Core::Car::setWheelBase(float value) { _wheel_base = value; }

float Core::Car::getDrag() const { return _drag; }
void Core::Car::setDrag(float value) { _drag = value; }

float Core::Car::getFriction() const { return _friction; }
void Core::Car::setFriction(float value) { _friction = value; }

float Core::Car::getMaxSlipVelocity() const { return _max_slip_velocity; }
void Core::Car::setMaxSlipVelocity(float value) { _max_slip_velocity = value; }

float Core::Car::getMinTraction() const { return _min_traction; }
void Core::Car::setMinTraction(float value) { _min_traction = value; }

float Core::Car::getMaxTraction() const { return _max_traction; }
void Core::Car::setMaxTraction(float value) { _max_traction = value; }