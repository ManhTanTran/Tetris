// mixer.cpp
#include "mixer.h"
#include <SDL.h>
#include <iostream>

bool Mixer::isInitialized = false;

bool Mixer::init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (Mix_Init(MIX_INIT_MP3) == 0) {
        std::cerr << "Mix_Init Error: " << Mix_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        return false;
    }

    isInitialized = true;
    return true;
}

void Mixer::close() {
    if (isInitialized) {
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
        isInitialized = false;
    }
}

bool Mixer::playMusic(const std::string& musicFile) {
    if (!isInitialized) return false;

    Mix_Music* music = Mix_LoadMUS(musicFile.c_str());
    if (!music) {
        std::cerr << "Mix_LoadMUS Error: " << Mix_GetError() << std::endl;
        return false;
    }

    if (Mix_PlayMusic(music, -1) == -1) {
        std::cerr << "Mix_PlayMusic Error: " << Mix_GetError() << std::endl;
        Mix_FreeMusic(music);
        return false;
    }

    return true;
}

void Mixer::stopMusic() {
    Mix_HaltMusic();
}

void Mixer::playSoundEffect(const std::string& soundFile) {
    if (!isInitialized) return;

    Mix_Chunk* soundEffect = Mix_LoadWAV(soundFile.c_str());
    if (!soundEffect) {
        std::cerr << "Mix_LoadWAV Error: " << Mix_GetError() << std::endl;
        return;
    }

    if (Mix_PlayChannel(-1, soundEffect, 0) == -1) {
        std::cerr << "Mix_PlayChannel Error: " << Mix_GetError() << std::endl;
    }

    Mix_FreeChunk(soundEffect);
}

void Mixer::haltMusic() {
    Mix_HaltMusic(); // dừng nhạc không giải phóng
}

Mix_Music* Mixer::loadMusicForLevel(int level) {
    std::string musicFile = "tetris_music_level" + std::to_string(level) + ".mp3";
    Mix_Music* music = Mix_LoadMUS(musicFile.c_str());

    if (!music) {
        std::cerr << "Mix_LoadMUS Error: " << Mix_GetError() << std::endl;
    }

    return music;
}

