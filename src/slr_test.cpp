//
// Created by Sam Roggeman on 21/12/2021.
//
#include "Sam/SLR.h"

int srltest(int argc, char* argv[]) {
        CFG cfg = CFG("../Inputfiles/SLR.json");
        SLR slr = SLR(cfg);
        std::cout << slr;
        slr.PrintTable();
        return 0;
}
