#!/bin/bash

# Add include/ to header search path
CPLUS_INCLUDE_PATH="${PWD}/include/:$CPLUS_INCLUDE_PATH"
export CPLUS_INCLUDE_PATH

# Compile ALL .cpp files in src/
g++ ./src/*.cpp -Iinclude -o jump \
    $(/opt/homebrew/bin/sdl2-config --cflags --libs) \
    -std=c++17 -lSDL2_ttf

# Run the game
./jump
