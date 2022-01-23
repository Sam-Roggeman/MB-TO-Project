#ifndef UABA2_AP_PROJECT_VECTOR2F_H
#define UABA2_AP_PROJECT_VECTOR2F_H

#include <cmath>
#include <iostream>

namespace Core {
class Vector2f
{
public:
        float x;
        float y;

        Vector2f();

        Vector2f(float _x, float _y);

        Vector2f(const Vector2f& other);

        Vector2f& operator=(const Vector2f& other);

        Vector2f operator+(const Vector2f& other) const;

        Vector2f operator+(float scalar) const;

        friend Vector2f operator+(float scalar, const Vector2f& other);

        Vector2f& operator+=(const Vector2f& other);

        Vector2f& operator+=(float scalar);

        Vector2f operator-(const Vector2f& other) const;

        Vector2f operator-(float scalar) const;

        friend Vector2f operator-(float scalar, const Vector2f& other);

        Vector2f& operator-=(const Vector2f& other);

        Vector2f& operator-=(float scalar);

        Vector2f operator*(float scalar) const;

        friend Vector2f operator*(float scalar, const Vector2f& other);

        Vector2f& operator*=(float scalar);

        Vector2f operator/(float scalar) const;

        friend Vector2f operator/(float scalar, const Vector2f& other);

        Vector2f& operator/=(float scalar);

        bool operator==(const Vector2f& other) const;

        bool operator<(const Vector2f& other) const;

        bool operator<=(const Vector2f& other) const;

        bool operator>(const Vector2f& other) const;

        bool operator>=(const Vector2f& other) const;

        friend std::ostream& operator<<(std::ostream& os, const Vector2f& vector);

        bool empty() const;

        void clear();

        float length() const;

        void normalize();

        float dotProduct(const Vector2f& other) const;

        float crossProduct(const Vector2f& other) const;

        Vector2f& rotate(float angle_radian, const Vector2f& pivot_point);

        Vector2f& scale(const Vector2f& scale, const Vector2f& pivot_point);
};
} // namespace Core

#endif // UABA2_AP_PROJECT_VECTOR2F_H
