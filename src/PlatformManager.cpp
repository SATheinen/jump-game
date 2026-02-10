#include "PlatformManager.h"
#include "Platform.h"
#include <random>
#include <algorithm>
#include "Config.h"

// Platform definitions
Platform::Platform()
    : x(0), y(0), width(0), height(0), velocityX(0), isWall(true) {}

Platform::Platform(int x, int y, int width, int height, int velocityX, bool isWall)
    : x(x), y(y), width(width), height(height), velocityX(velocityX), isWall(isWall) {}

std::vector<Platform>& PlatformManager::getPlatforms() {
    return platforms;
}
    
const std::vector<Platform>& PlatformManager::getPlatforms() const {
    return platforms;
}

// PlatformManager definitions
int PlatformManager::getPlatformCount() const {
    return static_cast<int>(platforms.size());
}

void PlatformManager::addPlatform(int x, int y, int width, int height, int velocityX, bool isWall) {
    platforms.emplace_back(x, y, width, height, velocityX, isWall);
}

void PlatformManager::addScrollPlatforms(int numPlatforms, float camera_offset_y) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> x_dist(0.0f, SCREEN_WIDTH - 200.0f);
    std::uniform_real_distribution<float> y_dist(SCREEN_HEIGHT - SCROLL_TRIGGER_Y - 700.0f,
        SCREEN_HEIGHT - SCROLL_TRIGGER_Y - 650.0f);
    std::uniform_real_distribution<float> width_dist(100.0f, 120.0f);
    std::uniform_real_distribution<float> height_dist(10.0f, 20.0f);
    std::uniform_real_distribution<float> velocityX_dist(-PLATFORM_MAX_SPEED, PLATFORM_MAX_SPEED);

    for (int i = 0; i < numPlatforms; i++){
        float x = x_dist(gen);
        float y = y_dist(gen);
        float width = width_dist(gen);
        float height = height_dist(gen);
        float velocityX = velocityX_dist(gen);

        addPlatform(x, y + camera_offset_y, width, height, velocityX, false);
    }
}

void PlatformManager::removeOffscreenPlatform(float camera_offset_y) {
platforms.erase(
    std::remove_if(platforms.begin(), platforms.end(),
        [camera_offset_y](const Platform& p) {
            return p.x + p.width < 0 || p.x > SCREEN_WIDTH ||
                    (p.y - camera_offset_y + p.height < 0) ||
                    (p.y - camera_offset_y > SCREEN_HEIGHT);
            }),
        platforms.end()
    );
}

void PlatformManager::clearAllPlatforms() {
    platforms.clear();
}

// move platforms
void PlatformManager::updatePlatformVelocity() {
    for (auto& platform : platforms) {
        platform.x += static_cast<int>(platform.velocityX);

        if (platform.x <= 0 || platform.x + platform.width >= SCREEN_WIDTH) {
            platform.velocityX = -static_cast<int>(platform.velocityX);
        }
    }
}

void PlatformManager::updatePlatforms(float camera_offset_y) {
    removeOffscreenPlatform(camera_offset_y);
    int platformnum = getPlatformCount();
    if (platformnum < NUM_PLATFORMS) {
        addScrollPlatforms(NUM_PLATFORMS - platformnum, camera_offset_y);
    }
}

void PlatformManager::render(SDL_Renderer* renderer, float camera_offset_y) {
    // draw platforms
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (auto& platform : platforms) {
        float screen_y = platform.y - camera_offset_y;
        SDL_Rect rect = {
            static_cast<int>(platform.x),
            static_cast<int>(screen_y),
            static_cast<int>(platform.width),
            static_cast<int>(platform.height)
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}

void PlatformManager::createInitialPlatforms() {
    clearAllPlatforms();

    // define initial platforms
    std::vector<Platform> initial_platforms = {
        {0, SCREEN_HEIGHT -50, SCREEN_WIDTH, 20, 0, false}, // Ground
        //{0, SCREEN_HEIGHT -400 -50 +20, 20, 400, 0, true}, // Left Wall
        //{SCREEN_WIDTH -20, SCREEN_HEIGHT -400 -50 +20, 20, 400, 0, true}, // Right Wall
        {200, SCREEN_HEIGHT -300, 200, 20, 2, false}, // Floating Platform
        {500, SCREEN_HEIGHT -450, 200, 20, -3, false}, // Floating Platform
        {500, SCREEN_HEIGHT -600, 150, 20, -5, false},  // Floating Platform
        {200, SCREEN_HEIGHT -750, 150, 20, -5, false},  // Floating Platform
        {300, SCREEN_HEIGHT -900, 150, 20, -5, false}  // Floating Platform
    };

    // create initial platforms
    for (const auto& p : initial_platforms) {
        addPlatform(p.x, p.y, p.width, p.height, p.velocityX, p.isWall);
    }
}

void PlatformManager::reset() {
    createInitialPlatforms();
}