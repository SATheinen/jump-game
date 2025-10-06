//#include <SDL2/SDL.h>
#include </opt/homebrew/include/SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <random>
#include <SDL_ttf.h>
#include "Game.h"

// Main function
int main() {
    Game game;
    if (!game.init()) return -1;
    game.run();
    game.clean();
    return 0;
}