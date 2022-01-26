#include "game_representation/Game.h"

int srltest(int argc, char* argv[]);
int gridtest(int argc, char* argv[]);

#include <iostream>

int main(int argc, char* argv[])
{
        //        srltest(argc, argv);
        ios::sync_with_stdio(false);
        Representation::Game game;
        game.run();
        return 0;
}