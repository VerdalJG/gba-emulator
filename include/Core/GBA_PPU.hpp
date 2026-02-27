#pragma once    
#include "Core/GBA_IO_Helpers.hpp"

#include <cstdint>

static constexpr uint32_t CYCLES_PER_FRAME = 228 * 160; // ≈ 36,480
static constexpr uint32_t GBA_WIDTH = 240;
static constexpr uint32_t GBA_HEIGHT = 160;

enum class PPUState
{
    ActiveDisplay,
    HBlank,
    VBlank
};

class EmulatorCore;
class GBA_Bus;

class GBA_PPU
{
public:
    GBA_PPU() = delete;
    explicit GBA_PPU(EmulatorCore* core, GBA_Bus& bus, IO_LCDRegisters& lcdRegisters);
    ~GBA_PPU() = default;

    uint8_t Read8_Bus(uint32_t address);
    uint16_t Read16_Bus(uint32_t address);
    uint32_t Read32_Bus(uint32_t address);

    void RenderFrame();

    const uint32_t* GetFrameBuffer() const { return frameBuffer; }

    bool FrameReady();

    void Step(uint32_t cycles);
    PPUState GetState() const { return state; }
    bool IsAccessingVideoMemory() const { return state == PPUState::ActiveDisplay; }

private:
    uint32_t frameBuffer[240 * 160]; // ARGB8888
    uint32_t frameCycleCounter = 0;
    bool frameReady = false;

    PPUState state = PPUState::VBlank;

    uint32_t currentLine = 0;
    uint32_t cycleInLine = 0;

    EmulatorCore* core;
    GBA_Bus& bus;
    IO_LCDRegisters& lcdRegisters;
};


// Documentation: 
// https://www.reddit.com/r/EmuDev/comments/116nrtf/cycleaccurate_ppu_emulation_in_nanoboyadvance_my/