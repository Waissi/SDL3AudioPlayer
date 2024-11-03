#include "clipPanel.h"

struct ClipPanel *NewClipPanel(SDL_Renderer *renderer, struct AudioClip *clip, TTF_Font *font, const char *title, SDL_Rect rect)
{
    struct ClipPanel *newClipPanel = SDL_malloc(sizeof(struct ClipPanel));
    newClipPanel->renderer = renderer;
    newClipPanel->rect = SDL_malloc(sizeof(SDL_FRect));
    newClipPanel->rect->x = rect.x;
    newClipPanel->rect->y = rect.y;
    newClipPanel->rect->w = rect.w;
    newClipPanel->rect->h = rect.h;
    newClipPanel->clip = clip;
    newClipPanel->clipVolume = 1.0f;
    newClipPanel->clipPan = 0.0f;

    newClipPanel->playButton = NewButton(renderer, "assets/images/play.png");
    newClipPanel->pauseButton = NewButton(renderer, "assets/images/pause.png");
    newClipPanel->stopButton = NewButton(renderer, "assets/images/stop.png");
    newClipPanel->panLeftButton = NewButton(renderer, "assets/images/minus.png");
    newClipPanel->panRightButton = NewButton(renderer, "assets/images/plus.png");
    newClipPanel->volDownButton = NewButton(renderer, "assets/images/minus.png");
    newClipPanel->volUpButton = NewButton(renderer, "assets/images/plus.png");

    newClipPanel->titleText = NewText(renderer, font, title, BLUE);
    newClipPanel->panText = NewText(renderer, font, "PAN.", BLACK);
    newClipPanel->volumeText = NewText(renderer, font, "VOL.", BLACK);

    int x, y;
    int panelW = rect.x + rect.w;
    int panelH = rect.y + rect.h;
    int panelHalfW = rect.x + rect.w / 2;
    int panelHalfH = rect.y + rect.h / 2;
    SetTextPosition(newClipPanel->titleText, panelHalfW, rect.y + newClipPanel->titleText->height / 2);

    x = rect.x + newClipPanel->panLeftButton->width / 2;
    y = panelHalfH - rect.h / 8;
    SetButtonPosition(newClipPanel->panLeftButton, x, y);
    SetTextPosition(newClipPanel->panText, panelHalfW, y);
    SetButtonPosition(newClipPanel->panRightButton, panelW - newClipPanel->panRightButton->width / 2, y);

    y = panelHalfH + rect.h / 8;
    SetButtonPosition(newClipPanel->volDownButton, x, y);
    SetTextPosition(newClipPanel->volumeText, panelHalfW, y);
    SetButtonPosition(newClipPanel->volUpButton, panelW - newClipPanel->volUpButton->width / 2, y);

    SetButtonPosition(newClipPanel->pauseButton, panelHalfW - newClipPanel->pauseButton->width, panelH - newClipPanel->pauseButton->height / 2);
    SetButtonPosition(newClipPanel->playButton, panelHalfW, panelH - newClipPanel->playButton->height / 2);
    SetButtonPosition(newClipPanel->stopButton, panelHalfW + newClipPanel->stopButton->width, panelH - newClipPanel->stopButton->height / 2);

    return newClipPanel;
}

SDL_bool CheckMousePositionOnClipPanel(struct ClipPanel *panel, int mouseX, int mouseY)
{
    SDL_bool isHoovering = SDL_FALSE;
    if (CheckMousePositionOnButton(panel->playButton, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    if (CheckMousePositionOnButton(panel->pauseButton, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    if (CheckMousePositionOnButton(panel->stopButton, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    if (CheckMousePositionOnButton(panel->panLeftButton, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    if (CheckMousePositionOnButton(panel->panRightButton, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    if (CheckMousePositionOnButton(panel->volDownButton, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    if (CheckMousePositionOnButton(panel->volUpButton, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    return isHoovering;
}

SDL_bool CheckMouseClickOnClipPanel(struct ClipPanel *panel)
{
    if (CheckMouseClickOnButon(panel->playButton) == SDL_TRUE)
    {
        AudioPlayer_PlayAudioClip(panel->clip);
        return SDL_TRUE;
    }
    else if (CheckMouseClickOnButon(panel->pauseButton) == SDL_TRUE)
    {
        if (AudioPlayer_AudioClipIsPlaying(panel->clip))
        {
            AudioPlayer_PauseAudioClip(panel->clip);
            return SDL_TRUE;
        }
        AudioPlayer_ResumeAudioClip(panel->clip);
        return SDL_TRUE;
    }
    else if (CheckMouseClickOnButon(panel->stopButton) == SDL_TRUE)
    {
        AudioPlayer_StopAudioClip(panel->clip);
        return SDL_TRUE;
    }
    else if (CheckMouseClickOnButon(panel->panLeftButton) == SDL_TRUE)
    {
        panel->clipPan -= 0.1f;
        panel->clipPan = SDL_clamp(panel->clipPan, -1.0f, 1.0f);
        AudioPlayer_SetAudioClipPan(panel->clip, panel->clipPan);
        return SDL_TRUE;
    }
    else if (CheckMouseClickOnButon(panel->panRightButton) == SDL_TRUE)
    {
        panel->clipPan += 0.1f;
        panel->clipPan = SDL_clamp(panel->clipPan, -1.0f, 1.0f);
        AudioPlayer_SetAudioClipPan(panel->clip, panel->clipPan);
        return SDL_TRUE;
    }
    else if (CheckMouseClickOnButon(panel->volDownButton) == SDL_TRUE)
    {
        panel->clipVolume -= 0.1f;
        panel->clipVolume = SDL_clamp(panel->clipVolume, 0.0f, 1.0f);
        AudioPlayer_SetAudioClipVolume(panel->clip, panel->clipVolume);
        return SDL_TRUE;
    }
    else if (CheckMouseClickOnButon(panel->volUpButton) == SDL_TRUE)
    {
        panel->clipVolume += 0.1f;
        panel->clipVolume = SDL_clamp(panel->clipVolume, 0.0f, 1.0f);
        AudioPlayer_SetAudioClipVolume(panel->clip, panel->clipVolume);
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

void DrawClipPanel(struct ClipPanel *panel)
{
    SDL_SetRenderDrawColor(panel->renderer, 10, 128, 80, 255);
    SDL_RenderRect(panel->renderer, panel->rect);
    DrawButton(panel->playButton);
    DrawButton(panel->pauseButton);
    DrawButton(panel->stopButton);
    DrawButton(panel->panLeftButton);
    DrawButton(panel->panRightButton);
    DrawButton(panel->volDownButton);
    DrawButton(panel->volUpButton);
    DrawText(panel->titleText);
    DrawText(panel->panText);
    DrawText(panel->volumeText);
}

void FreeClipPanel(struct ClipPanel *panel)
{
    FreeButton(panel->playButton);
    FreeButton(panel->pauseButton);
    FreeButton(panel->stopButton);
    FreeButton(panel->panLeftButton);
    FreeButton(panel->panRightButton);
    FreeButton(panel->volDownButton);
    FreeButton(panel->volUpButton);
    FreeText(panel->titleText);
    FreeText(panel->panText);
    FreeText(panel->volumeText);
    SDL_free(panel->rect);
    SDL_free(panel);
}