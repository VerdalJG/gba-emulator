#pragma once
//#include <SDL3/SDL.h>
//#include "Timer.hpp"
#include "GBACPU.hpp"
#include <string>

class EmulatorCore
{
public:
    EmulatorCore();
    ~EmulatorCore() = default;

    bool Init();
    bool LoadROM(const std::string& romPath);
    void Run();
    void Shutdown();

// private:
    void Update();
    void Tick();
    void Render();
    void HandleSDLEvents();

//     SDL_Window* window = nullptr;
//     SDL_Renderer* renderer = nullptr;
//     bool isRunning = true;

//     Timer timer;
//     GBA_CPU cpu;
    //GBA_PPU ppu; // For video
    //GBA_APU apu; // For audio
    //InputHandler inputHandler; // For input handling

};