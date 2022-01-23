#ifndef UABA2_AP_PROJECT_RAYCAST_H
#define UABA2_AP_PROJECT_RAYCAST_H

#include "Vector2f.h"

namespace Core {
class Raycast
{
private:
        Vector2f _origin;
        Vector2f _endpoint;

        bool _activated;
        Vector2f _collision_point;

public:
        Raycast();
        Raycast(const Vector2f& origin, const Vector2f& endpoint);
        /**
         * The _direction of the raycast must be normalized.
         */
        Raycast(const Vector2f& origin, const Vector2f& direction, float length);

        ~Raycast() = default;

        Vector2f getOrigin() const;
        void setOrigin(const Vector2f& origin);

        Vector2f getEndpoint() const;
        void setEndpoint(const Vector2f& endpoint);

        void move(const Vector2f& vector);

        void rotate(float angle);

        void rotate(float angle, const Vector2f& pivot);

        void scale(const Vector2f& scale);

        void scale(const Vector2f& scale, const Vector2f& pivot);

        Vector2f getDirection() const;
        void setDirection(const Vector2f& direction);

        float getLength() const;
        void setLength(float length);

        float getCollisionLength() const;

        bool isActivated() const;
        void setActivated(bool activated);

        Vector2f getCollisionPoint() const;
        void setCollisionPoint(const Vector2f& collision_point);
};
} // namespace Core
#endif // UABA2_AP_PROJECT_RAYCAST_H
