#ifndef RACINGSIMULATOR_IMAGEPROCESSOR_H
#define RACINGSIMULATOR_IMAGEPROCESSOR_H
#include "../game_logic/data_structures/Vector2f.h"
#include "memory"
#include "pixelCell.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <utility>

using pixelPtr = std::shared_ptr<pixelCell>;
using checkPtr = std::shared_ptr<CheckPoint>;

class imageProcessor : public std::vector<std::vector<pixelPtr>>
{
        pixelPtr roadcell = std::make_shared<Road>();
        pixelPtr aircell = std::make_shared<Air>();
        pixelPtr wallcell = std::make_shared<Wall>();
        pixelPtr startCell = std::make_shared<Start>();
        pixelPtr directionCell = std::make_shared<Direction>();
        pixelPtr endLineCell = std::make_shared<Finish>();
        pixelPtr checkPointCell = std::make_shared<CheckPoint>();
        std::vector<std::shared_ptr<CheckPoint>> checkpoints{};

        Core::Vector2f endline_v1{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()},
            endline_v2{}, start_point{}, direction{};
        std::vector<std::pair<Core::Vector2f, Core::Vector2f>> checkPoint_vector{};

        void processImage()
        {
                Core::Vector2f lowest_start{0.0f, (float)this->size()}, highest_direction{0, (float)this->size()},
                    highest_start{}, lowest_direction{};
                pixelPtr _pix{};
                checkPtr _checkPtr{};
                std::vector<std::pair<unsigned int, unsigned int>> checkpoints_points{};
                int x_diff{}, y_diff{};
                double distance{}, min_distance{};
                std::pair<unsigned int, unsigned int> checkpoint_point1{}, checkpoint_point2{}, checkpoint_point3{},
                    checkpoint_point4{};

                for (unsigned int row_ind = 0; row_ind < size(); row_ind++) {
                        for (unsigned int col_ind = 0; col_ind < at(row_ind).size(); col_ind++) {
                                _pix = at(row_ind).at(col_ind);
                                if (_pix == startCell) {
                                        if (lowest_start.y > (float)row_ind)
                                                lowest_start = {(float)col_ind, (float)row_ind};
                                        if (highest_start.y < (float)row_ind)
                                                highest_start = {(float)col_ind, (float)row_ind};
                                } else if (_pix == endLineCell) {
                                        if (endline_v1.y > (float)row_ind)
                                                endline_v1 = {(float)col_ind, (float)row_ind};
                                        if (endline_v2.y < (float)row_ind)
                                                endline_v2 = {(float)col_ind, (float)row_ind};
                                } else if (_pix == checkPointCell) {
                                        _checkPtr = std::make_shared<CheckPoint>(row_ind, col_ind, checkpoints.size());
                                        checkpoints.emplace_back(_checkPtr);
                                        foundNewCheckPoint(_checkPtr, row_ind, col_ind);
                                }
                        }
                }
                start_point = (lowest_start + highest_start) / 2;

                for (unsigned int row_ind = 0; row_ind < size(); row_ind++) {
                        for (unsigned int col_ind = 0; col_ind < at(row_ind).size(); col_ind++) {
                                _pix = at(row_ind).at(col_ind);
                                if (_pix == directionCell) {
                                        if ((float)row_ind > lowest_direction.y)
                                                lowest_direction = {(float)col_ind, (float)row_ind};
                                        if ((float)row_ind < highest_direction.y)
                                                highest_direction = {(float)col_ind, (float)row_ind};
                                }
                        }
                }
                direction = (lowest_direction + highest_direction) / 2 - start_point;
                for (const checkPtr& _checkptr : checkpoints) {
                        checkpoint_point1 = _checkptr->getP1();
                        checkpoints_points = {_checkptr->getP2(), _checkptr->getP3(), _checkptr->getP4()};
                        y_diff = (int)checkpoint_point1.second - (int)checkpoints_points.at(0).second;
                        x_diff = (int)checkpoint_point1.first - (int)checkpoints_points.at(0).first;
                        min_distance = std::sqrt(std::pow(x_diff, 2) + std::pow(y_diff, 2));
                        checkpoint_point2 = checkpoints_points.at(0);
                        for (const std::pair<unsigned int, unsigned int>& checkpoint_point : checkpoints_points) {
                                y_diff = (int)checkpoint_point1.second - (int)checkpoint_point.second;
                                x_diff = (int)checkpoint_point1.first - (int)checkpoint_point.first;
                                distance = std::sqrt(std::pow(x_diff, 2) + std::pow(y_diff, 2));
                                if (distance < min_distance) {
                                        min_distance = distance;
                                        checkpoint_point2 = checkpoint_point;
                                }
                        }

                        if (checkpoint_point2 == checkpoints_points.at(0)) {
                                checkpoint_point3 = checkpoints_points.at(1);
                                checkpoint_point4 = checkpoints_points.at(2);
                        } else if (checkpoint_point2 == checkpoints_points.at(1)) {
                                checkpoint_point3 = checkpoints_points.at(0);
                                checkpoint_point4 = checkpoints_points.at(2);
                        } else if (checkpoint_point2 == checkpoints_points.at(2)) {
                                checkpoint_point3 = checkpoints_points.at(0);
                                checkpoint_point4 = checkpoints_points.at(1);
                        }
                        checkPoint_vector.emplace_back(
                            std::make_pair(Core::Vector2f(checkpoint_point2.first + checkpoint_point1.first,
                                                          checkpoint_point2.second + checkpoint_point1.second) /
                                               2.0f,
                                           Core::Vector2f(checkpoint_point3.first + checkpoint_point4.first,
                                                          checkpoint_point3.second + checkpoint_point4.second) /
                                               2.0f));
                }
        }

public:
        Core::Vector2f getEndlineV1() const { return endline_v1; }
        Core::Vector2f getEndlineV2() const { return endline_v2; }
        Core::Vector2f getStartPoint() const { return start_point; }
        Core::Vector2f getDirection() const { return direction; }
        vector<std::pair<Core::Vector2f, Core::Vector2f>> getCheckPoints() const { return checkPoint_vector; }

        bool inside_bound(unsigned int row, unsigned int col)
        {
                return (row < size() && row >= 0 && col >= 0 && at(row).size() > col);
        }
        void foundNewCheckPoint(const checkPtr& _checkP, unsigned int row, unsigned int col)
        {
                bool looking_for_new_column = false;
                bool new_column_found = false;
                unsigned int new_column{};

                if (inside_bound(row, col) && at(row).at(col) == checkPointCell) {
                        at(row).at(col) = _checkP;
                        _checkP->updateExtrema(row, col);
                }
                // loop down
                for (unsigned int _working_row = row;
                     inside_bound(_working_row, col) && at(_working_row).at(col)->getType() == CheckPointCell;
                     _working_row++) {
                        if (inside_bound(_working_row + 1, col) && at(_working_row + 1).at(col) &&
                            at(_working_row + 1).at(col)->getType() != CheckPointCell) {
                                looking_for_new_column = true;
                        }
                        // go right
                        for (unsigned int _working_col = col;
                             inside_bound(_working_row, _working_col) &&
                             at(_working_row).at(_working_col)->getType() == CheckPointCell;
                             _working_col++) {
                                at(_working_row).at(_working_col) = _checkP;
                                _checkP->updateExtrema(_working_row, _working_col);
                                if (looking_for_new_column && inside_bound(_working_row + 1, _working_col) &&
                                    at(_working_row + 1).at(_working_col) &&
                                    at(_working_row + 1).at(_working_col)->getType() == CheckPointCell) {
                                        looking_for_new_column = false;
                                        new_column = _working_col;
                                        new_column_found = true;
                                }
                        }
                        // go left
                        for (unsigned int _working_col = col - 1;
                             inside_bound(_working_row, _working_col) &&
                             at(_working_row).at(_working_col)->getType() == CheckPointCell;
                             _working_col--) {
                                at(_working_row).at(_working_col) = _checkP;
                                _checkP->updateExtrema(_working_row, _working_col);
                                if (looking_for_new_column && inside_bound(_working_row + 1, _working_col) &&
                                    at(_working_row + 1).at(_working_col) &&
                                    at(_working_row + 1).at(_working_col)->getType() == CheckPointCell) {
                                        looking_for_new_column = false;
                                        new_column = _working_col;
                                        new_column_found = true;
                                }
                        }
                        if (new_column_found) {
                                new_column_found = false;
                                col = new_column;
                        }
                }
        }

public:
        explicit imageProcessor(const cv::Mat& input)
            : std::vector<std::vector<pixelPtr>>(input.rows, std::vector<pixelPtr>(input.cols, nullptr))
        {
                pixelPtr _pix{};
                Color pixel{};
                cv::Vec3b color{};

                for (unsigned int row = 0; row != size(); row++) {
                        for (unsigned int col = 0; col != at(row).size(); col++) {
                                color = input.at<cv::Vec3b>(cv::Point(col, row));
                                pixel = Color(color.val[2], color.val[1], color.val[0]);
                                if (this->at(row).at(col)) {
                                        continue;
                                }
                                if (pixel.isGrey()) {
                                        _pix = roadcell;
                                } else if (pixel.isBrown()) {
                                        _pix = wallcell;
                                } else if (pixel.isGreen()) {
                                        _pix = endLineCell;
                                } else if (pixel.isRed()) {
                                        _pix = startCell;

                                } else if (pixel.isOrange()) {
                                        _pix = directionCell;
                                } else if (pixel.isYellow()) {
                                        _pix = checkPointCell;
                                } else {
                                        _pix = aircell;
                                }
                                this->at(row).at(col) = _pix;
                        }
                }
                processImage();
        }

        explicit imageProcessor(const std::string& image_path)
            : imageProcessor(cv::imread(cv::samples::findFile(image_path), cv::IMREAD_COLOR))
        {
        }
        friend std::ostream& operator<<(std::ostream& out, const imageProcessor& grid)
        {
                for (unsigned int row = 0; row != grid.size(); row++) {
                        for (unsigned int col = 0; col != grid.at(row).size(); col++) {
                                out << grid.at(row).at(col)->toString() << ' ';
                        }
                        out << std::endl;
                }
                return out;
        }

        bool isWall(double d, unsigned int i) { return at(d).at(i) == wallcell; }

        unsigned int getRows() { return size(); }
        unsigned int getColumns() { return at(0).size(); }
};

#endif // RACINGSIMULATOR_IMAGEPROCESSOR_H
