#ifndef UABA2_AP_PROJECT_UTILS_H
#define UABA2_AP_PROJECT_UTILS_H

#include <cmath>

namespace CoreUtils {
float lerp(float x0, float x1, float alpha);
float toRadian(float angle);
float toDegree(float angle);
void fDegreeMod(float& angle);
} // namespace CoreUtils
#endif // UABA2_AP_PROJECT_UTILS_H
