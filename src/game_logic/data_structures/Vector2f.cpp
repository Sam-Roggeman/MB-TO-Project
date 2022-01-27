#include "Vector2f.h"

Core::Vector2f::Vector2f() : x(0), y(0) {}

Core::Vector2f::Vector2f(float _x, float _y) : x(_x), y(_y) {}

Core::Vector2f::Vector2f(const Core::Vector2f& other)
{
        x = other.x;
        y = other.y;
}

Core::Vector2f& Core::Vector2f::operator=(const Core::Vector2f& other)
{
        if (this != &other) {
                x = other.x;
                y = other.y;
        }
        return *this;
}

Core::Vector2f Core::Vector2f::operator+(const Core::Vector2f& other) const { return {x + other.x, y + other.y}; }

Core::Vector2f Core::Vector2f::operator+(float scalar) const { return {x + scalar, y + scalar}; }

Core::Vector2f Core::operator+(float scalar, const Core::Vector2f& other)
{
        return {scalar + other.x, scalar + other.y};
}

Core::Vector2f& Core::Vector2f::operator+=(const Core::Vector2f& other)
{
        x += other.x;
        y += other.y;
        return *this;
}

Core::Vector2f& Core::Vector2f::operator+=(float scalar)
{
        x += scalar;
        y += scalar;
        return *this;
}

Core::Vector2f Core::Vector2f::operator-(const Core::Vector2f& other) const { return {x - other.x, y - other.y}; }

Core::Vector2f Core::Vector2f::operator-(float scalar) const { return {x - scalar, y - scalar}; }

Core::Vector2f Core::operator-(float scalar, const Core::Vector2f& other)
{
        return {scalar - other.x, scalar - other.y};
}

Core::Vector2f& Core::Vector2f::operator-=(const Core::Vector2f& other)
{
        x -= other.x;
        y -= other.y;
        return *this;
}

Core::Vector2f& Core::Vector2f::operator-=(float scalar)
{
        x -= scalar;
        y -= scalar;
        return *this;
}

Core::Vector2f Core::Vector2f::operator*(float scalar) const { return {x * scalar, y * scalar}; }

Core::Vector2f Core::operator*(float scalar, const Core::Vector2f& other)
{
        return {scalar * other.x, scalar * other.y};
}

Core::Vector2f& Core::Vector2f::operator*=(float scalar)
{
        x *= scalar;
        y *= scalar;
        return *this;
}

Core::Vector2f Core::Vector2f::operator/(float scalar) const { return {x / scalar, y / scalar}; }

Core::Vector2f Core::operator/(float scalar, const Core::Vector2f& other)
{
        return {other.x / scalar, other.y / scalar};
}

Core::Vector2f& Core::Vector2f::operator/=(float scalar)
{
        x /= scalar;
        y /= scalar;
        return *this;
}

bool Core::Vector2f::operator==(const Core::Vector2f& other) const { return (x == other.x) && (y == other.y); }

bool Core::Vector2f::operator<(const Core::Vector2f& other) const { return (x < other.x) && (y < other.y); }

bool Core::Vector2f::operator<=(const Core::Vector2f& other) const { return (x <= other.x) && (y <= other.y); }

bool Core::Vector2f::operator>(const Core::Vector2f& other) const { return (x > other.x) && (y > other.y); }

bool Core::Vector2f::operator>=(const Core::Vector2f& other) const { return (x >= other.x) && (y >= other.y); }

std::ostream& Core::operator<<(std::ostream& os, const Core::Vector2f& vector)
{
        os << "(" << vector.x << ", " << vector.y << ")";
        return os;
}

bool Core::Vector2f::empty() const { return x == 0 & y == 0; }

void Core::Vector2f::clear()
{
        x = 0;
        y = 0;
}

float Core::Vector2f::length() const { return static_cast<float>(std::sqrt(std::pow(x, 2) + std::pow(y, 2))); }

void Core::Vector2f::normalize()
{
        float length = this->length();
        x /= length;
        y /= length;
}

Core::Vector2f Core::Vector2f::normalized()
{
        Vector2f new_vector = *this;
        new_vector.normalize();
        return new_vector;
}

float Core::Vector2f::dotProduct(const Vector2f& other) const { return x * other.x + y * other.y; }

float Core::Vector2f::crossProduct(const Vector2f& other) const { return (x * other.y) - (y * other.x); }

void Core::Vector2f::rotate(float angle_radian, const Vector2f& pivot_point)
{
        if (pivot_point == *this)
                return;

        // translate point back to origin pivot
        *this -= pivot_point;

        // rotated endpoint
        Vector2f new_point = {this->x * std::cos(angle_radian) - this->y * std::sin(angle_radian),
                              this->x * std::sin(angle_radian) + this->y * std::cos(angle_radian)};

        // translate point back to pivot point
        *this = pivot_point + new_point;
}

Core::Vector2f Core::Vector2f::rotated(float angle_radian, const Vector2f& pivot_point) const
{
        if (pivot_point == *this)
                return *this;

        Vector2f rotated_point = *this;

        rotated_point.rotate(angle_radian, pivot_point);

        return rotated_point;
}

void Core::Vector2f::scale(const Vector2f& scale, const Vector2f& pivot_point)
{
        if (pivot_point == *this)
                return;

        // translate point back to origin pivot
        *this -= pivot_point;

        // scaled endpoint
        this->x *= scale.x;
        this->y *= scale.y;

        // translate point back to pivot point
        *this += pivot_point;
}

Core::Vector2f Core::Vector2f::scaled(const Core::Vector2f& scale, const Core::Vector2f& pivot_point) const
{

        if (pivot_point == *this)
                return *this;

        Vector2f scaled_point = *this;

        scaled_point.scale(scale, pivot_point);

        return scaled_point;
}
