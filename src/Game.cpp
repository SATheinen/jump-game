#include <SDL.h>
#include <iostream>
#include <SDL_ttf.h>
#include "Game.h"
#include "PlayerManager.h"
#include "PlatformManager.h"
#include "Config.h"

using namespace std;

Game::Game(bool headless) : headless(headless) {}

Game::~Game() {
    clean();
}

bool Game::init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize! Error: " << SDL_GetError() << endl;
        return false;
    }

    if (!headless) {
        window = SDL_CreateWindow("Jump & Run Game",
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window) {
            cout << "Window could not be created! Error: " << SDL_GetError() << endl;
            return false;
        }
    }
    else {
        window = nullptr;
    }

    if (!headless) {
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
    }
    else {
        renderer = nullptr;
    }

    if (!headless) {
        // Create Score and Game Over textboxes
        scoreText = std::make_unique<TextWindow>(renderer, "Score: 0", "assets/arial.ttf", 40, 50, 0);
        gameOverText = std::make_unique<TextWindow>(renderer, "Game Over!", "assets/arial.ttf", 100, 0, 0, true);
    }

    platformManager.createInitialPlatforms();

    running = true;

    return true;
}

// score function
void Game::updateScore(float camera_offset_y) {
    score = static_cast<int>(-camera_offset_y * 100 / SCREEN_HEIGHT);
}

GameState Game::getState() const {
    GameState state;
    
    // 1. Player data extrahieren
    const Player& p = playerManager.getPlayer();
    state.player_x = static_cast<float>(p.x);
    state.player_y = static_cast<float>(p.y);
    state.player_vx = static_cast<float>(p.velocityX);
    state.player_vy = static_cast<float>(p.velocityY);
    state.player_on_ground = p.onGround;
    
    // 2. Platform data extrahieren
    const std::vector<Platform>& platforms = platformManager.getPlatforms();
    
    // Fülle die ersten 7 Platforms (oder weniger, wenn weniger existieren)
    for (int i = 0; i < NUM_PLATFORMS; i++) {
        if (i < platforms.size()) {
            state.platforms[i][0] = static_cast<float>(platforms[i].x);
            state.platforms[i][1] = static_cast<float>(platforms[i].y);
            state.platforms[i][2] = static_cast<float>(platforms[i].width);
            state.platforms[i][3] = static_cast<float>(platforms[i].height);
            state.platforms[i][4] = static_cast<float>(platforms[i].velocityX);
            state.platforms[i][5] = static_cast<float>(platforms[i].isWall);
        } else {
            // Falls weniger als 7 Platforms: Mit 0en füllen
            state.platforms[i][0] = 0.0f;
            state.platforms[i][1] = 0.0f;
            state.platforms[i][2] = 0.0f;
            state.platforms[i][3] = 0.0f;
            state.platforms[i][4] = 0.0f;
            state.platforms[i][5] = 0.0f;
        }
    }
    
    // 3. Score
    state.score = score;
    
    return state;
}

// loop
void Game::run() {
    SDL_Event event;
    while (running) {
        playerManager.handleInput(event, running, inputState);
        playerManager.updatePlayer(inputState);
        update();
        render();
        SDL_Delay(16);
    }
}

GameState Game::step(bool left, bool right, bool jump, int num_frames) {

    inputState.left = left;
    inputState.right = right;
    inputState.jump = jump;

    float deltaTime = 0.016f;

    for (int i = 0; i < num_frames; i++) {
        playerManager.updatePlayer(inputState);
        update();

        if (playerManager.isGameOver(camera_offset_y)) {
            break;
        }
    }

    return getState();
}

void Game::update() {
    playerManager.playerPhysics(platformManager);
    platformManager.updatePlatforms(camera_offset_y);
    platformManager.updatePlatformVelocity();

    updateScore(camera_offset_y);
    if (!headless) {
        scoreText->setText("Score: " + std::to_string(score));
        if (playerManager.isGameOver(camera_offset_y)) {
            gameOverText->setText("Game Over!");
        }
    }
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

    if (playerManager.isGameOver(camera_offset_y)) {
        gameOverText->render();
    }

    SDL_RenderPresent(renderer);
}

void Game::reset() {
    running = true;
    score = 0;
    
    // track camera movement
    camera_offset_y = 0.0f;

    playerManager.reset();
    platformManager.reset();

    //gameOverText->reset();
}

void Game::clean() {
    // Close SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
