//#include <SDL2/SDL.h>
#include </opt/homebrew/include/SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

// Constants
const int SCREEN_WIDTH = 1960;
const int SCREEN_HEIGHT = 1080;
const int PLAYER_WIDTH = 50;
const int PLAYER_HEIGHT = 50;
const int GRAVITY = 1;
const int JUMP_FORCE = 20;
const int MOVE_SPEED = 10;
const int FRICTION = 2;
const int MAX_MOVESPEED = 10;
const double WALL_DEFLECT_COEFF = 0.2;
float camera_offset_y = 0.0f;
const float scroll_trigger_y = SCREEN_HEIGHT / 3.00f;
const int NUM_PLATFORMS = 7;

// SDL Global Variables
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// Player properties
struct Player {
    int x, y;
    int velocityX, velocityY;
    bool onGround;
    bool hitGround;

    Player() : x(100), y(SCREEN_HEIGHT - PLAYER_HEIGHT - 450), velocityX(0), velocityY(0), onGround(false), hitGround(true) {}
};

// Platform Properties
struct Platform {
    int x, y, width, height;
    int velocityX;
    bool isWall;

    Platform() : x(0), y(0), width(0), height(0), velocityX(0), isWall(true) {}

    Platform(int x, int y, int width, int height, int velocityX, bool isWall)
        : x(x), y(y), width(width), height(height), velocityX(velocityX), isWall(isWall) {}
};

// Dynamically create and destroy platforms
class PlatformManager {
public:
    std::vector<Platform> platforms;

    int getPlatformCount() const {
        return static_cast<int>(platforms.size());
    }

    void addPlatform(int x, int y, int width, int height, int velocityX, bool isWall) {
        platforms.emplace_back(x, y, width, height, velocityX, isWall);
    }

    void addScrollPlatforms(int numPlatforms, float camera_offset_y) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<float> x_dist(0.0f, SCREEN_WIDTH - 200.0f);
        std::uniform_real_distribution<float> y_dist(SCREEN_HEIGHT - 500.0f, SCREEN_HEIGHT - 450.0f);
        std::uniform_real_distribution<float> width_dist(100.0f, 120.0f);
        std::uniform_real_distribution<float> height_dist(10.0f, 20.0f);
        std::uniform_real_distribution<float> velocityX_dist(-2.0f, 2.0f);

        for (int i = 0; i < numPlatforms; i++){
            float x = x_dist(gen);
            float y = y_dist(gen);
            float width = width_dist(gen);
            float height = height_dist(gen);
            float velocityX = velocityX_dist(gen);

            addPlatform(x, y + camera_offset_y, width, height, velocityX, false);
        }
    }

    void removeOffscreenPlatform(float camera_offset_y) {
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

    void clearAllPlatforms() {
        platforms.clear();
    }

    // move platforms
    void updatePlatformVelocity() {
        for (auto& platform : platforms) {
            platform.x += platform.velocityX;

            if (platform.x <= 0 || platform.x + platform.width >= SCREEN_WIDTH) {
                platform.velocityX = -platform.velocityX;
            }
        }
    }

    void updatePlatforms(float camera_offset_y) {
        removeOffscreenPlatform(camera_offset_y);
        int platformnum = getPlatformCount();
        //std::cout << platformnum << std::endl;
        if (platformnum < NUM_PLATFORMS) {
            addScrollPlatforms(NUM_PLATFORMS - platformnum, camera_offset_y);
        }
    }

};

// Game Objects
Player player;
PlatformManager platformManager;

// define initial platforms
vector<Platform> initial_platforms = {
    {0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 20, 0, false}, // Ground
    {0, SCREEN_HEIGHT -400 -50 +20, 20, 400, 0, true}, // Left Wall
    {SCREEN_WIDTH -20, SCREEN_HEIGHT - 400 -50 +20, 20, 400, 0, true}, // Right Wall
    {200, SCREEN_HEIGHT - 150, 200, 20, 2, false}, // Floating Platform
    {500, SCREEN_HEIGHT - 250, 200, 20, -3, false}, // Floating Platform
    {500, SCREEN_HEIGHT - 350, 150, 20, -5, false}  // Floating Platform
};

// Initialize SDL
bool initSDL() {
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
    return true;
}

// Close SDL
void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Apply Gravity and Handle Collision
void applyPhysics() {

    // update y position
    player.velocityY += GRAVITY;
    player.y += player.velocityY;

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
    player.x += player.velocityX;
    player.onGround = false; 
    
    // Player Platform interactions
    for (auto& platform : platformManager.platforms) {
        // check if the player hits a platform vertically
        if (player.y + PLAYER_HEIGHT > platform.y && player.y + PLAYER_HEIGHT < platform.y + platform.height && 
                player.x + PLAYER_WIDTH > platform.x && player.x < platform.x + platform.width && 
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

// Render Game Objects
void renderGame() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // scroll screen
    if (player.y - camera_offset_y < scroll_trigger_y) {
       camera_offset_y = player.y - scroll_trigger_y;
    }

    // draw platforms
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (auto& platform : platformManager.platforms) {
        float screen_y = platform.y - camera_offset_y;
        SDL_Rect rect = {
            static_cast<int>(platform.x),
            static_cast<int>(screen_y),
            static_cast<int>(platform.width),
            static_cast<int>(platform.height)
        };
        SDL_RenderFillRect(renderer, &rect);
    }

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

    SDL_RenderPresent(renderer);
}

// Function declaration
void handleInput(SDL_Event& event, bool& running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_a) player.velocityX = -MOVE_SPEED;  // Move Left
            if (event.key.keysym.sym == SDLK_d) player.velocityX = MOVE_SPEED;  // Move Right
            if (event.key.keysym.sym == SDLK_s) player.velocityX = 0;  // Stop
            if (event.key.keysym.sym == SDLK_SPACE && player.onGround) {
                player.velocityY = -JUMP_FORCE;  // Jump
                player.onGround = false;
                player.hitGround = true;
            }
        }
    }
}

// Game Loop
void gameLoop() {
    bool running = true;
    SDL_Event event;

    // create initial platforms
    for (const auto& p : initial_platforms) {
        platformManager.addPlatform(p.x, p.y, p.width, p.height, p.velocityX, p.isWall);
    }
    
    // loop
    while (running) {
        handleInput(event, running);
        applyPhysics();
        //platformManager.removeOffscreenPlatform(camera_offset_y);
        platformManager.updatePlatforms(camera_offset_y);
        platformManager.updatePlatformVelocity();
        renderGame();
        SDL_Delay(16);
    }
}

// Main function
int main() {
    if (!initSDL()) return -1;

    gameLoop();

    closeSDL();
    return 0;
}
