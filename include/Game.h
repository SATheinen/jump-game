#pragma once
#include "Player.h"
#include "PlatformManager.h"
#include "textWindow.h"

// Game Loop
class Game {
public:
    Game();
    ~Game();

    bool init();
    void run();
    void clean();
    void updateScore(float camera_offset_y);

private:
    void render();
    void update();

    // SDL Global Variables
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Game Objects
    PlatformManager platformManager;
    std::unique_ptr<TextWindow> scoreText;
    std::unique_ptr<TextWindow> gameOverText;

    bool running = true;
    int score = 0;
};