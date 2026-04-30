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
    alGenSources(1, &srcIntro);
    alGenSources(1, &srcMenu);
    alGenSources(1, &srcGame);
    alGenSources(1, &srcNotime);
    alSourcei(srcIntro, AL_BUFFER, bufIntro);
    alSourcei(srcMenu,  AL_BUFFER, bufMenu);
    alSourcei(srcGame,  AL_BUFFER, bufGame);
    alSourcei(srcNotime, AL_BUFFER, bufNotime);
    alSourcef(srcIntro, AL_GAIN, 1.0f); alSourcef(srcIntro, AL_PITCH, 1.0f);
    alSourcef(srcMenu,  AL_GAIN, 1.0f); alSourcef(srcMenu,  AL_PITCH, 1.0f);
    alSourcef(srcGame,  AL_GAIN, 1.0f); alSourcef(srcGame,  AL_PITCH, 1.0f);
    alSourcef(srcNotime, AL_GAIN, 1.0f); alSourcef(srcNotime, AL_PITCH, 0.4f);
}

inline void sound_shutdown() {
    alDeleteSources(1, &srcIntro);
    alDeleteSources(1, &srcMenu);
    alDeleteSources(1, &srcGame);
    alDeleteSources(1, &srcNotime);
    alDeleteBuffers(1, &bufIntro);
    alDeleteBuffers(1, &bufMenu);
    alDeleteBuffers(1, &bufGame);
    alDeleteBuffers(1, &bufNotime);
    ALCcontext *ctx = alcGetCurrentContext();
    ALCdevice  *dev = alcGetContextsDevice(ctx);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ctx);
    alcCloseDevice(dev);
}

#endif // SOUND_H
