#pragma once
#include "Player.h"
#include "PlatformManager.h"
#include "PlayerManager.h"
#include "textWindow.h"
#include "InputState.h"
#include "GameState.h"
#include "Config.h"

// Game Loop
class Game {
public:
    Game(bool headless = false);
    ~Game();

    bool init();
    void run();
    void step();
    void clean();
    void reset(); // call PlayerManager and PlatformManager reset functions
    void updateScore(float camera_offset_y);

    GameState step(bool left, bool right, bool jump, int num_frames = 4);
    GameState getState() const;

private:
    void render();
    void update();

    // SDL Global Variables
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Game Objects
    PlatformManager platformManager;
    PlayerManager playerManager;
    InputState inputState;
    std::unique_ptr<TextWindow> scoreText;
    std::unique_ptr<TextWindow> gameOverText;

    bool headless;
    bool running = true;
    int score = 0;

    // track camera movement
    float camera_offset_y = 0.0f;
};
