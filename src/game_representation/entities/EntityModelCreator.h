#ifndef UABA2_AP_PROJECT_ENTITYMODELCREATOR_H
#define UABA2_AP_PROJECT_ENTITYMODELCREATOR_H

#include "../../game_logic/entities/IEntityModelCreator.h"
#include "EntityView.h"
#include <memory>
#include <utility>
#include <vector>

namespace Representation {
class EntityModelCreator : public Core::IEntityModelCreator
{
private:
        std::vector<std::weak_ptr<Representation::EntityView>> _wall_views;
        std::vector<std::weak_ptr<Representation::EntityView>> _car_views;
        std::vector<std::weak_ptr<Representation::EntityView>> _player_views;

public:
        EntityModelCreator();

        std::shared_ptr<Core::Doodle> createDoodleModel(std::shared_ptr<Core::Camera> camera,
                                                        const Core::Vector2f& position,
                                                        const Core::Vector2f& view_size) override;

        std::shared_ptr<Core::Car> createCarModel(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position,
                                                  const Core::Vector2f& view_size) override;

        std::shared_ptr<Core::Wall> createWallModel(std::shared_ptr<Core::Camera> camera,
                                                    const Core::Vector2f& position,
                                                    const Core::Vector2f& view_size) override;

        std::vector<std::shared_ptr<Representation::EntityView>> getEntityViews();

private:
        std::vector<std::shared_ptr<Representation::EntityView>> getEntityViews(
            std::vector<std::weak_ptr<Representation::EntityView>>& entity_views);
};
} // namespace Representation

#endif // UABA2_AP_PROJECT_ENTITYMODELCREATOR_H
