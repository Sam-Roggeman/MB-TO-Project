#ifndef RACINGSIMULATOR_GROUNDTILE_H
#define RACINGSIMULATOR_GROUNDTILE_H

#include "../CoreConstants.h"
#include "EntityModel.h"

namespace Core {
class GroundTile : public EntityModel
{
public:
        GroundTile(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position,
                   const Core::Vector2f& view_size);

        void update(double t, float dt) override;
};
} // namespace Core
#endif // RACINGSIMULATOR_GROUNDTILE_H
