#include </opt/homebrew/include/SDL2/SDL.h>
#include <iostream>
#include <SDL_ttf.h>
#include "Game.h"
#include "PlayerManager.h"
#include "PlatformManager.h"
#include "Config.h"

using namespace std;

// track camera movement
float camera_offset_y = 0.0f;
int score = 0;

// define initial platforms
vector<Platform> initial_platforms = {
    {0, SCREEN_HEIGHT -50, SCREEN_WIDTH, 20, 0, false}, // Ground
    //{0, SCREEN_HEIGHT -400 -50 +20, 20, 400, 0, true}, // Left Wall
    //{SCREEN_WIDTH -20, SCREEN_HEIGHT -400 -50 +20, 20, 400, 0, true}, // Right Wall
    {200, SCREEN_HEIGHT -300, 200, 20, 2, false}, // Floating Platform
    {500, SCREEN_HEIGHT -450, 200, 20, -3, false}, // Floating Platform
    {500, SCREEN_HEIGHT -600, 150, 20, -5, false},  // Floating Platform
    {200, SCREEN_HEIGHT -750, 150, 20, -5, false},  // Floating Platform
    {300, SCREEN_HEIGHT -900, 150, 20, -5, false}  // Floating Platform
};

Game::Game() {}

Game::~Game() {
    clean();
}

PlayerManager playerManager;   // create player manager

bool Game::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! Error: " << SDL_GetError() << endl;
        return false;
    }
    window = SDL_CreateWindow("Jump & Run Game",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Window could not be created! Error: " << SDL_GetError() << endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Renderer could not be created! Error: " << SDL_GetError() << endl;
        return false;
    }
    // Initialise TTF
    if (TTF_Init() == -1) {
        std::cout << "TTF_Init: " << TTF_GetError() << std::endl;
        return false;
    }
    // Get TTF font
    TTF_Font* font = TTF_OpenFont("assets/arial.ttf", 200);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        return false;
    }

    // Create Score and Game Over textboxes
    scoreText = std::make_unique<TextWindow>(renderer, "Score: 0", "assets/arial.ttf", 40, 50, 0);
    gameOverText = std::make_unique<TextWindow>(renderer, "Game Over!", "assets/arial.ttf", 64, 200, 250);

    // create initial platforms
    for (const auto& p : initial_platforms) {
        platformManager.addPlatform(p.x, p.y, p.width, p.height, p.velocityX, p.isWall);
    }

    running = true;

    return true;
}

// score function
void Game::updateScore(float camera_offset_y) {
    score = static_cast<int>(-camera_offset_y);
}

// loop
void Game::run() {
    SDL_Event event;
    while (running) {
        playerManager.handleInput(event, running);
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::update() {
    playerManager.playerPhysics(platformManager);
    platformManager.updatePlatforms(camera_offset_y);
    platformManager.updatePlatformVelocity();

    updateScore(camera_offset_y);
    scoreText->setText("Score: " + std::to_string(score));
}

// Render Game Objects
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
    SDL_RenderClear(renderer);

    // scroll screen
    camera_offset_y = playerManager.scrollCamera(camera_offset_y);

    scoreText->render();
    platformManager.render(renderer, camera_offset_y);
    playerManager.render(renderer, camera_offset_y);

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    // Close SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}