#pragma once    
#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Core/IO/IORegisters.hpp"

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
    explicit GBA_PPU(EmulatorCore* core, GBA_Bus& bus);
    ~GBA_PPU() = default;

    uint8_t Read8_Bus(uint32_t address);
    uint16_t Read16_Bus(uint32_t address);
    uint32_t Read32_Bus(uint32_t address);

    void RenderFrame();

    const uint32_t* GetFrameBuffer() const { return frameBuffer; }

    bool FrameReady();

    void Step(u32 cycles);
    PPUState GetState() const { return state; }
    bool InVBlank() const { return scanline >= 160;}
    bool InHBlank() const { return dotCycle >= 960;}
    u32 GetVCount() const { return scanline; }
    bool IsForcedBlankEnabled() { return lcdRegisters.dispcnt.value & (1 << 7); }
    bool IsAccessingVideoMemory() const { return state == PPUState::ActiveDisplay; }

    bool IsWithinVRAM_OBJBoundary(u32 address);

    IO_LCDRegisters& GetLCDRegisters() { return lcdRegisters; }

private:
    uint32_t frameBuffer[240 * 160]; // ARGB8888
    uint32_t frameCycleCounter = 0;
    bool frameReady = false;

    PPUState state = PPUState::VBlank;

    u32 scanline = 0;        // 0–227
    u32 dotCycle = 0;        // 0–1231

    EmulatorCore* core;
    GBA_Bus& bus;
    IO_LCDRegisters lcdRegisters;
};


// Documentation: 
// https://www.reddit.com/r/EmuDev/comments/116nrtf/cycleaccurate_ppu_emulation_in_nanoboyadvance_my/