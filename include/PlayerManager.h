#pragma once
#include "Player.h"
#include "PlatformManager.h"   // for platform collisions
#include "InputState.h"
#include <SDL.h>

class PlayerManager {
public:
    PlayerManager();

    Player& getPlayer();
    const Player& getPlayer() const;
    void playerPhysics(PlatformManager& platformManager);
    void handleInput(SDL_Event& event, bool& running, InputState& input);
    void updatePlayer(InputState& input);
    float scrollCamera(float camera_offset_y);
    bool isGameOver(float camera_offset_y) const;
    void render(SDL_Renderer* renderer, float camera_offset_y);
    void reset(); // reset player und gameOverFlag

    bool gameOverFlag = false;
private:
    Player player;
};
