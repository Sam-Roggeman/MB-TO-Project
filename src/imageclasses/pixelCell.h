//
// Created by Sam Roggeman on 1/15/2022.
//

#ifndef RACINGSIMULATOR_PIXELCELL_H
#define RACINGSIMULATOR_PIXELCELL_H
#include "Color.h"

enum celltype
{
        WallCell,
        RoadCell,
        AirCell
};

class pixelCell
{
protected:
        celltype type = WallCell;
        Color c{};

public:
        virtual ~pixelCell() = default;
        pixelCell(celltype type, const Color& c);
        pixelCell(celltype type, Color&& c);
        virtual std::string toString() const = 0;
        friend std::ostream& operator<<(std::ostream& out, const pixelCell& cell)
        {
                out << cell.toString();
                return out;
        }
};

class Wall : public pixelCell
{
public:
        virtual ~Wall() override = default;
        std::string toString() const override { return "w"; };
        Wall();
};
class Air : public pixelCell
{
public:
        virtual ~Air() override = default;
        std::string toString() const override { return " "; };
        Air() : pixelCell(AirCell, Color()){};
};
class Road : public pixelCell
{
public:
        virtual ~Road() override = default;
        std::string toString() const override { return "R"; };
        Road();
};

#endif // RACINGSIMULATOR_PIXELCELL_H
