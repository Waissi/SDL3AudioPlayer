#include "audioPlayer.h"

static SDL_AudioDeviceID audioDeviceId;
static const SDL_AudioSpec audioSpec = {SDL_AUDIO_S16, 2, 44100};
static struct BusNode *headBus = NULL;
static float masterVolume = 1.0f;

static int ReleaseAudioClip(struct AudioClip *clip)
{
    SDL_free(clip->wavBuffer);
    SDL_free(clip);
    return 0;
}

static int ReleaseAllClips(struct ClipNode *clipNode)
{
    while (clipNode)
    {
        struct ClipNode *currentNode = clipNode;
        clipNode = currentNode->next;
        if (ReleaseAudioClip(currentNode->clip) != 0)
        {
            return -1;
        }
        SDL_free(currentNode);
    }
    return 0;
}

static int ReleaseAudioBus(struct AudioBus *bus)
{
    if (SDL_ClearAudioStream(bus->stream) != 0)
    {
        SDL_Log("Could not clear audio stream, %s", SDL_GetError());
        return -1;
    }
    SDL_DestroyAudioStream(bus->stream);
    if (ReleaseAllClips(bus->headClip) != 0)
    {
        return -1;
    }
    SDL_free(bus);
    return 0;
}

static int ReleaseAllBuses()
{
    while (headBus)
    {
        struct BusNode *currentNode = headBus;
        headBus = currentNode->next;
        if (ReleaseAudioBus(currentNode->bus) != 0)
        {
            return -1;
        }
        SDL_free(currentNode);
    }
    return 0;
}

void AudioPlayer_Quit()
{
    if (ReleaseAllBuses() == 0)
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        SDL_Log("AudioPlayer released");
        return;
    }
    SDL_Log("Could not release AudioPlayer");
}

static void SDLCALL PostmixCallback(void *userdata, const SDL_AudioSpec *spec, float *buffer, int buflen)
{
}

void AudioPlayer_Init()
{
    if (SDL_WasInit(SDL_INIT_AUDIO) == SDL_INIT_AUDIO)
    {
        SDL_Log("SDL_Audio subsystem initialized");
    }
    else if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("Could not initialize audio subsystem, %s", SDL_GetError());
        return;
    }
    audioDeviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_OUTPUT, &audioSpec);
    if (audioDeviceId == 0)
    {
        SDL_Log("Could not open audio device default output %s", SDL_GetError());
        return;
    }
    if (SDL_SetAudioPostmixCallback(audioDeviceId, PostmixCallback, NULL) != 0)
    {
        SDL_Log("Could not set postmix callback, %s", SDL_GetError());
        return;
    }
    SDL_Log("AudioPlayer ready");
}

static void StreamAudioClip(struct AudioClip *clip, Sint16 *busSamples, int additionalAmount)
{
    if (!AudioPlayer_AudioClipIsPlaying(clip))
    {
        return;
    }

    int bufferSize = SDL_min(additionalAmount, clip->wavLength - clip->audioLength);
    double samplesRatio = (double)bufferSize / (double)clip->fadeSamplesLength;
    switch (clip->state)
    {
    case FADEIN:
        clip->currentVolume += clip->volume * samplesRatio;
        if (clip->currentVolume >= clip->volume)
        {
            clip->currentVolume = clip->volume;
            clip->state = PLAY;
        }
        break;
    case FADEOUT:
        clip->currentVolume -= clip->volume * samplesRatio;
        if (clip->currentVolume <= 0.0f)
        {
            clip->state = clip->nextState;
            clip->currentVolume = clip->volume;
            return;
        }
        break;
    default:
        clip->currentVolume = clip->volume;
        break;
    }

    Sint16 samples[bufferSize];
    SDL_memcpy(samples, ((const Uint8 *)clip->audioPos), (size_t)bufferSize);
    if (bufferSize < additionalAmount)
    {
        float endQueue;
        for (int i = 0; i < bufferSize; i += 2)
        {
            endQueue = clip->currentVolume - (clip->currentVolume / bufferSize) * i;
            endQueue = endQueue > 0.05f ? endQueue : 0.0f;
            samples[i] *= endQueue * clip->leftPan;
            samples[i + 1] *= endQueue * clip->rightPan;
            busSamples[i] += samples[i];
            busSamples[i + 1] += samples[i + 1];
        }
    }
    else
    {
        for (int i = 0; i < bufferSize; i += 2)
        {
            samples[i] *= clip->currentVolume * clip->leftPan;
            samples[i + 1] *= clip->currentVolume * clip->rightPan;
            busSamples[i] += samples[i];
            busSamples[i + 1] += samples[i + 1];
        }
    }
    clip->audioPos += bufferSize;
    clip->audioLength += bufferSize;

    if (clip->audioLength >= clip->wavLength)
    {
        if (clip->loop == SDL_TRUE)
        {
            clip->audioLength = 0;
            clip->audioPos = clip->wavBuffer;
        }
        else
        {
            clip->state = STOP;
        }
    }
}

static void SDLCALL BusCallback(void *bus, SDL_AudioStream *stream, int additionalAmount, int totalAmount)
{
    struct AudioBus *currentBus = (struct AudioBus *)bus;
    struct ClipNode *clipNode = currentBus->headClip;

    Sint16 samples[additionalAmount];
    SDL_memset(samples, '\0', additionalAmount);

    while (clipNode)
    {
        struct AudioClip *currentClip = clipNode->clip;
        StreamAudioClip(currentClip, samples, additionalAmount);
        clipNode = clipNode->next;
    }

    Uint8 copiedSamples[additionalAmount];
    SDL_memset(copiedSamples, '\0', additionalAmount);
    SDL_memcpy(copiedSamples, ((const Sint16 *)samples), (size_t)additionalAmount);

    Uint8 mixBuffer[additionalAmount];
    SDL_memset(mixBuffer, '\0', additionalAmount);
    if (SDL_MixAudioFormat(mixBuffer, copiedSamples, SDL_AUDIO_S16, additionalAmount, SDL_min(currentBus->currentVolume, 128)) != 0)
    {
        SDL_Log("Error mixing the bus samples %s", SDL_GetError());
    }
    if (SDL_PutAudioStreamData(stream, mixBuffer, additionalAmount) != 0)
    {
        SDL_Log("Could not put data on Audio stream, %s", SDL_GetError());
    }
}

struct AudioBus *AudioPlayer_NewBus()
{
    struct AudioBus *newAudioBus = (struct AudioBus *)SDL_malloc(sizeof(struct AudioBus));
    SDL_AudioStream *stream = SDL_OpenAudioDeviceStream(audioDeviceId, &audioSpec, BusCallback, newAudioBus);
    if (!stream)
    {
        SDL_free(newAudioBus);
        SDL_Log("Could not create audio stream, %s", SDL_GetError());
        return NULL;
    }
    if (SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream)) != 0)
    {
        SDL_free(newAudioBus);
        SDL_Log("Could not resume audio device, %s", SDL_GetError());
        return NULL;
    }

    struct BusNode *newBusNode = (struct BusNode *)SDL_malloc(sizeof(struct BusNode));
    newBusNode->bus = newAudioBus;
    newBusNode->next = headBus;
    headBus = newBusNode;

    newAudioBus->stream = stream;
    newAudioBus->volume = 128;
    newAudioBus->currentVolume = 128;
    newAudioBus->headClip = NULL;
    return newAudioBus;
}

struct AudioClip *AudioPlayer_NewAudioClip(struct AudioBus *bus, const char *path)
{
    struct AudioClip *newAudioClip = (struct AudioClip *)SDL_malloc(sizeof(struct AudioClip));
    if (SDL_LoadWAV(path, &newAudioClip->spec, &newAudioClip->wavBuffer, &newAudioClip->wavLength) != 0)
    {
        SDL_free(newAudioClip);
        SDL_Log("Could not load audio file %s, %s", path, SDL_GetError());
        return NULL;
    }
    Uint32 sampleSize = SDL_AUDIO_BITSIZE(newAudioClip->spec.format) / 8;
    Uint32 sampleCount = newAudioClip->wavLength / sampleSize;
    Uint32 sampleLen = sampleCount / newAudioClip->spec.channels;
    newAudioClip->msDuration = ((double)sampleLen / (double)newAudioClip->spec.freq) * 1000;
    newAudioClip->volume = 1.0f;
    newAudioClip->currentVolume = 1.0f;
    newAudioClip->currentVolume = 1.0f;
    newAudioClip->leftPan = 1.0f;
    newAudioClip->rightPan = 1.0f;
    newAudioClip->fadeSamplesLength = 0;
    newAudioClip->stream = bus->stream;
    newAudioClip->audioPos = newAudioClip->wavBuffer;
    newAudioClip->audioLength = 0;
    newAudioClip->state = STOP;
    newAudioClip->loop = SDL_FALSE;
    struct ClipNode *newClipNode = (struct ClipNode *)SDL_malloc(sizeof(struct ClipNode));
    newClipNode->clip = newAudioClip;
    newClipNode->next = bus->headClip;
    bus->headClip = newClipNode;
    return newAudioClip;
}

static void FadeOutClip(struct AudioClip *clip, float duration, enum TransportState nextState)
{
    clip->state = FADEOUT;
    clip->nextState = nextState;
    float fadeDurationRatio = clip->msDuration / (duration * 1000);
    clip->fadeSamplesLength = clip->wavLength / fadeDurationRatio;
}

void AudioPlayer_FadeOutAudioClip(struct AudioClip *clip, float duration)
{
    if (!clip)
        return;
    FadeOutClip(clip, duration, STOP);
}

void AudioPlayer_FadeInAudioClip(struct AudioClip *clip, float duration)
{
    if (!clip)
        return;
    clip->state = FADEIN;
    clip->currentVolume = 0.0f;
    clip->currentVolume = 0.0f;
    float fadeDurationRatio = clip->msDuration / (duration * 1000);
    clip->fadeSamplesLength = clip->wavLength / fadeDurationRatio;
}

void AudioPlayer_PlayAudioClip(struct AudioClip *clip)
{
    if (!clip)
        return;
    clip->audioLength = 0;
    clip->audioPos = clip->wavBuffer;
    AudioPlayer_FadeInAudioClip(clip, 0.1f);
}

void AudioPlayer_PauseAudioClip(struct AudioClip *clip)
{
    if (!clip)
        return;
    if (clip->state == PLAY)
    {
        FadeOutClip(clip, 0.1f, PAUSE);
    }
}

void AudioPlayer_ResumeAudioClip(struct AudioClip *clip)
{
    if (!clip)
        return;
    if (clip->state == PAUSE)
    {
        AudioPlayer_FadeInAudioClip(clip, 0.1f);
    }
}

void AudioPlayer_StopAudioClip(struct AudioClip *clip)
{
    if (!clip)
        return;
    if (clip->state == STOP)
    {
        return;
    }
    FadeOutClip(clip, 0.1f, STOP);
}

void AudioPlayer_AudioClipSetLoop(struct AudioClip *clip, SDL_bool loop)
{
    if (!clip)
        return;
    clip->loop = loop;
}

SDL_bool AudioPlayer_AudioClipIsPlaying(struct AudioClip *clip)
{
    if (!clip)
        return SDL_FALSE;
    return clip->state != STOP && clip->state != PAUSE;
}

void AudioPlayer_SetMasterVolume(float volume)
{
    masterVolume = volume;
    struct BusNode *currentNode = headBus;
    while (currentNode)
    {
        struct AudioBus *bus = currentNode->bus;
        bus->currentVolume = (int)bus->volume * volume;
        currentNode = currentNode->next;
    }
}

void AudioPlayer_SetBusVolume(struct AudioBus *bus, float volume)
{
    if (!bus)
        return;
    bus->volume = (int)volume * 128;
    bus->currentVolume = (int)volume * masterVolume * 128;
}

void AudioPlayer_SetAudioClipPan(struct AudioClip *clip, float pan)
{
    if (!clip)
        return;
    pan = SDL_clamp(pan, -0.9f, 0.9f);
    if (pan >= 0.0f)
    {
        clip->leftPan = 1.0f - pan;
        clip->rightPan = 1.0f;
        return;
    }
    clip->leftPan = 1.0f;
    clip->rightPan = 1.0f + pan;
}

void AudioPlayer_SetAudioClipVolume(struct AudioClip *clip, float volume)
{
    if (!clip)
        return;
    clip->volume = volume;
}