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
    
    void SetUsingHLE(bool enabled) { usingHLEBios = enabled; }
    bool GetUsingHLE() { return usingHLEBios; }

    GBA_CPU& GetCPU() { return cpu; }
    GBA_Memory& GetMemory() { return memory; }


// private:
    void Update();
    void Tick();
    void Render();
    void HandleSDLEvents();

protected:
    GBA_Memory memory;
    GBA_CPU cpu;
    bool usingHLEBios = false; // High level emulation bios, used when can't use GBA real BIOS
    
    //GBA_PPU ppu; // For video
    //GBA_APU apu; // For audio
    //InputHandler inputHandler; // For input handling


//     SDL_Window* window = nullptr;
//     SDL_Renderer* renderer = nullptr;
//     bool isRunning = true;

//     Timer timer;


};