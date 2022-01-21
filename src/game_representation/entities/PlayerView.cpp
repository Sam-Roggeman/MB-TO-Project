#include "PlayerView.h"

Representation::PlayerView::PlayerView(std::weak_ptr<Core::EntityModel> entity_model)
    : EntityView(std::move(entity_model))
{
}

void Representation::PlayerView::handleEvent(const std::string& event)
{
        if (event == "left") {
                direction = true;
        } else if (event == "right") {
                direction = false;
        } else if (event == "jump") {
                jumping = true;
        } else if (event == "stand") {
                jumping = false;
        }

        // right jumping
        if (!direction & jumping) {
                setTexture(0);
        }
        // left jumping
        else if (direction & jumping) {
                setTexture(1);
        }
        // right standing
        else if (!direction & !jumping) {
                setTexture(2);
        }
        // left standing
        else {
                setTexture(3);
        }

        EntityView::handleEvent(event);
}

void Representation::PlayerView::setTexture(unsigned int texture_id)
{
        EntityView::setTexture(texture_id);

        if (direction) {
                _sprite.setOrigin(static_cast<float>(_sprite.getTextureRect().width) / 3 * 2,
                                  static_cast<float>(_sprite.getTextureRect().height) / 2);
        } else {
                _sprite.setOrigin(static_cast<float>(_sprite.getTextureRect().width) / 3 * 1,
                                  static_cast<float>(_sprite.getTextureRect().height) / 2);
        }
}
