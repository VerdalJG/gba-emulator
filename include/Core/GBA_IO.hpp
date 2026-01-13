#pragma once    
#include <cstdint>

class EmulatorCore;
class GBA_PPU;
class GBA_APU;
class GBA_DMAController;
class GBA_TimerController;
class GBA_InterruptController;
class GBA_Keypad;

class GBA_IO
{
public:
    GBA_IO() = delete;
    explicit GBA_IO(EmulatorCore* core, GBA_PPU* ppu, GBA_APU* apu, 
        GBA_DMAController* dmaController, GBA_TimerController* timerController, 
        GBA_InterruptController* interruptController);

    ~GBA_IO() = default;

    uint8_t  Read8(uint32_t address);
    uint16_t Read16(uint32_t address);
    uint32_t Read32(uint32_t address);

    void Write8(uint32_t address, uint8_t value);
    void Write16(uint32_t address, uint16_t value);
    void Write32(uint32_t address, uint32_t value);

private:
    EmulatorCore* core;
    GBA_PPU* ppu;
    GBA_APU* apu;
    GBA_DMAController* dma;
    GBA_TimerController* timers;
    GBA_InterruptController* interrupts;
    GBA_Keypad* keypad;
};