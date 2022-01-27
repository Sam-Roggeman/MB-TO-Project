//
// Created by Sam Roggeman on 1/15/2022.
//

#include "pixelCell.h"

pixelCell::pixelCell(celltype type, const Color& c) : type(type), c(c) {}
pixelCell::pixelCell(celltype type, Color&& c) : type(type), c(c) {}
celltype pixelCell::getType() const { return type; }
Wall::Wall() : pixelCell(WallCell, Color(255, 248, 220)) {}
Road::Road() : pixelCell(RoadCell, Color(220, 220, 220)) {}
Direction::Direction() : pixelCell(DirectionCell, Color(255, 167, 73)) {}
Finish::Finish() : pixelCell(FinishCell, Color(117, 255, 79)) {}
Start::Start() : pixelCell(StartCell, Color(255, 28, 43)) {}
CheckPoint::CheckPoint(unsigned int row, unsigned int col, int _id)
    : pixelCell(CheckPointCell, Color(255, 233, 127)), id(_id), rightmost_point({col, row}), bottem_point({col, row}),
      leftmost_point({col, row}), highest_point({col, row})
{
}
void CheckPoint::updateExtrema(unsigned int row, unsigned int col)
{
        if (row < highest_point.second) {
                highest_point = {col, row};
        } else if (row > bottem_point.second) {
                bottem_point = {col, row};
        }
        if (col < leftmost_point.first) {
                leftmost_point = {col, row};
        } else if (col > rightmost_point.first) {
                rightmost_point = {col, row};
        }
}
unsigned int CheckPoint::getId() const { return id; }
const std::pair<unsigned int, unsigned int>& CheckPoint::getP1() const { return leftmost_point; }
const std::pair<unsigned int, unsigned int>& CheckPoint::getP2() const { return bottem_point; }
const std::pair<unsigned int, unsigned int>& CheckPoint::getP3() const { return rightmost_point; }
const std::pair<unsigned int, unsigned int>& CheckPoint::getP4() const { return highest_point; }
