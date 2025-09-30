#include </opt/homebrew/include/SDL2/SDL.h>
#include <iostream>
#include <SDL_ttf.h>
#include "TextWindow.h"

TextWindow::TextWindow(SDL_Renderer* renderer, const std::string& text,
                        const std::string& fontPath, int fontSize, int x, int y)
    : renderer(renderer), text(text), x(x), y(y)
{
    // Create font
    font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }
    updateTexture();
}

TextWindow::~TextWindow() {
    if (texture) SDL_DestroyTexture(texture);
    if (font) TTF_CloseFont(font);
}   

void TextWindow::updateTexture() {
    if (texture) SDL_DestroyTexture(texture);

    // r g b alpha
    SDL_Color color = {0, 0, 0, 192};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Text render error: " << TTF_GetError() << std::endl;
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    dstRect.x = x;
    dstRect.y = y;
    SDL_QueryTexture(texture, nullptr, nullptr, &dstRect.w, &dstRect.h);
}

void TextWindow::setText(const std::string& newText) {
    text = newText;
    updateTexture();
}

void TextWindow::render() const {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    }
}