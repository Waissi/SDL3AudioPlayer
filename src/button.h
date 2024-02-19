#ifndef BUTTON_H
#define BUTTON_H

#include "image.h"

enum ButtonState
{
    HOOVER,
    IDLE
};

struct Button
{
    int x;
    int y;
    int width;
    int height;
    struct Image *image;
    enum ButtonState state;
};

struct Button *NewButton(SDL_Renderer *renderer, const char *path);
void SetButtonPosition(struct Button *button, int x, int y);
SDL_bool CheckMouseClickOnButon(struct Button *button);
SDL_bool CheckMousePositionOnButton(struct Button *button, float mouseX, float mouseY);
void DrawButton(struct Button *button);
void FreeButton(struct Button *button);

#endif