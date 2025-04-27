#include "TextRenderer.h"

TextRenderer::TextRenderer(SDL_Renderer* renderer)
    : renderer(renderer)
{
    // Mở font và kiểm tra lỗi
    font = TTF_OpenFont("arialbd.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
    }
}

TextRenderer::~TextRenderer() {
    // Đảm bảo font được đóng khi đối tượng bị hủy
    if (font) {
        TTF_CloseFont(font);
    }
}

void TextRenderer::drawText(const std::string& text, int x, int y, SDL_Color color) {
    // Kiểm tra font trước khi vẽ
    if (!font) {
        return;
    }

    // Tạo bề mặt văn bản
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        return;
    }

    // Tạo texture từ bề mặt
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    // Tính toán vị trí căn giữa
    int centerX = x - (surface->w / 2);
    int centerY = y - (surface->h / 2);

    // Tạo và vẽ rectangle cho văn bản tại vị trí đã căn giữa
    SDL_Rect dst = { centerX, centerY, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    // Giải phóng tài nguyên
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

