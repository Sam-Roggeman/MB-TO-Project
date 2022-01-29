#include "Stopwatch.h"

namespace Core {
Stopwatch& Stopwatch::getInstance()
{
        static Stopwatch instance;
        return instance;
}

Stopwatch::Stopwatch()
    : _a(std::chrono::steady_clock::now()), _b(std::chrono::steady_clock::now()), _fps_limit(60), _cap_framerate(true),
      _delta_time(0), _physics_delta_time(1.0f / 120), _physics_time(0), _accumulator(0), _physics_speed(1),
      _sample_duration(500), _duration(0), _frame_count(0), _average_fps(0)
{
        _ms_limit = (1000.f / static_cast<float>(_fps_limit));
}

void Stopwatch::sleep_frame()
{
        _a = std::chrono::steady_clock::now();
        std::chrono::duration<float> work_time = _a - _b;

        if (_cap_framerate & (work_time.count() * 1e3 < _ms_limit)) {
                std::chrono::duration<float> delta_us((_ms_limit / 1e3) - work_time.count());

                std::this_thread::sleep_for(
                    std::chrono::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(delta_us)));
        }

        _b = std::chrono::steady_clock::now();
        std::chrono::duration<float> sleep_time = _b - _a;

        // frame counter
        _frame_count++;
        _delta_time = (work_time + sleep_time).count() * 1e3f;
        _duration += _delta_time;

        if (_duration >= _sample_duration) {
                _average_fps = _frame_count / _duration * 1e3f;

                _frame_count = 0;
                _duration = 0;
        }
}

float Stopwatch::getDeltaTime() const { return _delta_time * 1e-3f; }

float Stopwatch::getPhysicsDeltaTime() const { return _physics_delta_time; }

void Stopwatch::setPhysicsDeltaTime(float dt) { _physics_delta_time = dt; }

double Stopwatch::getPhysicsTime() const { return _physics_time; }

float Stopwatch::getPhysicsSpeed() const { return _physics_speed; }

void Stopwatch::setPhysicsSpeed(float physics_speed) { _physics_speed = physics_speed; }

void Stopwatch::PhysicsUpdate(const std::function<void(double, float)>& func)
{
        _accumulator += _delta_time * 1e-3f * _physics_speed;
        while (_accumulator >= _physics_delta_time) {
                func(_physics_time, _physics_delta_time);

                _physics_time += _physics_delta_time;
                _accumulator -= _physics_delta_time;
        }
}

float Stopwatch::computeAlpha() const { return _accumulator / _physics_delta_time; }

bool Stopwatch::getCapFramerate() const { return _cap_framerate; }

void Stopwatch::setCapFramerate(bool cap_fps) { _cap_framerate = cap_fps; }

unsigned int Stopwatch::getFpsLimit() const { return _fps_limit; }

void Stopwatch::setFpsLimit(unsigned int fps_limit)
{
        _fps_limit = fps_limit;
        _ms_limit = (1e3f / static_cast<float>(_fps_limit));
}

float Stopwatch::getAverageFps() const { return _average_fps; }
} // namespace Core