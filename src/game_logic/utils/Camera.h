#ifndef UABA2_AP_PROJECT_CAMERA_H
#define UABA2_AP_PROJECT_CAMERA_H

#include "../data_structures/Vector2f.h"
#include <cmath>

namespace Core {
class Camera
{
private:
        Vector2f _position;

        Vector2f _camera_x_bounderies;
        Vector2f _camera_y_bounderies;
        Vector2f _representation_x_bounderies;
        Vector2f _representation_y_bounderies;

        bool _is_focused;

public:
        Camera();

        bool getFocused() const;
        void setFocused(bool is_focused);

        void move(const Vector2f& vector);

        void reset();

        Vector2f getPosition() const;
        void setPosition(const Vector2f& position);

        Vector2f getXBounderies() const;
        Vector2f getYBounderies() const;

        void setCameraBounderies(float x_min, float x_max, float y_min, float y_max);

        float getCamerawidth() const;
        float getCameraheight() const;

        Vector2f getRepresentationXBounderies() const;
        Vector2f getRepresentationYBounderies() const;

        void setRepresentationBounderies(float x_min, float x_max, float y_min, float y_max);

        Vector2f projectCoordinateWorldToRepresentation(const Vector2f& point) const;
        Vector2f projectCoordinateRepresentationToWorld(const Vector2f& point) const;
        Vector2f projectCoordinateCustomToWorld(const Vector2f& point, float x_min, float x_max, float y_min, float y_max) const;

        Core::Vector2f projectSize(const Core::Vector2f& size) const;
        Core::Vector2f projectSize(const Core::Vector2f& size, float x_min, float x_max, float y_min,
                                   float y_max) const;
};
} // namespace Core
#endif // UABA2_AP_PROJECT_CAMERA_H
