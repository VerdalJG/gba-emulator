#pragma once
//#include <SDL3/SDL3.h>
//#include "Timer.hpp"
#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/CPU_Memory.hpp"
#include <string>
#include <vector>

class EmulatorCore
{
public:
    EmulatorCore();
    ~EmulatorCore() = default;

    bool Init();
    void LoadBIOS(const std::vector<uint8_t>& biosData);
    bool LoadROM(const std::vector<uint8_t>& romData);
    void Run();
    void Shutdown();

// private:
    void Update();
    void Tick();
    void Render();
    void HandleSDLEvents();

private:
    GBA_Memory memory;
    GBA_CPU cpu;
    //GBA_PPU ppu; // For video
    //GBA_APU apu; // For audio
    //InputHandler inputHandler; // For input handling


//     SDL_Window* window = nullptr;
//     SDL_Renderer* renderer = nullptr;
//     bool isRunning = true;

//     Timer timer;


};