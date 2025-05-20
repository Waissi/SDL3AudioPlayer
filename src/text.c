#include "text.h"

struct Text *NewText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, strlen(text), color);
    if (!surface)
    {
        SDL_Log("Could not create surface, %s", SDL_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_Log("Could not create texture for Image %s", SDL_GetError());
        return NULL;
    }
    struct Text *newText = (struct Text *)SDL_malloc(sizeof(struct Text));
    newText->renderer = renderer;
    newText->texture = texture;
    newText->rect.x = 0;
    newText->rect.y = 0;
    newText->rect.w = texture->w;
    newText->rect.h = texture->h;
    newText->x = 0;
    newText->y = 0;

    SDL_DestroySurface(surface);
    return newText;
}

void SetTextPosition(struct Text *text, int x, int y)
{
    text->x = x;
    text->y = y;
    text->rect.x = x - text->rect.w / 2;
    text->rect.y = y - text->rect.h / 2;
}

void DrawText(struct Text *text)
{
    SDL_RenderTexture(text->renderer, text->texture, NULL, &text->rect);
}

void FreeText(struct Text *text)
{
    SDL_DestroyTexture(text->texture);
    SDL_free(text);
}