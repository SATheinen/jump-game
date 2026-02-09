#include <SDL.h>
#include <iostream>
#include <SDL_ttf.h>
#include "TextWindow.h"

TextWindow::TextWindow(SDL_Renderer* renderer, const std::string& text,
                        const std::string& fontPath, int fontSize, int x, int y, bool center)
    : renderer(renderer), text(text), x(x), y(y), center(center)
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

    SDL_Color color = {0, 0, 0, 192};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        std::cerr << "Text render error: " << TTF_GetError() << std::endl;
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Get text size
    int textW = 0, textH = 0;
    TTF_SizeText(font, text.c_str(), &textW, &textH);

    // Get screen size
    int screenW = 0, screenH = 0;
    SDL_GetRendererOutputSize(renderer, &screenW, &screenH);

    // Center the text
    dstRect.w = textW;
    dstRect.h = textH;
    dstRect.x = (screenW - textW) / 2;
    dstRect.y = (screenH - textH) / 2;

    if (center) {
        dstRect.x = (screenW - textW) / 2;
        dstRect.y = (screenH - textH) / 2;
    } else {
        dstRect.x = x;
        dstRect.y = y;
    }
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