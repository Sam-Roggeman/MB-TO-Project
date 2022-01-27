#ifndef UABA2_AP_PROJECT_CORECONSTANTS_H
#define UABA2_AP_PROJECT_CORECONSTANTS_H

#include <limits>
#include "data_structures/Vector2f.h"

namespace CoreConstants {
// camera
extern const float world_x_min;
extern const float world_x_max;
extern const float world_y_min;
extern const float world_y_max;

extern const float camera_view_x_min;
extern const float camera_view_x_max;
extern const float camera_view_y_min;
extern const float camera_view_y_max;

// player
extern const bool generate_player;

// entities
extern const Core::Vector2f spawn_location;
extern const Core::Vector2f spawn_direction;

// AI
extern const float time_limit;
} // namespace CoreConstants

#endif // UABA2_AP_PROJECT_CORECONSTANTS_H
