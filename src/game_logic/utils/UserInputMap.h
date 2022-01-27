#ifndef RACINGSIMULATOR_USERINPUTMAP_H
#define RACINGSIMULATOR_USERINPUTMAP_H

#include "../data_structures/Vector2f.h"

namespace Core {
struct UserInputMap
{
        bool a{false};
        bool b{false};
        bool c{false};
        bool d{false};
        bool e{false};
        bool f{false};
        bool g{false};
        bool h{false};
        bool i{false};
        bool j{false};
        bool k{false};
        bool l{false};
        bool m{false};
        bool n{false};
        bool o{false};
        bool p{false};
        bool q{false};
        bool r{false};
        bool s{false};
        bool t{false};
        bool u{false};
        bool v{false};
        bool w{false};
        bool x{false};
        bool y{false};
        bool z{false};

        Vector2f mouse_pos_world;
        Vector2f mouse_pos_representation_window;

        bool mouse_button_left{false};
        bool mouse_button_right{false};
        bool mouse_button_middle{false};

        float mouse_wheel_delta{0};
};
} // namespace Core

#endif // RACINGSIMULATOR_USERINPUTMAP_H
