#include "Car.h"

Core::Car::Car(std::shared_ptr<Core::Camera> camera, const Core::Vector2f& position, const Core::Vector2f& view_size)
    : EntityModel(std::move(camera), position, view_size), _brain(5, 4, 3, 2), _ai_controlled(false), _is_dead(false),
      _reached_finish(false), _total_distance_traveled(0), _time_since_checkpoint(0), _fitness(0)
{
        _direction = {0, 1};
        _mass = 1500;

        _acceleration_power = 2000;
        _reverse_acceleration_power = 1000;
        _braking_power = 6000;

        _steering_angle = CoreUtils::toRadian(45);
        _wheel_base = view_size.y * 0.9f;

        _drag = 0.015;
        _friction = 0.3;

        _max_slip_velocity = 4;
        _min_traction = 1;
        _max_traction = 0.05;

        _input_map = make_shared<Core::InputMap>();
}

void Core::Car::update(double t, float dt)
{
        // neural network
        if (_ai_controlled && !(_is_dead || _reached_finish)) {
                vector<float> raycast_lengths;
                for (auto& raycast : _raycasts) {
                        raycast_lengths.push_back(
                            (raycast->isActivated() ? raycast->getCollisionLength() : raycast->getLength()));
                }
                vector<float> neural_outputs = _brain(raycast_lengths);

                _input_map->up = 1.0;
                _input_map->down = neural_outputs[0];
                _input_map->right = neural_outputs[1];
                _input_map->left = neural_outputs[2];

        } else if (_ai_controlled) {
                _input_map->up = 0;
                _input_map->down = 0;
                _input_map->right = 0;
                _input_map->left = 0;
        }

        // clear
        if (_input_map->reset) {
                reset(CoreConstants::spawn_location, CoreConstants::spawn_direction);
        }

        // direction
        float velocity_dot_direction = _velocity.dotProduct(_direction);

        bool going_forwards = velocity_dot_direction > 0;
        float direction_sign = 1;
        if (!going_forwards) {
                direction_sign = -1;
        }

        // friction & drag
        Vector2f friction_force = _velocity * _friction;
        if (_velocity.length() < 0.1) {
                friction_force *= 3;
        }
        Vector2f drag_force = _velocity * _velocity.length() * _drag;
        _acceleration -= friction_force + drag_force;

        // steering & traction
        float alpha = _velocity.length() / _max_slip_velocity;
        alpha = 1.f - (1.f - alpha) * (1.f - alpha);
        alpha = std::clamp(alpha, 0.f, 1.f);

        float current_angle = _steering_angle * CoreUtils::lerp(_min_traction, _max_traction, alpha);

        if (_input_map->left > 0) {
                steer(current_angle * _input_map->left, direction_sign, dt);
        }
        if (_input_map->right > 0) {
                steer(-current_angle * _input_map->right, direction_sign, dt);
        }

        // acceleration
        if (_input_map->up > 0) {
                _force += _direction * _acceleration_power * _input_map->up;
                // braking
                if (!going_forwards)
                        _force += _direction * _braking_power * _input_map->up;
        }
        if (_input_map->down > 0) {
                _force -= _direction * _reverse_acceleration_power * _input_map->down;
                // braking
                if (going_forwards)
                        _force -= _direction * _braking_power * _input_map->down;
        }

        // raycasts
        resetRaycasts();
        if (hit_checkpoint) {
                _time_since_checkpoint = dt;
                hit_checkpoint = false;
        } else {
                // add time
                _time_since_checkpoint += dt;
        }

        if (!_reached_finish) {
                _total_time += dt;
        }

        Vector2f old_position;

        // calculate distance travelled
        if (!(_is_dead || _reached_finish))
                old_position = _position;

        // apply physics
        Core::EntityModel::update(t, dt);

        if (!(_is_dead || _reached_finish))
                _total_distance_traveled += (_position - old_position).length();
}

void Core::Car::onHit()
{
        setDead(true);
        _velocity.clear();
}

void Core::Car::onHitCheckpoint(unsigned int checkpoint_id)
{
        unsigned int s1 = _checkpoint_ids.size();
        _checkpoint_ids.insert(checkpoint_id);
        if (_checkpoint_ids.size() > s1) {
                hit_checkpoint = true;
        }
}

void Core::Car::reset(const Vector2f& position, const Vector2f& direction)
{
        setPosition(position);
        _velocity.clear();
        _force.clear();

        setRotation(0);
        rotate(atan2f(direction.y, direction.x) - static_cast<float>(M_PI) / 2.f);
        _direction = direction;

        _is_dead = false;
        _reached_finish = false;
        _checkpoint_ids.clear();

        _total_distance_traveled = 0;
        _time_since_checkpoint = 0;
        _fitness = 0;
        _total_time = 0;
}

void Core::Car::steer(float angle_radian, float direction_sign, float dt)
{
        Vector2f back_wheels = _position - _wheel_base / 2 * _direction;
        Vector2f front_wheels = _position + _wheel_base / 2 * _direction;

        back_wheels += dt * _velocity;
        front_wheels += dt * _velocity.rotated(angle_radian, {0, 0});

        _direction = (front_wheels - back_wheels).normalized();
        _velocity = direction_sign * _velocity.length() * _direction;

        float new_direction_rotation = atan2f(_direction.y, _direction.x) - static_cast<float>(M_PI) / 2.f;
        setRotation(new_direction_rotation);
}

void Core::Car::loadPhysicsPreset(const std::string& preset_file_path)
{
        std::map<std::string, std::string> values;
        SLR slr = SLR("assets/SLR_json/Xml.json");

        slr.ParseXML(values, preset_file_path);

        if (!values.empty()) {
                _acceleration_power = std::stof(values["acceleration_power"]);
                _reverse_acceleration_power = std::stof(values["reverse_acceleration_power"]);
                _braking_power = std::stof(values["braking_power"]);

                _steering_angle = CoreUtils::toRadian(std::stof(values["steering_angle"]));
                _wheel_base = _view_size.y * std::stof(values["wheel_base"]);

                _drag = std::stof(values["drag"]);
                _friction = std::stof(values["friction"]);

                _max_slip_velocity = std::stof(values["max_slip_velocity"]);
                _min_traction = std::stof(values["min_traction"]);
                _max_traction = std::stof(values["max_traction"]);
        } else {
                std::cerr << "Can't read xml file!" << std::endl;
        }
}

bool Core::Car::getAIControlled() const { return _ai_controlled; }
void Core::Car::setAIControlled(bool ai_controlled) { _ai_controlled = ai_controlled; }

bool Core::Car::isDead() const { return _is_dead; }
void Core::Car::setDead(bool is_dead)
{
        _is_dead = is_dead;
        if (is_dead)
                calculateFitness(false);
}

bool Core::Car::reachedFinish() const { return _reached_finish; }
void Core::Car::checkReachedFinish(unsigned int checkpoint_count)
{
        if (_checkpoint_ids.size() == checkpoint_count) {
                _reached_finish = true;
                _velocity.clear();
                calculateFitness(false);
        }
}

float Core::Car::getTotalDistanceTraveled() const { return _total_distance_traveled; }
float Core::Car::getTotalTime() const { return _time_since_checkpoint; }

void Core::Car::calculateFitness(bool overtime)
{
        if (_fitness == 0) {
                _fitness =
                    100 * (static_cast<float>(_total_checkpoint_count) - static_cast<float>(_checkpoint_ids.size())) +
                    10.0f * _total_time; //+std::min(10.0f, _time_since_checkpoint);
                if (!_reached_finish) {
                        _fitness += 1000;
                        _fitness += 10.0f * generation;

                        //_fitness += 10*((30.0-_total_time)/30.0);
                }

                //                std::cout << "time: " << _total_time << std::endl;
                //                std::cout << "checkpoints: " << _total_checkpoint_count << std::endl;
                //                std::cout << "checkpoints not triggered: " << _total_checkpoint_count -
                //                _checkpoint_ids.size() << std::endl; std::cout << "******************" << std::endl;
        }
}

float Core::Car::getFitness() const { return _fitness; }

FFNeuralNetwork& Core::Car::getBrain() { return _brain; }
void Core::Car::setBrain(FFNeuralNetwork brain)
{
        generation = 0;
        _brain = brain;
}
void Core::Car::setCheckpointCount(unsigned int count) { _total_checkpoint_count = count; }

float Core::Car::getAccelerationPower() const { return _acceleration_power; }
void Core::Car::setAccelerationPower(float value) { _acceleration_power = value; }

float Core::Car::getReverseAccelerationPower() const { return _reverse_acceleration_power; }
void Core::Car::setReverseAccelerationPower(float value) { _reverse_acceleration_power = value; }

float Core::Car::getBrakingPower() const { return _braking_power; }
void Core::Car::setBrakingPower(float value) { _braking_power = value; }

float Core::Car::getSteeringAngle() const { return _steering_angle; }
void Core::Car::setSteeringAngle(float value) { _steering_angle = value; }

float Core::Car::getWheelBase() const { return _wheel_base; }
void Core::Car::setWheelBase(float value) { _wheel_base = value; }

float Core::Car::getDrag() const { return _drag; }
void Core::Car::setDrag(float value) { _drag = value; }

float Core::Car::getFriction() const { return _friction; }
void Core::Car::setFriction(float value) { _friction = value; }

float Core::Car::getMaxSlipVelocity() const { return _max_slip_velocity; }
void Core::Car::setMaxSlipVelocity(float value) { _max_slip_velocity = value; }

float Core::Car::getMinTraction() const { return _min_traction; }
void Core::Car::setMinTraction(float value) { _min_traction = value; }

float Core::Car::getMaxTraction() const { return _max_traction; }
void Core::Car::setMaxTraction(float value) { _max_traction = value; }
void Core::Car::survived() { generation++; }
