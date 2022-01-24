#ifndef UABA2_AP_PROJECT_ENTITYMODEL_H
#define UABA2_AP_PROJECT_ENTITYMODEL_H

#include "../data_structures/Hitbox.h"
#include "../data_structures/Raycast.h"
#include "../data_structures/Vector2f.h"
#include "../observer_pattern_interface/ISubject.h"
#include "../utils/Camera.h"
#include "../utils/InputMap.h"
#include <algorithm>
#include <set>
#include <utility>
#include <vector>

class InputMap;
class Hitbox;
class Raycast;

namespace Core {
class EntityModel : public ISubject
{
protected:
        Vector2f _position;
        float _rotation;
        Vector2f _scale;

        Vector2f _force;
        float _mass;
        Vector2f _velocity;
        Vector2f _max_velocity;
        Vector2f _acceleration;
        Vector2f _drag;

        std::shared_ptr<Hitbox> _hitbox;
        bool _is_static;

        std::vector<std::shared_ptr<Raycast>> _raycasts;

        std::shared_ptr<Core::Camera> _camera;
        Vector2f _view_size;

        std::shared_ptr<InputMap> _input_map;

public:
        EntityModel(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position,
                    const Core::Vector2f& view_size);

        virtual void update(double t, float dt);

        void setInputMap(const std::shared_ptr<InputMap>& input_map);

        virtual Vector2f getPosition() const;

        virtual Core::Vector2f getRepresentationPosition() const;

        virtual void setPosition(const Vector2f& position);

        virtual void move(const Vector2f& vector);

        virtual float getRotation() const;

        virtual void setRotation(float angle_radian);

        virtual void rotate(float angle_radian);

        virtual void rotate(float angle_radian, const Core::Vector2f& pivot_point);

        virtual Vector2f getScale() const;

        virtual void setScale(const Vector2f& scale);

        virtual void scale(const Vector2f& scale);

        virtual Vector2f getForce() const;

        virtual void setForce(const Vector2f& force);

        virtual float getMass() const;

        virtual void setMass(float mass);

        virtual Vector2f getVelocity() const;

        virtual void setVelocity(const Vector2f& velocity);

        virtual Vector2f getMaxVelocity() const;

        virtual void setMaxVelocity(const Vector2f& max_velocity);

        virtual Vector2f getAcceleration() const;

        virtual void setAcceleration(const Vector2f& acceleration);

        virtual Vector2f getDrag() const;

        virtual void setDrag(const Vector2f& drag);

        virtual Vector2f getViewSize() const;

        virtual Vector2f getRepresentationViewSize() const;

        virtual void setViewSize(const Vector2f& view_size);

        virtual std::shared_ptr<Hitbox> getHitbox() const;

        virtual std::shared_ptr<Hitbox> getRepresentationHitbox() const;

        virtual bool getStatic() const;

        virtual void setStatic(bool is_static);

        virtual std::shared_ptr<Raycast> getRaycast(unsigned int raycast_id) const;

        std::vector<std::shared_ptr<Raycast>> getRaycasts() const;

        std::vector<std::shared_ptr<Raycast>> getRepresentationRaycasts() const;

        virtual unsigned int addRaycast(const std::shared_ptr<Core::Raycast>& raycast);
};
} // namespace Core

#endif // UABA2_AP_PROJECT_ENTITYMODEL_H
