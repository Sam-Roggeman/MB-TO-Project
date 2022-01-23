//
// Created by Sam Roggeman on 1/17/2022.
//
#include "imageProcessor.h"
#include <iostream>

using namespace cv;
int gridtest(int argc, char* argv[])
{

        std::string image_path = samples::findFile("Inputfiles/Untitled.png");
        Mat img = imread(image_path, IMREAD_COLOR);
        //        if(img.empty())
        //        {
        //                std::cout << "Could not read the image: " << image_path << std::endl;
        //                return 1;
        //        }
        //        imshow("Display window", img);
        //        int k = waitKey(0); // Wait for a keystroke in the window
        //        if(k == 's')
        //        {
        //                imwrite("starry_night.png", img);
        //        }
        imageProcessor g{img};
        std::cout << g;
        return 0;
}