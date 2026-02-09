#include "Player.h"

Player::Player()
    : x(100),
      y(SCREEN_HEIGHT - PLAYER_HEIGHT - 450),
      velocityX(0),
      velocityY(0),
      onGround(false),
      hitGround(true) {}

void Player::reset() {
  x = 100;
  y = SCREEN_HEIGHT - PLAYER_HEIGHT - 450;
  velocityX = 0;
  velocityY = 0;
  onGround = false;
  hitGround = true;
}
