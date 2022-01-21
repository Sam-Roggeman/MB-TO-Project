#include "Hitbox.h"

Core::Hitbox::Hitbox() : _is_passthrough(false) {}

Core::Hitbox::Hitbox(const Core::Vector2f& origin) : _origin(origin) {}

Core::Hitbox::Hitbox(const Core::Vector2f& origin, float width, float height) : _origin(origin)
{
        setRectangleHitbox(width, height);
}

Core::Vector2f Core::Hitbox::getOrigin() const { return _origin; }

void Core::Hitbox::setOrigin(const Core::Vector2f& origin) { _origin = origin; }

std::vector<Core::Vector2f> Core::Hitbox::getPoints() const { return _points; }

void Core::Hitbox::setPoints(const std::vector<Vector2f>& points) { _points = points; }

Core::Vector2f Core::Hitbox::getRadius() const { return _radius; }

void Core::Hitbox::setRadius(const Core::Vector2f& radius) { _radius = radius; }

bool Core::Hitbox::isCircle() const { return !_radius.empty() && _radius.x == _radius.y; }

bool Core::Hitbox::isEllipse() const { return !_radius.empty() && _radius.x != _radius.y; }

void Core::Hitbox::move(const Core::Vector2f& vector)
{
        _origin += vector;

        for (auto& point : _points) {
                point += vector;
        }
}

void Core::Hitbox::rotate(float angle)
{
        float angle_radian = angle * static_cast<float>(M_PI) / 180.f;
        for (auto& point : _points) {
                // translate point back to origin
                point -= _origin;

                // rotate point
                Vector2f new_point = {point.x * std::cos(angle_radian) - point.y * std::sin(angle_radian),
                                      point.x * std::sin(angle_radian) + point.y * std::cos(angle_radian)};

                // translate point back to pivot point
                point = new_point + _origin;
        }
}

void Core::Hitbox::rotate(float angle, const Vector2f& pivot)
{
        float angle_radian = angle * static_cast<float>(M_PI) / 180.f;

        // translate point to pivot
        _origin -= pivot;

        // rotate point
        Vector2f new_origin = {_origin.x * std::cos(angle_radian) - _origin.y * std::sin(angle_radian),
                               _origin.x * std::sin(angle_radian) + _origin.y * std::cos(angle_radian)};

        // translate point back to pivot point
        _origin = new_origin + pivot;

        for (auto& point : _points) {
                // translate point to pivot origin
                point -= pivot;

                // rotate point
                Vector2f new_point = {point.x * std::cos(angle_radian) - point.y * std::sin(angle_radian),
                                      point.x * std::sin(angle_radian) + point.y * std::cos(angle_radian)};

                // translate point back to pivot point
                point = new_point + pivot;
        }
}

void Core::Hitbox::scale(const Core::Vector2f& scale)
{
        for (auto& point : _points) {
                // translate point back to origin
                point -= _origin;

                // scale point
                point.x = point.x * scale.x;
                point.y = point.y * scale.y;

                // translate point back to pivot point
                point += _origin;
        }
        _radius = _radius * (scale.x + scale.y) / 2;
}

void Core::Hitbox::scale(const Vector2f& scale, const Vector2f& pivot)
{
        // translate origin back to pivot origin
        _origin -= pivot;

        // scale point
        _origin.x = _origin.x * scale.x;
        _origin.y = _origin.y * scale.y;

        // translate origin back to pivot point
        _origin += pivot;

        for (auto& point : _points) {
                // translate point back to pivot origin
                point -= pivot;

                // scale point
                point.x = point.x * scale.x;
                point.y = point.y * scale.y;

                // translate point back to pivot point
                point += pivot;
        }
        _radius = _radius * (scale.x + scale.y) / 2;
}

void Core::Hitbox::setSquareHitbox(float side)
{
        _points = {{_origin.x - side / 2, _origin.y - side / 2},
                   {_origin.x - side / 2, _origin.y + side / 2},
                   {_origin.x + side / 2, _origin.y + side / 2},
                   {_origin.x + side / 2, _origin.y - side / 2}};

        _radius.clear();
}

void Core::Hitbox::setRectangleHitbox(float width, float height)
{
        _points = {{_origin.x - width / 2, _origin.y - height / 2},
                   {_origin.x - width / 2, _origin.y + height / 2},
                   {_origin.x + width / 2, _origin.y + height / 2},
                   {_origin.x + width / 2, _origin.y - height / 2}};

        _radius.clear();
}

void Core::Hitbox::setCircleHitbox(float radius)
{
        _points.clear();

        _radius = {radius, radius};
}

void Core::Hitbox::setEllipseHitbox(float a, float b)
{
        _points.clear();

        _radius = {a, b};
}

void Core::Hitbox::setPassthrough(bool is_passthrough) { _is_passthrough = is_passthrough; }

bool Core::Hitbox::getPassthrough() const { return _is_passthrough; }

void Core::Hitbox::setSolidSides(const std::vector<Vector2f>& solid_sides) { _solid_sides = solid_sides; }
