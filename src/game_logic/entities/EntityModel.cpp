#include "EntityModel.h"

Core::EntityModel::EntityModel(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position,
                               const Core::Vector2f& view_size)
    : _position(position), _rotation(0), _scale(1, 1), _mass(1), _camera(std::move(camera)), _view_size(view_size),
      _input_map(new InputMap()), _is_static(true)
{
        _hitbox = std::make_shared<Core::Hitbox>(_position, _view_size.x, _view_size.y);
}

void Core::EntityModel::update(double t, float dt)
{
        // force acceleration
        _acceleration += _force / _mass;

        Vector2f move_velocity = _velocity + dt * _acceleration / 2;

        // position += timestep * (velocity + timestep * acceleration / 2)
        move(dt * move_velocity);
        _velocity += dt * _acceleration;

        // remove small velocities
        if (_velocity.x < 1.e-2f && _velocity.x > -1.e-2f)
                _velocity.x = 0;
        if (_velocity.y < 1.e-2f && _velocity.y > -1.e-2f)
                _velocity.y = 0;

        // reset forces/accelerations
        _force = {0, 0};
        _acceleration = {0, 0};

        notifyObservers();
}

void Core::EntityModel::setInputMap(const std::shared_ptr<Core::InputMap>& input_map) { _input_map = input_map; }

Core::Vector2f Core::EntityModel::getPosition() const { return _position; }

Core::Vector2f Core::EntityModel::getRepresentationPosition() const { return _camera->projectCoordinate(_position); }

void Core::EntityModel::setPosition(const Core::Vector2f& position)
{
        _hitbox->move(position - _position);
        for (auto& raycast : _raycasts) {
                raycast->move(position - _position);
        }
        _position = position;
}

void Core::EntityModel::move(const Vector2f& vector)
{
        _hitbox->move(vector);
        for (auto& raycast : _raycasts) {
                raycast->move(vector);
        }
        _position += vector;
}

float Core::EntityModel::getRotation() const { return _rotation; }

void Core::EntityModel::setRotation(float angle_radian)
{
        rotate(_rotation);
        rotate(angle_radian);
}

void Core::EntityModel::rotate(float angle_radian)
{
        _hitbox->rotate(angle_radian, _position);
        for (auto& raycast : _raycasts) {
                raycast->rotate(angle_radian, _position);
        }
        _rotation -= angle_radian;
        CoreUtils::fRadianMod(_rotation);
}

void Core::EntityModel::rotate(float angle_radian, const Core::Vector2f& pivot_point)
{
        _hitbox->rotate(angle_radian, pivot_point);
        for (auto& raycast : _raycasts) {
                raycast->rotate(angle_radian, pivot_point);
        }

        _rotation -= angle_radian;
        CoreUtils::fRadianMod(_rotation);
        _position = _position.rotated(angle_radian, pivot_point);
}

Core::Vector2f Core::EntityModel::getScale() const { return _scale; }

void Core::EntityModel::setScale(const Core::Vector2f& scale)
{
        _hitbox->scale({scale.x / _scale.x, scale.y / _scale.y}, _position);
        for (auto& raycast : _raycasts) {
                raycast->scale({scale.x / _scale.x, scale.y / _scale.y}, _position);
        }
        _scale = scale;
}

void Core::EntityModel::scale(const Vector2f& scale)
{
        _hitbox->scale(scale, _position);
        for (auto& raycast : _raycasts) {
                raycast->scale(scale, _position);
        }
        _scale.x *= scale.x;
        _scale.y *= scale.y;
}

Core::Vector2f Core::EntityModel::getForce() const { return _force; }

void Core::EntityModel::setForce(const Vector2f& force) { _force = force; }

float Core::EntityModel::getMass() const { return _mass; }

void Core::EntityModel::setMass(float mass) { _mass = mass; }

Core::Vector2f Core::EntityModel::getAcceleration() const { return _acceleration; }

void Core::EntityModel::setAcceleration(const Vector2f& acceleration) { _acceleration = acceleration; }

Core::Vector2f Core::EntityModel::getVelocity() const { return _velocity; }

void Core::EntityModel::setVelocity(const Core::Vector2f& velocity) { _velocity = velocity; }

Core::Vector2f Core::EntityModel::getViewSize() const { return _view_size; }
Core::Vector2f Core::EntityModel::getAbsoluteViewSize() const
{
        return {_view_size.x * _scale.x, _view_size.y * _scale.y};
}

Core::Vector2f Core::EntityModel::getRepresentationViewSize() const { return _camera->projectSize(_view_size); }

void Core::EntityModel::setViewSize(const Core::Vector2f& view_size) { _view_size = view_size; }

std::shared_ptr<Core::Hitbox> Core::EntityModel::getHitbox() const { return _hitbox; }

std::shared_ptr<Core::Hitbox> Core::EntityModel::getRepresentationHitbox() const
{
        std::shared_ptr<Core::Hitbox> representation_hitbox(new Core::Hitbox());

        representation_hitbox->setOrigin(_camera->projectCoordinate(_hitbox->getOrigin()));

        std::vector<Vector2f> new_points;
        for (auto& point : _hitbox->getPoints()) {
                new_points.push_back(_camera->projectCoordinate(point));
        }
        representation_hitbox->setPoints(new_points);

        representation_hitbox->setRadius(_camera->projectSize(_hitbox->getRadius()));

        return representation_hitbox;
}

bool Core::EntityModel::getStatic() const { return _is_static; }

void Core::EntityModel::setStatic(bool is_static) { _is_static = is_static; }

std::shared_ptr<Core::Raycast> Core::EntityModel::getRaycast(unsigned int raycast_id) const
{
        return _raycasts[raycast_id];
}

std::vector<std::shared_ptr<Core::Raycast>> Core::EntityModel::getRaycasts() const { return _raycasts; }

std::vector<std::shared_ptr<Core::Raycast>> Core::EntityModel::getRepresentationRaycasts() const
{
        std::vector<std::shared_ptr<Raycast>> new_raycasts;
        for (const auto& raycast : _raycasts) {
                std::shared_ptr<Raycast> new_raycast;
                if (raycast->isActivated()) {
                        new_raycast =
                            std::make_shared<Core::Raycast>(_camera->projectCoordinate(raycast->getOrigin()),
                                                            _camera->projectCoordinate(raycast->getCollisionPoint()));
                } else {
                        new_raycast =
                            std::make_shared<Core::Raycast>(_camera->projectCoordinate(raycast->getOrigin()),
                                                            _camera->projectCoordinate(raycast->getEndpoint()));
                }

                new_raycasts.push_back(new_raycast);
        }

        return new_raycasts;
}

unsigned int Core::EntityModel::addRaycast(const std::shared_ptr<Core::Raycast>& raycast)
{
        _raycasts.push_back(raycast);
        return _raycasts.size() - 1;
}