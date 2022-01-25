//
// Created by razer_blade on 20/11/2021.
//

#include "Random.h"

Core::Random::Random() : _generator(_r()) {

}
Core::Random& Core::Random::_getInstance() {
        static Core::Random instance;
        return instance;
}

int Core::Random::uniformInt(int a, int b) {
        return _getInstance()._uniformInt(a,b);
}

float Core::Random::uniformReal(float a, float b) {
        return _getInstance()._uniformReal(a,b);
}

int Core::Random::Bernoulli(float p) {
        return _getInstance()._bernoulli(p);
}

float Core::Random::Normal(float m, float n) {
        return _getInstance()._normal(m,n);
}

int Core::Random::_uniformInt(int a, int b) {
        std::uniform_int_distribution<int> d(a, b);
        return d(_generator);
}

float Core::Random::_uniformReal(float a, float b) {
        std::uniform_real_distribution<float> d(a, b);
        return d(_generator);
}

int Core::Random::_bernoulli(float p) {
        std::bernoulli_distribution d(p);
        return d(_generator);
}

float Core::Random::_normal(float m, float n) {
        std::normal_distribution<float> d(m,n);
        return d(_generator);
}
