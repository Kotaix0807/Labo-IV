#include <stdio.h>
#include <stdlib.h>

#include "sound.h"

int initAudio(void)
{
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0)
    {
        if (SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            fprintf(stderr, "Error SDL_Init: %s\n", SDL_GetError());
            return 0;
        }
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr, "Error Mix_OpenAudio: %s\n", Mix_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return 0;
    }
    return 1;
}

void quitAudio(void)
{
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void playAndFreeSfx(const char *sound)
{
    if (!initAudio())
        return;

    char path[256];
    snprintf(path, sizeof(path), "%s%s", SFX_DIR, sound);

    Mix_Chunk *sfx_chunk = Mix_LoadWAV(path);
    if(!sfx_chunk) {
        fprintf(stderr, "Error al cargar %s: %s\n", path, Mix_GetError());
        return;
    }

    int channel = Mix_PlayChannel(-1, sfx_chunk, 0);
    if (channel == -1) {
        fprintf(stderr, "Error al reproducir %s: %s\n", path, Mix_GetError());
        Mix_FreeChunk(sfx_chunk);
        return;
    }

    while (Mix_Playing(channel))
        SDL_Delay(10);

    Mix_FreeChunk(sfx_chunk);
}

sfx *initSfxLib(char *sounds[], int n)
{
    if (!initAudio())
        return NULL;

    if (!sounds || n <= 0)
        return NULL;

    sfx *cur = calloc(1, sizeof(sfx));
    if (!cur)
        return NULL;

    cur->n = n;
    cur->chunks = calloc((size_t)n, sizeof(Mix_Chunk *));
    if (!cur->chunks) {
        free(cur);
        return NULL;
    }

    for(int i = 0; i < n; i++)
    {
        if(!sounds[i])
            continue;
        char path[256];
        snprintf(path, sizeof(path), "%s%s", SFX_DIR, sounds[i]);
        cur->chunks[i] = Mix_LoadWAV(path);
        if(!cur->chunks[i])
            fprintf(stderr, "Error al cargar %s: %s\n", path, Mix_GetError());
    }
    return cur;
}

music *initMusicLib(char *songs[], int n)
{
    if (!initAudio())
        return NULL;

    if (!songs || n <= 0)
        return NULL;

    music *cur = calloc(1, sizeof(music));
    if (!cur)
        return NULL;

    cur->n = n;
    cur->chunks = calloc((size_t)n, sizeof(Mix_Music *));
    if (!cur->chunks) {
        free(cur);
        return NULL;
    }

    for(int i = 0; i < n; i++)
    {
        if(songs[i]) {
            char path[256];
            snprintf(path, sizeof(path), "%s%s", MUSIC_DIR, songs[i]);
            cur->chunks[i] = Mix_LoadMUS(path);
            if(!cur->chunks[i])
                fprintf(stderr, "Error al cargar %s: %s\n", path, Mix_GetError());
        }
    }
    return cur;
}

void freeSfxLib(sfx *cur)
{
    if(!cur)
        return;

    if(cur->chunks) {
        for(int i = 0; i < cur->n; i++)
        {
            if(cur->chunks[i])
                Mix_FreeChunk(cur->chunks[i]);
        }
        free(cur->chunks);
    }
    cur->chunks = NULL;
    cur->n = 0;
    free(cur);
}

void freeMusicLib(music *cur)
{
    if(!cur)
        return;

    if(cur->chunks) {
        for(int i = 0; i < cur->n; i++)
        {
            if(cur->chunks[i])
                Mix_FreeMusic(cur->chunks[i]);
        }
        free(cur->chunks);
    }
    cur->chunks = NULL;
    cur->n = 0;
    free(cur);
}
