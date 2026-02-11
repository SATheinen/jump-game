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
    float platforms[NUM_PLATFORMS][4];  // [x, y, width, velocityX]
    
    // Meta information
    bool done;
    int score;
};