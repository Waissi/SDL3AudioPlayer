#ifndef CLIP_PANEL_H
#define CLIP_PANEL_H

#include "audioPlayer.h"
#include "color.h"
#include "button.h"
#include "text.h"

struct ClipPanel
{
    struct Button *playButton;
    struct Button *pauseButton;
    struct Button *stopButton;
    struct Button *panLeftButton;
    struct Button *panRightButton;
    struct Button *volDownButton;
    struct Button *volUpButton;
    struct Text *titleText;
    struct Text *panText;
    struct Text *volumeText;
    struct AudioClip *clip;
    float clipVolume;
    float clipPan;
    SDL_FRect *rect;
    SDL_Renderer *renderer;
};

struct ClipPanel *NewClipPanel(SDL_Renderer *renderer, struct AudioClip *clip, TTF_Font *font, const char *title, SDL_Rect rect);
SDL_bool CheckMousePositionOnClipPanel(struct ClipPanel *panel, int mouseX, int mouseY);
SDL_bool CheckMouseClickOnClipPanel(struct ClipPanel *panel);
void DrawClipPanel(struct ClipPanel *panel);
void FreeClipPanel(struct ClipPanel *panel);

#endif