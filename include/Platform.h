#pragma once

struct Platform {
    int x, y, width, height;
    int velocityX;
    bool isWall;

    Platform();
    Platform(int x, int y, int width, int height, int velocityX, bool isWall);
};