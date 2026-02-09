#pragma once
#include "Config.h"

struct GameState {
    // Player state (5 values)
    float player_x;
    float player_y;
    float player_vx;
    float player_vy;
    bool player_on_ground;
    
    // Platform state
    float platforms[NUM_PLATFORMS][6];  // [x, y, width, height, velocityX, isWall]
    
    // Meta information
    int score;
};