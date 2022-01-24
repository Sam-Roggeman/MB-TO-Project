#include "Car.h"
Core::Car::Car(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size)
    : EntityModel(std::move(camera), position, view_size), _is_rotating_left(false), _is_rotating_right(false)
{
        _direction = {0, 1};
        _mass = 1500;
        _drag = {0.25, 0.25};
        _acceleration_power = 2000;
        _braking_power = 2000;
}

void Core::Car::update(double t, float dt)
{
        float velocity_dot_direction = _velocity.dotProduct(_direction);

        bool going_forwards = true;
        if (velocity_dot_direction < 0)
                going_forwards = false;

        if (_input_map->up > 0) {
                _force += _direction * _acceleration_power;
        }

        if (_input_map->down > 0) {
                _force -= _direction * _braking_power;
        }

        float turning_radius = 0.3;

        // _rotation
        if (_input_map->left > 0) {
                if (!_is_rotating_left) {
                        // calculate and store left pivot point
                        Vector2f pivot_direction = _direction;
                        pivot_direction.rotate(M_PI / 2, {0, 0});
                        _pivot_point = _position + (pivot_direction * turning_radius );

                        // set _is_rotating_left true
                        _is_rotating_left = true;
                }
                // rotate around pivot point
                float angle = CoreUtils::toRadian(45) * dt;

                rotate(angle, _pivot_point);
                _direction = _direction.rotate(angle, _pivot_point);
        } else {
                _is_rotating_left = false;
        }

        if (_input_map->right > 0) {

        }

        for (auto& raycast : _raycasts) {
                raycast->setActivated(false);
        }

        _camera->setPosition(_position);

        EntityModel::update(t, dt);
}