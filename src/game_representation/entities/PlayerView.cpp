#include "PlayerView.h"

Representation::PlayerView::PlayerView(std::weak_ptr<Core::EntityModel> entity_model)
    : EntityView(std::move(entity_model)), _direction(true), _jumping(true)
{
}

void Representation::PlayerView::handleEvent(const std::string& event)
{
        if (event == "left") {
                _direction = true;
        } else if (event == "right") {
                _direction = false;
        } else if (event == "jump") {
                _jumping = true;
        } else if (event == "stand") {
                _jumping = false;
        }

        // right _jumping
        if (!_direction & _jumping) {
                setTexture(0);
        }
        // left _jumping
        else if (_direction & _jumping) {
                setTexture(1);
        }
        // right standing
        else if (!_direction & !_jumping) {
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

        if (_direction) {
                _sprite.setOrigin(static_cast<float>(_sprite.getTextureRect().width) / 3 * 2,
                                  static_cast<float>(_sprite.getTextureRect().height) / 2);
        } else {
                _sprite.setOrigin(static_cast<float>(_sprite.getTextureRect().width) / 3 * 1,
                                  static_cast<float>(_sprite.getTextureRect().height) / 2);
        }
}
