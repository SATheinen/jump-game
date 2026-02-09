#include "PlayerManager.h"
#include "Config.h"
#include <iostream>

using namespace std;

PlayerManager::PlayerManager() {}

Player& PlayerManager::getPlayer() {
    return player;
}

const Player& PlayerManager::getPlayer() const {
    return player;
}

// Apply Gravity and Handle Collision
void PlayerManager::playerPhysics(PlatformManager& platformManager) {

    // update y position
    player.velocityY += GRAVITY;
    player.y += static_cast<int>(player.velocityY);

    // cap movement speed
    if (player.velocityX > MAX_MOVESPEED) {
        player.velocityX = MAX_MOVESPEED;
    }
    else {
        if (player.velocityX < -MAX_MOVESPEED) {
            player.velocityX = -MAX_MOVESPEED;
        }
    }

    // update player position
    player.x += static_cast<int>(player.velocityX);
    player.onGround = false; 
    
    // Player Platform interactions
    for (auto& platform : platformManager.getPlatforms()) {
        // check if the player hits a platform vertically
        if (player.y + PLAYER_HEIGHT > platform.y &&
            player.y < platform.y + platform.height && 
            player.x + PLAYER_WIDTH > platform.x &&
            player.x < platform.x + platform.width && 
            platform.height < PLAYER_HEIGHT) {

            player.y = platform.y - PLAYER_HEIGHT; // Place player on top
            
            // apply velocity x of platform to player who just arrives on platform
            if (player.hitGround == true) {
                player.velocityX += platform.velocityX;
                player.hitGround = false;
            }

            // apply friction
            if (player.velocityX - platform.velocityX > 0) {
                player.velocityX -= FRICTION;
            }
            else {
                if (player.velocityX - platform.velocityX < 0) {
                player.velocityX += FRICTION;
                }
            } 

            player.onGround = true;
            player.velocityY = 0.0;
        }
        
        /// check if the player hits a platform horizontally
        // approaching platform from right 
        if (player.x < platform.x + platform.width &&
            player.x + PLAYER_WIDTH / 2 > platform.x &&
                 player.y + PLAYER_HEIGHT > platform.y && player.y < platform.y + platform.height &&
                 platform.isWall == true &&
                 player.velocityX < 0.0) {
        
             player.x = platform.x + platform.width;
             player.velocityX =  - WALL_DEFLECT_COEFF * player.velocityX;
        }
        // approaching platform from left 
        else {
            if (player.x + PLAYER_WIDTH / 2 < platform.x + platform.width &&
                player.x + PLAYER_WIDTH / 2 > platform.x &&
                    player.y + PLAYER_HEIGHT > platform.y && player.y < platform.y + platform.height &&
                    platform.isWall == true &&
                    player.velocityX > 0.0) {
        
                player.x = platform.x - PLAYER_WIDTH;
                player.velocityX =  - WALL_DEFLECT_COEFF * player.velocityX;
                }
            }
        }

    // Prevent Player from falling out of the Platform
    if (player.y > SCREEN_HEIGHT - PLAYER_HEIGHT) {
        player.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
        player.velocityY = 0;
        player.onGround = true;
    }

    // Prevent player from leaving the screen left or right
    if (player.x < 0) {
        player.x = SCREEN_WIDTH;
        }
    else {
         if (player.x > SCREEN_WIDTH) {
             player.x = 0;
            }
      }  
}

void PlayerManager::handleInput(SDL_Event& event, bool& running, InputState& input) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_a) input.left = true;
            if (event.key.keysym.sym == SDLK_d) input.right = true;
            if (event.key.keysym.sym == SDLK_SPACE) input.jump = true;
        }
        
        if (event.type == SDL_KEYUP) {
            if (event.key.keysym.sym == SDLK_a) input.left = false;
            if (event.key.keysym.sym == SDLK_d) input.right = false;
            if (event.key.keysym.sym == SDLK_SPACE) input.jump = false;
        }
    }
}

void PlayerManager::updatePlayer(InputState& input) {
    if (input.left && !input.right) {
        player.velocityX = -MOVE_SPEED;
    }
    else if (input.right && !input.left) {
        player.velocityX = MOVE_SPEED;
    }
    else {
        player.velocityX = 0;
    }
    
    // Jump logic
    if (input.jump && player.onGround) {
        player.velocityY = -JUMP_FORCE;
        player.onGround = false;
        player.hitGround = true;
    }
}

float PlayerManager::scrollCamera(float camera_offset_y) {
    if (player.y - camera_offset_y < SCROLL_TRIGGER_Y) {
       camera_offset_y = player.y - SCROLL_TRIGGER_Y;
    }
    else {
        camera_offset_y = camera_offset_y;
    }
    return camera_offset_y;
};

bool PlayerManager::isGameOver(float camera_offset_y) {
    if (player.y > SCREEN_HEIGHT + static_cast<int>(camera_offset_y))
        gameOverFlag = true;
    return gameOverFlag;
}

void PlayerManager::render(SDL_Renderer* renderer, float camera_offset_y) {
    // draw player
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    float player_screen_y = player.y - camera_offset_y;
    SDL_Rect playerRect = {
        static_cast<int>(player.x),
        static_cast<int>(player_screen_y),
        static_cast<int>(PLAYER_WIDTH),
        static_cast<int>(PLAYER_HEIGHT)
    };
    SDL_RenderFillRect(renderer, &playerRect);
};

void PlayerManager::reset() {
    player.reset();
    gameOverFlag = false;
}