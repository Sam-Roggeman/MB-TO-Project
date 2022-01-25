#ifndef SFML_TEST_RANDOM_H
#define SFML_TEST_RANDOM_H

#include <random>

/**
 * source: https://en.cppreference.com/w/cpp/numeric/random/uniform_real_distribution
 */
class Random
{
        std::mt19937* gen;
        std::uniform_real_distribution<float>* dis;

        Random();

        ~Random();

public:
        static Random& getInstance();

        Random(Random const&) = delete;

        void operator=(Random const&) = delete;

        float getRandomNumber();
};

#endif // SFML_TEST_RANDOM_H
