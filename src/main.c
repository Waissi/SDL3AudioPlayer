#include "audioPlayer.h"
#include "clipPanel.h"

const int WINDOW_W = 800;
const int WINDOW_H = 600;
float mouseX, mouseY;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event event;

struct AudioBus *musicBus = NULL;
struct AudioBus *sfxBus = NULL;
struct AudioBus *backgroundBus = NULL;
struct AudioClip *musicClip = NULL;
struct AudioClip *sfx1Clip = NULL;
struct AudioClip *sfx2Clip = NULL;
struct AudioClip *backgroundClip = NULL;

struct ClipPanel *musicPanel = NULL;
struct ClipPanel *sfx1Panel = NULL;
struct ClipPanel *sfx2Panel = NULL;
struct ClipPanel *backgroundPanel = NULL;

SDL_Cursor *handCursor = NULL;
SDL_Cursor *arrowCursor = NULL;

void Init()
{
    window = SDL_CreateWindow("SDL AUDIO PLAYER", WINDOW_W, WINDOW_H, 0);
    renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_PRESENTVSYNC);
    handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

    AudioPlayer_Init();
    musicBus = AudioPlayer_NewBus();
    sfxBus = AudioPlayer_NewBus();
    backgroundBus = AudioPlayer_NewBus();
    sfx1Clip = AudioPlayer_NewAudioClip(sfxBus, "assets/audio/sfx.wav");
    sfx2Clip = AudioPlayer_NewAudioClip(sfxBus, "assets/audio/sfx2.wav");
    backgroundClip = AudioPlayer_NewAudioClip(backgroundBus, "assets/audio/background.wav");
    AudioPlayer_AudioClipSetLoop(backgroundClip, SDL_TRUE);
    musicClip = AudioPlayer_NewAudioClip(musicBus, "assets/audio/music.wav");
    AudioPlayer_AudioClipSetLoop(musicClip, SDL_TRUE);

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("assets/fonts/Molot.otf", 50);
    musicPanel = NewClipPanel(renderer, musicClip, font, "MUSIC", (SDL_Rect){0, 0, WINDOW_W / 2, WINDOW_H / 2});
    backgroundPanel = NewClipPanel(renderer, backgroundClip, font, "BACKGROUND", (SDL_Rect){WINDOW_W / 2, 0, WINDOW_W / 2, WINDOW_H / 2});
    sfx1Panel = NewClipPanel(renderer, sfx1Clip, font, "SFX 1", (SDL_Rect){0, WINDOW_H / 2, WINDOW_W / 2, WINDOW_H / 2});
    sfx2Panel = NewClipPanel(renderer, sfx2Clip, font, "SFX 2", (SDL_Rect){WINDOW_W / 2, WINDOW_H / 2, WINDOW_W / 2, WINDOW_H / 2});
    TTF_CloseFont(font);
}

SDL_bool CheckEvents()
{
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            return SDL_FALSE;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);
            if ((buttons & SDL_BUTTON_LMASK) == 0)
            {
                break;
            }
            if (CheckMouseClickOnClipPanel(musicPanel) == SDL_TRUE)
            {
                break;
            }
            else if (CheckMouseClickOnClipPanel(sfx1Panel) == SDL_TRUE)
            {
                break;
            }
            else if (CheckMouseClickOnClipPanel(sfx2Panel) == SDL_TRUE)
            {
                break;
            }
            else if (CheckMouseClickOnClipPanel(backgroundPanel) == SDL_TRUE)
            {
                break;
            }
            else if (CheckMouseClickOnClipPanel(backgroundPanel) == SDL_TRUE)
            {
                break;
            }
            break;
        default:
            break;
        }
    }
    return SDL_TRUE;
}

void Update()
{
    SDL_bool isHoovering = SDL_FALSE;
    SDL_GetMouseState(&mouseX, &mouseY);
    if (CheckMousePositionOnClipPanel(musicPanel, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    if (CheckMousePositionOnClipPanel(sfx1Panel, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    if (CheckMousePositionOnClipPanel(sfx2Panel, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    if (CheckMousePositionOnClipPanel(backgroundPanel, mouseX, mouseY) == SDL_TRUE)
    {
        isHoovering = SDL_TRUE;
    }
    SDL_SetCursor(isHoovering == SDL_TRUE ? handCursor : arrowCursor);
}

void Draw()
{
    SDL_RenderClear(renderer);
    DrawClipPanel(musicPanel);
    DrawClipPanel(sfx1Panel);
    DrawClipPanel(sfx2Panel);
    DrawClipPanel(backgroundPanel);
    SDL_SetRenderDrawColor(renderer, 190, 190, 190, 255);
    SDL_RenderPresent(renderer);
}

void Loop()
{
    SDL_bool running = SDL_TRUE;
    while (running == SDL_TRUE)
    {
        running = CheckEvents();
        Update();
        Draw();
    }
}

void Quit()
{
    AudioPlayer_Quit();
    FreeClipPanel(musicPanel);
    FreeClipPanel(sfx1Panel);
    FreeClipPanel(sfx2Panel);
    FreeClipPanel(backgroundPanel);
    SDL_DestroyCursor(handCursor);
    SDL_DestroyCursor(arrowCursor);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    Init();
    Loop();
    Quit();
    return 0;
}