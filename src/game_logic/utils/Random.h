//
// Created by razer_blade on 20/11/2021.
//

#ifndef DOODLE_JUMP_LOGIC_RANDOM_H
#define DOODLE_JUMP_LOGIC_RANDOM_H

#include <random>

namespace Core {
class Random
{
public:
        Random(const Random&) = delete;
        static Random& _getInstance();
        static int uniformInt(int a, int b);
        static float uniformReal(float a, float b);
        static int Bernoulli(float p);
        static float Normal(float m, float n);

private:
        Random();
        int _uniformInt(int a, int b);
        float _uniformReal(float a, float b);
        int _bernoulli(float p);
        float _normal(float m, float n);
        std::random_device _r{};
        std::mt19937 _generator{};
};
} // namespace Core

#endif
