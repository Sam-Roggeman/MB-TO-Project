//
// Created by User on 19/02/2021.
//

#ifndef PROJECT_COLOR_H
#define PROJECT_COLOR_H
#include "string"
#include <algorithm>
#include <vector>
class Color
{
        double red;
        double green;
        double blue;

public:
        Color(double red, double green, double blue) : red(red), green(green), blue(blue){};
        explicit Color(std::vector<double> rgb)
        {
                red = rgb[0];
                green = rgb[1];
                blue = rgb[2];
        }
        Color operator*(const Color& c) const { return {red * c.red, blue * c.blue, green * c.green}; }
        Color operator*(const double d) const { return {red * d, blue * d, green * d}; }
        Color& operator+=(Color c)
        {
                red = std::min(red + c.red, 1.0);
                blue = std::min(blue + c.blue, 1.0);
                green = std::min(green + c.green, 1.0);
                return *this;
        }
        ~Color() = default;
        Color()
        {
                red = 0;
                blue = 0;
                green = 0;
        }

        bool isZero() const { return red == 0 && blue == 0 && green == 0; }
        bool isBrown() { return std::abs(red - 127) <= 35 && std::abs(green - 51) <= 35 && std::abs(blue - 0) <= 35; }
        bool isGrey() { return std::abs(red - 128) <= 50 && std::abs(green - 128) <= 50 && std::abs(blue - 128) <= 50; }
};

#endif // PROJECT_COLOR_H
