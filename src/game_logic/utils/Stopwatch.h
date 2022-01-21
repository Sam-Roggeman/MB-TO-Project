#ifndef UABA2_AP_PROJECT_STOPWATCH_H
#define UABA2_AP_PROJECT_STOPWATCH_H

#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <thread>

namespace Core {
/**
 * Stopwatch class
 */
class Stopwatch
{
        std::chrono::steady_clock::time_point _a;
        std::chrono::steady_clock::time_point _b;
        unsigned int _fps_limit;
        float _ms_limit;
        bool _cap_framerate;

        float _physics_time;
        float _delta_time;
        float _physics_delta_time;
        float _accumulator;

        float _sample_duration;
        float _frame_count;
        float _duration;
        float _average_fps;

        Stopwatch();

public:
        static Stopwatch& getInstance();

        Stopwatch(Stopwatch const&) = delete;

        void operator=(Stopwatch const&) = delete;

        void sleep_frame();

        float getDeltaTime() const;

        float getPhysicsDeltaTime() const;

        float getPhysicsTime() const;

        void PhysicsUpdate(const std::function<void(float, float)>& func);

        float computeAlpha() const;

        bool getCapFramerate() const;

        void setCapFramerate(bool cap_fps);

        unsigned int getFpsLimit() const;

        void setFpsLimit(unsigned int fps_limit);

        float getAverageFps() const;
};
} // namespace Core
#endif // UABA2_AP_PROJECT_STOPWATCH_H
