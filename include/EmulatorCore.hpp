#pragma once
//#include <SDL3/SDL.h>
//#include "Timer.hpp"
#include "GBACPU.hpp"
#include "GBAMemory.hpp"
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
    GBAMemory memory;
    GBA_CPU cpu;
    //GBA_PPU ppu; // For video
    //GBA_APU apu; // For audio
    //InputHandler inputHandler; // For input handling


//     SDL_Window* window = nullptr;
//     SDL_Renderer* renderer = nullptr;
//     bool isRunning = true;

//     Timer timer;


};