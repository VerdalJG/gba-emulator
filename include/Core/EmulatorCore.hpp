#pragma once
#include "Core/GBA_HLE.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/GBA_Bus.hpp"
#include "Core/Memory/GBA_Memory.hpp"
#include "Core/GBA_InterruptController.hpp"
#include "Core/GBA_PPU.hpp"
#include "Core/GBA_TimerController.hpp"
#include "Core/GBA_DMAController.hpp"
#include "Core/GBA_APU.hpp"
#include "Core/GBA_ROM.hpp"
#include "Core/IO/GBA_IO.hpp"
#include "Core/GBA_Keypad.hpp"

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
    bool UsingHLE() { return usingHLEBios; }

    GBA_CPU& GetCPU() { return cpu; }
    GBA_Memory& GetMemory() { return memory; }
    GBA_HLE& GetHLE() { return hle; }
    GBA_PPU& GetPPU() { return ppu; }
    void SetPostStatusCallback(std::function<void(const std::string&)> callback);

    void RunFrame();
    void Tick();
    void Render();
    void HandleSDLEvents();
    void PostStatus(const std::string& message);
    void Log(const std::string& message, LogType logType, const std::string functionName = "");


protected:
    bool usingHLEBios = false; // High level emulation bios, used when can't use GBA real BIOS
    std::function<void(const std::string&)> postStatusCallback;
    Logger* logger;

    // Core Components:

    // Memory-related components
    GBA_IO io;
    GBA_ROM rom;
    GBA_Memory memory;
    GBA_Bus bus;

    // Hardware components
    GBA_PPU ppu; // For video
    GBA_APU apu; // For audio
    GBA_DMAController dmaController; 
    GBA_TimerController timerController;
    GBA_InterruptController interruptController;
    GBA_Keypad keypad;
    GBA_CPU cpu;
    GBA_HLE hle;
};