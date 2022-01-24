#ifndef UABA2_AP_PROJECT_HITBOX_H
#define UABA2_AP_PROJECT_HITBOX_H

#include "../entities/EntityModel.h"
#include "../utils/Utils.h"
#include "Vector2f.h"
#include <vector>

class EntityModel;

namespace Core {
class Hitbox
{
        Vector2f _origin;
        std::vector<Vector2f> _points;
        Vector2f _radius;

        bool _is_passthrough;
        std::vector<Vector2f> _solid_sides;

public:
        Hitbox();
        Hitbox(const Vector2f& origin);
        Hitbox(const Vector2f& origin, float width, float height);
        ~Hitbox() = default;

        Vector2f getOrigin() const;

        void setOrigin(const Vector2f& origin);

        std::vector<Vector2f> getPoints() const;

        void setPoints(const std::vector<Vector2f>& points);

        Vector2f getRadius() const;

        void setRadius(const Vector2f& radius);

        bool isCircle() const;

        bool isEllipse() const;

        void move(const Vector2f& vector);

        void rotate(float angle_radian);

        void rotate(float angle_radian, const Vector2f& pivot);

        void scale(const Vector2f& scale);

        void scale(const Vector2f& scale, const Vector2f& pivot);

        void setSquareHitbox(float side);

        void setRectangleHitbox(float width, float height);

        void setCircleHitbox(float radius);

        void setEllipseHitbox(float a, float b);

        void setPassthrough(bool is_passthrough);

        bool getPassthrough() const;

        void setSolidSides(const std::vector<Vector2f>& solid_sides);
};
} // namespace Core
#endif // UABA2_AP_PROJECT_HITBOX_H
