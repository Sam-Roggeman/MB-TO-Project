#include "Car.h"
Core::Car::Car(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size)
    : EntityModel(std::move(camera), position, view_size)
{
        _mass = 1500;
        _drag = {1, 1};
        _acceleration_power = 5000;
        _braking_power = 5000;
        _rotation_angle = 120;
}

void Core::Car::update(double t, float dt)
{
        Vector2f front_direction = _position - _back_pivot_point;
        front_direction.normalize();

        float velocity_dot_direction = _velocity.dotProduct(front_direction);
        bool going_forwards = true;

        if (velocity_dot_direction < 0)
                going_forwards = false;

        if (going_forwards) {
                _velocity = front_direction * _velocity.length();
        } else {
                _velocity = -1 * front_direction * _velocity.length();
        }

        if (_input_map->up > 0) {
                _force += front_direction * _acceleration_power;
        }

        if (_input_map->down > 0) {
                _force -= front_direction * _braking_power;
        }

        float turning_radius{0};
        if (_velocity.length() > 0.01) {
                turning_radius = _velocity.length() * 3;
        }

        if (turning_radius > 1) turning_radius = 1;
//        float turning_radius = std::pow(_velocity.length(), 2);

        // _rotation
        if (_input_map->left > 0) {
                if (going_forwards)
                        rotate(_rotation_angle * dt * turning_radius, _back_pivot_point);
                else
                        rotate(-_rotation_angle * dt * turning_radius, _front_pivot_point);
        }

        if (_input_map->right > 0) {
                if (going_forwards)
                        rotate(-_rotation_angle * dt * turning_radius, _back_pivot_point);
                else
                        rotate(_rotation_angle * dt * turning_radius, _front_pivot_point);
        }

        for (auto& raycast : _raycasts) {
                raycast->setActivated(false);
        }

        EntityModel::update(t, dt);
}

void Core::Car::setPosition(const Core::Vector2f& position)
{
        _back_pivot_point += position - _position;
        _front_pivot_point += position - _position;
        EntityModel::setPosition(position);
}

void Core::Car::move(const Core::Vector2f& vector)
{
        _back_pivot_point += vector;
        _front_pivot_point += vector;
        EntityModel::move(vector);
}

void Core::Car::setRotation(float rotation)
{
        _back_pivot_point.rotate(rotation - _rotation, _position);
        _front_pivot_point.rotate(rotation - _rotation, _position);
        EntityModel::setRotation(rotation);
}

void Core::Car::rotate(float angle)
{
        _back_pivot_point.rotate(angle, _position);
        _front_pivot_point.rotate(angle, _position);
        EntityModel::rotate(angle);
}

void Core::Car::rotate(float angle, const Core::Vector2f& pivot_point)
{
        _back_pivot_point.rotate(angle, pivot_point);
        _front_pivot_point.rotate(angle, pivot_point);
        EntityModel::rotate(angle, pivot_point);
}

void Core::Car::scale(const Core::Vector2f& scale)
{
        _back_pivot_point.scale(scale, _position);
        _front_pivot_point.scale(scale, _position);
        EntityModel::scale(scale);
}

void Core::Car::setScale(const Core::Vector2f& scale)
{
        _back_pivot_point.scale({scale.x / _scale.x, scale.y / _scale.y}, _position);
        _front_pivot_point.scale({scale.x / _scale.x, scale.y / _scale.y}, _position);
        EntityModel::setScale(scale);
}

void Core::Car::setBackPivotPoint(const Core::Vector2f& pivot_point) { _back_pivot_point = pivot_point; }

void Core::Car::setFrontPivotPoint(const Core::Vector2f& pivot_point) { _front_pivot_point = pivot_point; }
