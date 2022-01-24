#include "Raycast.h"

Core::Raycast::Raycast() : _activated(false) {}

Core::Raycast::Raycast(const Core::Vector2f& origin, const Core::Vector2f& endpoint)
    : _activated(false), _origin(origin), _endpoint(endpoint)
{
}

Core::Raycast::Raycast(const Core::Vector2f& origin, const Core::Vector2f& direction, float length)
    : _activated(false), _origin(origin)
{
        _endpoint = origin + (direction * length);
}

Core::Vector2f Core::Raycast::getOrigin() const { return _origin; }

void Core::Raycast::setOrigin(const Core::Vector2f& origin) { _origin = origin; }

Core::Vector2f Core::Raycast::getEndpoint() const { return _endpoint; }

void Core::Raycast::setEndpoint(const Core::Vector2f& endpoint) { _endpoint = endpoint; }

void Core::Raycast::move(const Core::Vector2f& vector)
{
        _origin += vector;
        _endpoint += vector;
}

void Core::Raycast::rotate(float angle_radian)
{
        _endpoint.rotate(angle_radian, _origin);
}

void Core::Raycast::rotate(float angle_radian, const Core::Vector2f& pivot)
{
        _endpoint.rotate(angle_radian, pivot);
        _origin.rotate(angle_radian, pivot);
}

void Core::Raycast::scale(const Core::Vector2f& scale)
{
        _endpoint.scale(scale, _origin);
}

void Core::Raycast::scale(const Core::Vector2f& scale, const Core::Vector2f& pivot)
{
        _origin.scale(scale, pivot);
        _endpoint.scale(scale, pivot);
}

Core::Vector2f Core::Raycast::getDirection() const
{
        Vector2f direction = _endpoint - _origin;
        direction.normalize();
        return direction;
}

void Core::Raycast::setDirection(const Core::Vector2f& direction) { _endpoint = _origin + (direction * getLength()); }

float Core::Raycast::getLength() const { return (_endpoint - _origin).length(); }

void Core::Raycast::setLength(float length) { _endpoint = _origin + (getDirection() * length); }

float Core::Raycast::getCollisionLength() const
{
        if (_activated)
                return (_collision_point - _origin).length();
        return 0;
}

bool Core::Raycast::isActivated() const { return _activated; }

void Core::Raycast::setActivated(bool activated) { _activated = activated; }

Core::Vector2f Core::Raycast::getCollisionPoint() const { return _collision_point; }

void Core::Raycast::setCollisionPoint(const Core::Vector2f& collision_point) { _collision_point = collision_point; }
