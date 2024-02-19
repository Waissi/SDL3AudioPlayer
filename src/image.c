#include "image.h"

struct Image *NewImageFromFile(SDL_Renderer *renderer, const char *path)
{
    int width, height;
    struct Image *newImage = (struct Image *)SDL_malloc(sizeof(struct Image));
    newImage->renderer = renderer;
    newImage->texture = IMG_LoadTexture(renderer, path);
    if (!newImage->texture)
    {
        SDL_Log("Could not create texture for Image %s", SDL_GetError());
        return NULL;
    }
    SDL_QueryTexture(newImage->texture, NULL, NULL, &width, &height);
    newImage->rect.x = 0;
    newImage->rect.y = 0;
    newImage->rect.w = width;
    newImage->rect.h = height;
    return newImage;
}

void DrawImage(struct Image *image, int x, int y)
{
    image->rect.x = x - image->rect.w / 2;
    image->rect.y = y - image->rect.h / 2;
    SDL_RenderTexture(image->renderer, image->texture, NULL, &image->rect);
}

SDL_bool ImageIsColliding(struct Image *image, int x, int y)
{
    return x > image->rect.x && x < image->rect.x + image->rect.w && y > image->rect.y && y < image->rect.y + image->rect.h;
}

void FreeImage(struct Image *image)
{
    SDL_DestroyTexture(image->texture);
    SDL_free(image);
}