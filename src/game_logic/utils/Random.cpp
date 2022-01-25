#include "Random.h"

Random::Random()
{
        std::random_device rd;        // Will be used to obtain a seed for the random number engine
        gen = new std::mt19937(rd()); // Standard mersenne_twister_engine seeded with rd()
        dis = new std::uniform_real_distribution<float>(0, 1);
}

Random::~Random()
{
        delete gen;
        delete dis;
}

Random& Random::getInstance()
{
        static Random instance;
        return instance;
}

float Random::getRandomNumber() { return (*dis)(*gen); }
