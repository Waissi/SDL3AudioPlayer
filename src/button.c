#include "button.h"

struct Button *NewButton(SDL_Renderer *renderer, const char *path)
{
    struct Image *image = NewImageFromFile(renderer, path);
    if (!image)
    {
        return NULL;
    }
    struct Button *newButton = (struct Button *)SDL_malloc(sizeof(struct Button));
    newButton->image = image;
    newButton->width = newButton->image->rect.w;
    newButton->height = newButton->image->rect.h;
    newButton->x = 0;
    newButton->y = 0;
    newButton->state = IDLE;
    return newButton;
}

void SetButtonPosition(struct Button *button, int x, int y)
{
    button->x = x;
    button->y = y;
}

bool CheckMouseClickOnButon(struct Button *button)
{
    return button->state == HOOVER;
}

bool CheckMousePositionOnButton(struct Button *button, float mouseX, float mouseY)
{
    bool isHoovering = ImageIsColliding(button->image, mouseX, mouseY);
    button->state = isHoovering ? HOOVER : IDLE;
    return isHoovering;
}

void DrawButton(struct Button *button)
{
    DrawImage(button->image, button->x, button->y);
}

void FreeButton(struct Button *button)
{
    FreeImage(button->image);
    SDL_free(button);
}