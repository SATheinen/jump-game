#pragma once
#include </opt/homebrew/include/SDL2/SDL.h>
#include <iostream>
#include <SDL_ttf.h>

class TextWindow {
public:
    TextWindow(SDL_Renderer* renderer, const std::string& text,
        const std::string& fontPath, int fontSize, int x, int y);
    
    ~TextWindow();  

    void setText(const std::string& newText);
    void render() const;
    
private:
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    SDL_Texture* texture = nullptr;
    std::string text;
    SDL_Rect dstRect{};
    int x, y;

    void updateTexture();
};