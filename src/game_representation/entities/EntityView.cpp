#include "EntityView.h"

Representation::EntityView::EntityView(std::weak_ptr<Core::EntityModel> entity_model)
    : _entity_model(std::move(entity_model)), _current_texture(0)
{
}

void Representation::EntityView::handleEvent()
{
        std::shared_ptr<Core::EntityModel> entity_model_shared = _entity_model.lock();
        if (entity_model_shared) {
                Core::Vector2f new_position = entity_model_shared->getRepresentationPosition();
                _sprite.setPosition(new_position.x, new_position.y);

                _sprite.setRotation(entity_model_shared->getRotation());

                if (_current_texture + 1 > _textures.size())
                        return;

                // todo getRepresentationViewSize() does not give the correct size when rotated
                float x_scale_factor = entity_model_shared->getRepresentationViewSize().x /
                                       static_cast<float>(_textures[_current_texture].getSize().x) *
                                       entity_model_shared->getScale().x;

                float y_scale_factor = entity_model_shared->getRepresentationViewSize().y /
                                       static_cast<float>(_textures[_current_texture].getSize().y) *
                                       entity_model_shared->getScale().y;

                _sprite.setScale(x_scale_factor, y_scale_factor);
        }
}

void Representation::EntityView::handleEvent(const std::string& event) {}

sf::Sprite Representation::EntityView::getSprite() const { return _sprite; }

sf::VertexArray Representation::EntityView::getHitbox() const
{
        sf::VertexArray hitbox_draw;

        std::shared_ptr<Core::EntityModel> entity_model_shared = _entity_model.lock();
        if (entity_model_shared) {
                std::shared_ptr<Core::Hitbox> hitbox = entity_model_shared->getRepresentationHitbox();

                // polygon
                if (hitbox->getRadius().empty() && !hitbox->getPoints().empty()) {
                        hitbox_draw = sf::VertexArray(sf::LineStrip, hitbox->getPoints().size() + 1);

                        for (int i = 0; i < hitbox->getPoints().size(); i++) {
                                hitbox_draw[i].position =
                                    sf::Vector2f(hitbox->getPoints()[i].x, hitbox->getPoints()[i].y);

                                hitbox_draw[i].color = sf::Color::Magenta;
                        }

                        hitbox_draw[hitbox->getPoints().size()] = hitbox_draw[0];
                }
                // circle
                else if (hitbox->getRadius().x == hitbox->getRadius().y) {
                        unsigned int circle_points = RepresentationConstants::circle_hitbox_points;

                        hitbox_draw = sf::VertexArray(sf::LineStrip, circle_points + 1);

                        float angle_step = (2 * static_cast<float>(M_PI)) / static_cast<float>(circle_points);
                        float current_angle = 0;

                        for (int i = 0; i < circle_points; i++) {
                                hitbox_draw[i].position = sf::Vector2f(
                                    hitbox->getOrigin().x + hitbox->getRadius().x * std::cos(current_angle),
                                    hitbox->getOrigin().y + hitbox->getRadius().x * std::sin(current_angle));

                                hitbox_draw[i].color = sf::Color::Magenta;

                                current_angle += angle_step;
                        }

                        hitbox_draw[circle_points] = hitbox_draw[0];
                }
                // ellipse
                else {
                        // todo
                }
        }

        return hitbox_draw;
}

sf::VertexArray Representation::EntityView::getRaycasts() const
{
        sf::VertexArray lines;

        std::shared_ptr<Core::EntityModel> entity_model_shared = _entity_model.lock();
        if (entity_model_shared) {
                std::vector<std::shared_ptr<Core::Raycast>> raycasts = entity_model_shared->getRepresentationRaycasts();
                lines = sf::VertexArray(sf::Lines, raycasts.size() * 2);

                int i = 0;
                for (const auto& raycast : raycasts) {
                        lines[i].position = {raycast->getOrigin().x, raycast->getOrigin().y};
                        lines[i+1].position = {raycast->getEndpoint().x, raycast->getEndpoint().y};

                        lines[i].color = sf::Color::Yellow;
                        lines[i+1].color = sf::Color::Yellow;

                        i += 2;
                }
        }

        return lines;
}

unsigned int Representation::EntityView::addTexture(const std::string& file_path)
{
        sf::Texture new_texture;
        if (!new_texture.loadFromFile(file_path)) {
                std::cerr << "Couldn't load the texture!" << std::endl;
                return 0;
        }

        _textures.push_back(new_texture);

        return _textures.size() - 1;
}

void Representation::EntityView::setTexture(unsigned int texture_id)
{
        _current_texture = texture_id;
        if (texture_id + 1 > _textures.size())
                return;
        _sprite.setTexture(_textures[texture_id]);

        _sprite.setOrigin(static_cast<float>(_sprite.getTextureRect().width) / 2,
                          static_cast<float>(_sprite.getTextureRect().height) / 2);
}
