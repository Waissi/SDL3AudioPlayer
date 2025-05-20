#ifndef TEXT_H
#define TEXT_H

#include <string.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

struct Text
{
    int x;
    int y;
    SDL_Texture *texture;
    SDL_FRect rect;
    SDL_Renderer *renderer;
};

struct Text *NewText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color);
void SetTextPosition(struct Text *text, int x, int y);
void DrawText(struct Text *text);
void FreeText(struct Text *text);

#endif