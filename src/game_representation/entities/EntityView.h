#ifndef UABA2_AP_PROJECT_ENTITYVIEW_H
#define UABA2_AP_PROJECT_ENTITYVIEW_H

#include "../../game_logic/entities/EntityModel.h"
#include "../../game_logic/observer_pattern_interface/IObserver.h"
#include "../RepresentationConstants.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <utility>

namespace Representation {
class EntityView : public Core::IObserver
{
protected:
        std::weak_ptr<Core::EntityModel> _entity_model;
        sf::Sprite _sprite;
        std::vector<std::shared_ptr<sf::Texture>> _textures;
        unsigned int _current_texture;

public:
        EntityView(std::weak_ptr<Core::EntityModel> entity_model);

        void handleEvent() override;

        void handleEvent(const std::string& event) override;

        virtual sf::Sprite getSprite() const;

        virtual sf::VertexArray getHitbox() const;

        virtual sf::VertexArray getRaycasts() const;

        virtual unsigned int addTexture(const std::shared_ptr<sf::Texture>& texture);

        virtual void setTexture(unsigned int texture_id);
};
} // namespace Representation
#endif // UABA2_AP_PROJECT_ENTITYVIEW_H
