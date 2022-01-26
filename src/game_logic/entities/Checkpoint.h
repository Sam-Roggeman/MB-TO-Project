#ifndef RACINGSIMULATOR_CHECKPOINT_H
#define RACINGSIMULATOR_CHECKPOINT_H

#include "../CoreConstants.h"
#include "EntityModel.h"

namespace Core {
class Checkpoint : public EntityModel
{
private:
        static unsigned int _id_count;
        unsigned int _id;
        bool _show_raycast;

public:
        Checkpoint(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position,
                   const Core::Vector2f& view_size, const Vector2f& raycast_direction,
                   float raycast_length);
        ~Checkpoint() = default;

        unsigned int getID() const;

        void update(double t, float dt) override;

        void onHit() override;

        void setShowRaycast(bool show_raycast);
        bool getShowRaycast() const;
};
} // namespace Core
#endif // RACINGSIMULATOR_CHECKPOINT_H
