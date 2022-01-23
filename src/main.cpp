#include "game_representation/Game.h"

int srltest(int argc, char* argv[]);
int gridtest(int argc, char* argv[]);

#include <iostream>

int main(int argc, char* argv[])
{
        //        srltest(argc, argv);
        Representation::Game game;
        game.run();
        return 0;
}