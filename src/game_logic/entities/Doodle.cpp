#include "Doodle.h"

Core::Doodle::Doodle(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position,
                     const Core::Vector2f& view_size)
    : Core::EntityModel(std::move(camera), position, view_size)
{
        float jump_dt = 0.6;
        float jump_height = 1;

        _gravitational_acceleration = -2 * jump_height / (jump_dt * jump_dt);
        _initial_jump_velocity = 2 * jump_height / jump_dt;

        _horizontal_movement_force = 200;
        _drag.x = 0;
        _drag.y = 0;

        _mass = 20;
        _max_velocity = {1.4, _initial_jump_velocity * 1.5f};
}

void Core::Doodle::update(double t, float dt)
{
        // check raycast under doodle
        if (getRaycasts()[0]->isActivated() && _hit_platform) {
                _standing = true;
                _velocity.y = 0;
                notifyObservers("stand");
        } else {
                _standing = false;
        }

        getRaycasts()[0]->setActivated(false);

        // ground collision
        if (_hitbox->getOrigin().y - (_view_size.y / 2) < 0) {
                setPosition({_position.x, _view_size.y / 2});
                _standing = true;
                _velocity.y = 0;
                notifyObservers("stand");
        }

        _hit_platform = false;

        // reset player
        if (_input_map->reset) {
                setPosition({0, 1});
                _input_map->reset = false;
                _standing = false;
        }

        // _jumping
        if (_standing && _input_map->up > 0) {
                _velocity.y = _initial_jump_velocity;
                _standing = false;
                notifyObservers("jump");
        }

        // gravitational acceleration
        if (!_standing) {
                _acceleration.y += _gravitational_acceleration;
        }

        // left / right movement
        if (_input_map->right > 0) {
                _force.x += _horizontal_movement_force;
                notifyObservers("right");
        }
        if (_input_map->left > 0) {
                _force.x -= _horizontal_movement_force;
                notifyObservers("left");
        }

        // side scrolling
        if (_position.x <= CoreConstants::world_x_min) {
                setPosition({CoreConstants::world_x_max, _position.y});
        } else if (_position.x >= CoreConstants::world_x_max) {
                setPosition({CoreConstants::world_x_min, _position.y});
        }

        // camera scrolling infinite
        //        _camera->setPosition(_position);

        // camera scrolling y+ infinite
        //        if (_position.y > _camera->getPosition().y) {
        //                _camera->setPosition({0, _position.y});
        //        }

        EntityModel::update(t, dt);
}

void Core::Doodle::setHitPlatform(bool hit_platform) { _hit_platform = hit_platform; }
