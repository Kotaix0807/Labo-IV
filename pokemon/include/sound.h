#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define SFX_DIR "src/sfx/"
#define MUSIC_DIR "src/music/"

typedef struct sounds_{
    Mix_Chunk **chunks;
    int n;
}sfx;

typedef struct songs{
    Mix_Music **chunks;
    int n;
}music;

int initAudio(void);
void quitAudio(void);
void playAndFreeSfx(const char *sound);
sfx *initSfxLib(char *sounds[], int n);
music *initMusicLib(char *songs[], int n);
void freeSfxLib(sfx *cur);
void freeMusicLib(music *cur);

#endif /* SOUND_H */
