#include "Checkpoint.h"

unsigned int Core::Checkpoint::_id_count = 0;

Core::Checkpoint::Checkpoint(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position,
                             const Core::Vector2f& view_size, const Vector2f& raycast_direction, float raycast_length)
    : EntityModel(std::move(camera), position, view_size), _show_raycast(true)
{
        _id = _id_count;
        _id_count++;
        std::shared_ptr<Core::Raycast> new_raycast = std::make_shared<Core::Raycast>(position, raycast_direction, raycast_length);
        addRaycast(new_raycast);
}

unsigned int Core::Checkpoint::getID() const
{ return _id; }

void Core::Checkpoint::update(double t, float dt) { notifyObservers(); }

void Core::Checkpoint::onHit() { EntityModel::onHit(); }

bool Core::Checkpoint::getShowRaycast() const { return _show_raycast; }

void Core::Checkpoint::setShowRaycast(bool show_raycast) { _show_raycast = show_raycast; }