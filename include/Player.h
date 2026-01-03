#pragma once
#include "Config.h"
#include <SDL.h>

// Player properties
struct Player {
    int x, y;
    int velocityX, velocityY;
    bool onGround;
    bool hitGround;

    Player();
};
