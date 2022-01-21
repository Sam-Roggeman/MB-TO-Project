#include "Utils.h"

float CoreUtils::lerp(float x0, float x1, float alpha) { return (1 - alpha) * x0 + alpha * x1; }