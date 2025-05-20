#ifndef IMAGE_H
#define IMAGE_H

#include <SDL3_image/SDL_image.h>

struct Image
{
    SDL_FRect rect;
    SDL_Texture *texture;
    SDL_Renderer *renderer;
};

struct Image *NewImageFromFile(SDL_Renderer *renderer, const char *path);
void DrawImage(struct Image *image, int x, int y);
bool ImageIsColliding(struct Image *image, int x, int y);
void FreeImage(struct Image *);

#endif