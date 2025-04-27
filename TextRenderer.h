#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class TextRenderer {
public:
    TextRenderer(SDL_Renderer* renderer);
    ~TextRenderer();

    void drawText(const std::string& text, int x, int y, SDL_Color color);
    int getTextWidth(const std::string& text); // <-- Thêm dòng này tại đây

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
};

#endif

