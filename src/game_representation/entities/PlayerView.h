#ifndef UABA2_AP_PROJECT_PLAYERVIEW_H
#define UABA2_AP_PROJECT_PLAYERVIEW_H

#include "EntityView.h"
#include <utility>

namespace Representation {
class PlayerView : public EntityView
{
private:
        // false = right
        // true = left
        bool _direction;
        // true = in the air
        bool _jumping;

public:
        PlayerView(std::weak_ptr<Core::EntityModel> entity_model);

        void handleEvent(const std::string &event) override;

        void setTexture(unsigned int texture_id) override;
};
} // namespace Representation
#endif // UABA2_AP_PROJECT_PLAYERVIEW_H
