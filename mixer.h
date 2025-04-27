// mixer.h
#ifndef MIXER_H
#define MIXER_H

#include <SDL_mixer.h>
#include <string>

class Mixer {
public:
    // Khởi tạo SDL_mixer
    static bool init();

    // Đóng SDL_mixer
    static void close();

    // Phát nhạc nền (tên file nhạc)
    static bool playMusic(const std::string& musicFile);
    static void haltMusic(); // mới

    // Dừng nhạc nền
    static void stopMusic();

    // Phát âm thanh hiệu ứng (tên file âm thanh)
    static void playSoundEffect(const std::string& soundFile);

    static Mix_Music* loadMusicForLevel(int level);

private:
    static bool isInitialized;  // Biến kiểm tra xem SDL_mixer đã được khởi tạo chưa
};

#endif // MIXER_H

