#include "TextRenderer.h"

TextRenderer::TextRenderer(SDL_Renderer* renderer)
    : renderer(renderer)
{

    font = TTF_OpenFont("arialbd.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
    }
}

TextRenderer::~TextRenderer() {

    if (font) {
        TTF_CloseFont(font);
    }
}

void TextRenderer::drawText(const std::string& text, int x, int y, SDL_Color color) {
    if (!font) {
        return;
    }


    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    int centerX = x - (surface->w / 2);
    int centerY = y - (surface->h / 2);

    SDL_Rect dst = { centerX, centerY, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int TextRenderer::getTextWidth(const std::string& text) {
    if (!font) return 0;

    int text_width = 0;
    int text_height = 0;
    if (TTF_SizeText(font, text.c_str(), &text_width, &text_height) != 0) {
        SDL_Log("TTF_SizeText failed: %s", TTF_GetError());
    }

    return text_width;
}

