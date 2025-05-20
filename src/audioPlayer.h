#ifndef AUDIO_PLAYER_H
#define AUDIO_PLAYER_H

#include <SDL3/SDL.h>

enum TransportState
{
    PLAY,
    PAUSE,
    FADEIN,
    FADEOUT,
    STOP
};

struct AudioClip
{
    float volume;
    float currentVolume;
    float dryVolume;
    float wetVolume;
    int fadeSamplesLength;
    float leftPan;
    float rightPan;
    double msDuration;
    SDL_AudioStream *stream;
    Uint32 wavLength;
    Uint32 audioLength;
    Uint8 *wavBuffer;
    Uint8 *audioPos;
    SDL_AudioSpec spec;
    bool loop;
    enum TransportState state;
    enum TransportState nextState;
};

struct ClipNode
{
    struct AudioClip *clip;
    struct ClipNode *next;
};

struct AudioBus
{
    int volume;
    int currentVolume;
    SDL_AudioStream *stream;
    struct ClipNode *headClip;
};
struct BusNode
{
    struct AudioBus *bus;
    struct BusNode *next;
};

void AudioPlayer_Init();
void AudioPlayer_Quit();
void AudioPlayer_SetMasterVolume(float volume);

void AudioPlayer_SetBusVolume(struct AudioBus *, float volume);
struct AudioBus *AudioPlayer_NewBus();

void AudioPlayer_AudioClipSetLoop(struct AudioClip *clip, bool shouldLoop);
bool AudioPlayer_AudioClipIsPlaying(struct AudioClip *clip);
void AudioPlayer_SetAudioClipPan(struct AudioClip *clip, float pan);
void AudioPlayer_SetAudioClipVolume(struct AudioClip *clip, float volume);
void AudioPlayer_PlayAudioClip(struct AudioClip *clip);
void AudioPlayer_PauseAudioClip(struct AudioClip *clip);
void AudioPlayer_ResumeAudioClip(struct AudioClip *clip);
void AudioPlayer_StopAudioClip(struct AudioClip *clip);
void AudioPlayer_FadeOutAudioClip(struct AudioClip *clip, float fadeValue);
void AudioPlayer_FadeInAudioClip(struct AudioClip *clip, float fadeValue);
struct AudioClip *AudioPlayer_NewAudioClip(struct AudioBus *bus, const char *path);

#endif