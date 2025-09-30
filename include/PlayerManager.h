#pragma once
#include "Player.h"
#include "PlatformManager.h"   // for platform collisions
#include </opt/homebrew/include/SDL2/SDL.h>

class PlayerManager {
public:
    PlayerManager();

    Player& getPlayer();
    void playerPhysics(PlatformManager& platformManager);
    void handleInput(SDL_Event& event, bool& running);
    float scrollCamera(float camera_offset_y);
    void render(SDL_Renderer* renderer, float camera_offset_y);

private:
    Player player;
};