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

bool Core::Vector2f::operator==(const Core::Vector2f& other) const { return (x == other.x) && (y == other.y); }

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

Core::Vector2f Core::Vector2f::operator+(const Core::Vector2f& other) const { return {x + other.x, y + other.y}; }

Core::Vector2f Core::Vector2f::operator+(float scalar) const { return {x + scalar, y + scalar}; }

Core::Vector2f Core::operator+(float scalar, const Core::Vector2f& other)
{
        return {scalar + other.x, scalar + other.y};
}

Core::Vector2f Core::Vector2f::operator-(const Core::Vector2f& other) const { return {x - other.x, y - other.y}; }

Core::Vector2f Core::Vector2f::operator-(float scalar) const { return {x - scalar, y - scalar}; }

Core::Vector2f Core::operator-(float scalar, const Core::Vector2f& other)
{
        return {scalar - other.x, scalar - other.y};
}

Core::Vector2f Core::Vector2f::operator*(float scalar) const { return {x * scalar, y * scalar}; }

Core::Vector2f Core::operator*(float scalar, const Core::Vector2f& other)
{
        return {scalar * other.x, scalar * other.y};
}

Core::Vector2f Core::Vector2f::operator/(float scalar) const { return {x / scalar, y / scalar}; }

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

float Core::Vector2f::dotProduct(const Vector2f& other) const { return x * other.x + y * other.y; }

float Core::Vector2f::crossProduct(const Vector2f& other) const { return (x * other.y) - (y * other.x); }

Core::Vector2f& Core::Vector2f::rotate(const Vector2f& pivot_point, float angle) {
        if (pivot_point == *this) return *this;

        // translate point back to origin pivot
        *this -= pivot_point;

        float angle_radian = angle * static_cast<float>(M_PI) / 180.f;

        // rotate endpoint
        Vector2f new_point = {this->x * std::cos(angle_radian) - this->y * std::sin(angle_radian),
                              this->x * std::sin(angle_radian) + this->y * std::cos(angle_radian)};

        // translate point back to pivot point
        *this = new_point + pivot_point;

        return *this;
}

