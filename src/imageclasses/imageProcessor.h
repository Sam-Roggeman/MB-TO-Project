#ifndef RACINGSIMULATOR_IMAGEPROCESSOR_H
#define RACINGSIMULATOR_IMAGEPROCESSOR_H
#include "../game_logic/entities/Wall.h"
#include "memory"
#include "pixelCell.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

class imageProcessor : public std::vector<std::vector<std::shared_ptr<pixelCell>>>
{
        std::shared_ptr<pixelCell> roadcell = std::make_shared<Road>();
        std::shared_ptr<pixelCell> aircell = std::make_shared<Air>();
        std::shared_ptr<pixelCell> wallcell = std::make_shared<Wall>();

public:
        explicit imageProcessor(const cv::Mat& input)
            : std::vector<std::vector<std::shared_ptr<pixelCell>>>(
                  input.rows, std::vector<std::shared_ptr<pixelCell>>(input.cols, nullptr))
        {
                Color pixel{};
                cv::Vec3b color{};
                for (unsigned int row = 0; row != size(); row++) {
                        for (unsigned int col = 0; col != at(row).size(); col++) {
                                color = input.at<cv::Vec3b>(cv::Point(col, row));
                                pixel = Color(color.val[2], color.val[1], color.val[0]);
                                if (pixel.isGrey()) {
                                        this->at(row).at(col) = roadcell;
                                } else if (pixel.isBrown()) {
                                        this->at(row).at(col) = wallcell;
                                } else {
                                        this->at(row).at(col) = aircell;
                                }
                        }
                }
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
