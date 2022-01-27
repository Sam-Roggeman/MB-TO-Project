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
        AirCell,
        DirectionCell,
        StartCell,
        FinishCell,
        CheckPointCell
};

class pixelCell
{
protected:
        const celltype type;
        Color c{};

public:
        virtual ~pixelCell() = default;
        pixelCell(celltype type, const Color& c);
        celltype getType() const;
        pixelCell(celltype type, Color&& c);
        virtual std::string toString() const = 0;
        friend std::ostream& operator<<(std::ostream& out, const pixelCell& cell)
        {
                out << cell.toString();
                return out;
        }
        virtual unsigned int getId() const { return 0; }
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
class Start : public pixelCell
{
public:
        virtual ~Start() override = default;
        std::string toString() const override { return "S"; };
        Start();
};
class Direction : public pixelCell
{
public:
        virtual ~Direction() override = default;
        std::string toString() const override { return "D"; };
        Direction();
};
class Finish : public pixelCell
{
public:
        virtual ~Finish() override = default;
        std::string toString() const override { return "E"; };
        Finish();
};
class CheckPoint : public pixelCell
{
        int id;
        // leftmost
        std::pair<unsigned int, unsigned int> leftmost_point{};
        // bottem_point
        std::pair<unsigned int, unsigned int> bottem_point{};
        // rightmost
        std::pair<unsigned int, unsigned int> rightmost_point{};
        // highest_point
        std::pair<unsigned int, unsigned int> highest_point{};

public:
        virtual ~CheckPoint() override = default;
        std::string toString() const override { return "E"; };

        explicit CheckPoint(unsigned int row = 0, unsigned int col = 0, int _id = -1);
        void updateExtrema(unsigned int row, unsigned int col);
        unsigned int getId() const override;
        const std::pair<unsigned int, unsigned int>& getP1() const;
        const std::pair<unsigned int, unsigned int>& getP2() const;
        const std::pair<unsigned int, unsigned int>& getP3() const;
        const std::pair<unsigned int, unsigned int>& getP4() const;
};
#endif // RACINGSIMULATOR_PIXELCELL_H
