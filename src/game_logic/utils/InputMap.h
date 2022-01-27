#ifndef UABA2_AP_PROJECT_INPUTMAP_H
#define UABA2_AP_PROJECT_INPUTMAP_H

namespace Core {
struct InputMap
{
        float up{0};
        float down{0};
        float left{0};
        float right{0};

        bool reset{false};

        bool custom1{false};
        bool custom2{false};
        bool custom3{false};
        bool custom4{false};
        bool custom5{false};

        Vector2f mouse_pos;
};
} // namespace Core
#endif // UABA2_AP_PROJECT_INPUTMAP_H
