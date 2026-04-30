#ifndef SOUND_H
#define SOUND_H

#include <AL/alut.h>

ALuint bufIntro;
ALuint srcIntro;
ALuint bufGame;
ALuint srcGame;
ALuint bufMenu; 
ALuint srcMenu;
ALuint bufNotime;
ALuint srcNotime;
ALuint bufWin;
ALuint srcWin;

inline void sound_init() {
    alutInit(0, NULL);
    alGetError();
    float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);
    bufIntro = alutCreateBufferFromFile("./jumpscare.wav");
    bufMenu  = alutCreateBufferFromFile("./menu_sound.wav");
    bufGame  = alutCreateBufferFromFile("./menu_sound.wav");
    bufNotime = alutCreateBufferFromFile("./no_time.wav");
    bufWin = alutCreateBufferFromFile("./win_sound.wav");
    alGenSources(1, &srcIntro);
    alGenSources(1, &srcMenu);
    alGenSources(1, &srcGame);
    alGenSources(1, &srcNotime);
    alGenSources(1, &srcWin);
    alSourcei(srcIntro, AL_BUFFER, bufIntro);
    alSourcei(srcMenu,  AL_BUFFER, bufMenu);
    alSourcei(srcGame,  AL_BUFFER, bufGame);
    alSourcei(srcNotime, AL_BUFFER, bufNotime);
    alSourcei(srcWin, AL_BUFFER, bufWin);
    alSourcef(srcIntro, AL_GAIN, 1.0f); alSourcef(srcIntro, AL_PITCH, 1.0f);
    alSourcef(srcMenu,  AL_GAIN, 1.0f); alSourcef(srcMenu,  AL_PITCH, 1.0f);
    alSourcef(srcGame,  AL_GAIN, 1.0f); alSourcef(srcGame,  AL_PITCH, 1.0f);
    alSourcef(srcNotime, AL_GAIN, 1.0f); alSourcef(srcNotime, AL_PITCH, 0.4f);
    alSourcef(srcWin, AL_GAIN, 1.0f); alSourcef(srcWin, AL_PITCH, 1.0f);
}

inline void sound_shutdown() {
    alDeleteSources(1, &srcIntro);
    alDeleteSources(1, &srcMenu);
    alDeleteSources(1, &srcGame);
    alDeleteSources(1, &srcNotime);
    alDeleteSources(1, &srcWin);
    alDeleteBuffers(1, &bufIntro);
    alDeleteBuffers(1, &bufMenu);
    alDeleteBuffers(1, &bufGame);
    alDeleteBuffers(1, &bufNotime);
    alDeleteBuffers(1, &bufWin);
    ALCcontext *ctx = alcGetCurrentContext();
    ALCdevice  *dev = alcGetContextsDevice(ctx);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCloseDevice(dev);
}

#endif // SOUND_H
