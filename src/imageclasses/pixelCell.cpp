//
// Created by Sam Roggeman on 1/15/2022.
//

#include "pixelCell.h"
pixelCell::pixelCell(celltype type, const Color& c) : type(type), c(c) {}
pixelCell::pixelCell(celltype type, Color&& c) : type(type), c(std::move(c)) {}
Wall::Wall() : pixelCell(WallCell, Color(255, 248, 220)) {}
Road::Road() : pixelCell(RoadCell, Color(220, 220, 220)) {}
