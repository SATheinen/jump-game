#pragma once
#include "Player.h"
#include "PlatformManager.h"
#include "PlayerManager.h"
#include "textWindow.h"

// Game Loop
class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void clean();
    void reset(); // call PlayerManager and PlatformManager reset functions
    void updateScore(float camera_offset_y);

private:
    void render();
    void update();

    // SDL Global Variables
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Game Objects
    PlatformManager platformManager;
    PlayerManager playerManager;
    std::unique_ptr<TextWindow> scoreText;
    std::unique_ptr<TextWindow> gameOverText;

    bool running = true;
    bool headless = false;
    int score = 0;
    
    // track camera movement
    float camera_offset_y = 0.0f;
};
