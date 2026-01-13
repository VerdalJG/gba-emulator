#pragma once
//#include <SDL3/SDL3.h>
//#include "Timer.hpp"
#include "Core/GBA_HLE.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/GBA_InterruptController.hpp"
#include "Core/GBA_PPU.hpp"
#include "Core/GBA_TimerController.hpp"
#include "Core/GBA_DMAController.hpp"
#include "Core/GBA_APU.hpp"
#include "Core/GBA_ROM.hpp"
#include "Core/GBA_IO.hpp"

#include <string>
#include <vector>
#include <functional>

class Logger;

class EmulatorCore
{
public:
    EmulatorCore(Logger* logger);
    ~EmulatorCore() = default;

    bool InitializeCPU();
    void LoadBIOS(const std::vector<uint8_t>& biosData);
    bool LoadROM(const std::vector<uint8_t>& romData);
    void Run();
    void Shutdown();
    
    void SetUsingHLE(bool enabled) { usingHLEBios = enabled; }
    bool GetUsingHLE() { return usingHLEBios; }

    GBA_CPU& GetCPU() { return cpu; }
    GBA_Memory& GetMemory() { return memory; }
    GBA_HLE& GetHLE() { return hle; }
    void SetPostStatusCallback(std::function<void(const std::string&)> callback);

    void Step();
    void Tick();
    void Render();
    void HandleSDLEvents();
    void PostStatus(const std::string& message);
    void Log(const std::string& message, LogType logType, const char* functionName);


protected:
    bool usingHLEBios = false; // High level emulation bios, used when can't use GBA real BIOS
    std::function<void(const std::string&)> postStatusCallback;
    Logger* logger;

    // Core Components:
    GBA_HLE hle;
    GBA_CPU cpu;
    GBA_PPU ppu; // For video
    GBA_APU apu; // For audio
    GBA_DMAController dmaController; 
    GBA_TimerController timerController;
    GBA_InterruptController interruptController;
    GBA_IO io;
    GBA_ROM rom;
    GBA_Memory memory;
    //InputHandler inputHandler; // For input handling
};