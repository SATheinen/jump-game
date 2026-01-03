#pragma once
#include <SDL.h>
#include <vector>
#include <Platform.h>

// forward constants
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int NUM_PLATFORMS;

// Dynamically create and destroy platforms
class PlatformManager {
private:
    std::vector<Platform> platforms;
public:
    int getPlatformCount();
    const std::vector<Platform>& getPlatforms() const;
    void addPlatform(int x, int y, int width, int height, int velocityX, bool isWall);
    void addScrollPlatforms(int numPlatforms, float camera_offset_y);
    void removeOffscreenPlatform(float camera_offset_y);
    void clearAllPlatforms();
    // move platforms
    void updatePlatformVelocity();
    // Create new platforms
    void updatePlatforms(float camera_offset_y);
    void render(SDL_Renderer* renderer, float camera_offset_y);
};
