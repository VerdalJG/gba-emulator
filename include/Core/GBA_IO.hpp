#pragma once    
#include <cstdint>
#include <unordered_map>

#include "Core/GBA_Memory_Helpers.hpp"
#include "Core/GBA_IO_Helpers.hpp"

class EmulatorCore;
class GBA_PPU;
class GBA_APU;
class GBA_DMAController;
class GBA_TimerController;
class GBA_InterruptController;
class GBA_Keypad;
class GBA_WaitstateController;

class GBA_IO
{
public:
    GBA_IO() = delete;
    explicit GBA_IO(EmulatorCore* core);
    ~GBA_IO() = default;

    void AttachSubsystems(GBA_PPU* ppu, GBA_APU* apu, GBA_DMAController* dma, 
        GBA_TimerController* timers, GBA_InterruptController* interrupts, GBA_Keypad* keypad, 
        GBA_WaitstateController* waitstates);

    MemReadResult<uint8_t> Read8(uint32_t address);
    MemReadResult<uint16_t> Read16(uint32_t address);
    MemReadResult<uint32_t> Read32(uint32_t address);

    void Write8(uint32_t address, uint8_t value);
    void Write16(uint32_t address, uint16_t value);
    void Write32(uint32_t address, uint32_t value);

    // void ResetSIORegisters();
    // void ResetSoundRegisters();
    // void ResetOtherIORegisters();

    IO_LCDRegisters& GetLCDRegisters() { return lcdRegisters; }
    IO_SoundRegisters& GetSoundRegisters() { return soundRegisters; }
    IO_DMARegisters& GetDMARegisters() { return dmaRegisters; }
    IO_TimerRegisters& GetTimerRegisters() { return timerRegisters; }
    IO_SerialRegisters& GetSerialRegisters() { return serialRegisters; }
    IO_KeypadRegisters& GetKeypadRegisters() { return keypadRegisters; }
    IO_InterruptRegisters& GetInterruptRegisters() { return interruptRegisters; }
    IO_MiscRegisters& GetMiscRegisters() { return miscRegisters; }

private:
    void PopulateIORegistersMap();
    void SetupCallbacks();
    
    void SetupLCDReadCallbacks();
    void SetupLCDWriteCallbacks();

    EmulatorCore* core;

    // Components
    GBA_PPU* ppu;
    GBA_APU* apu;
    GBA_DMAController* dma;
    GBA_TimerController* timers;
    GBA_InterruptController* interrupts;
    GBA_Keypad* keypad;
    GBA_WaitstateController* waitstates;

    // Register groups
    IO_LCDRegisters lcdRegisters;
    IO_SoundRegisters soundRegisters;
    IO_DMARegisters dmaRegisters;
    IO_TimerRegisters timerRegisters;
    IO_SerialRegisters serialRegisters;
    IO_KeypadRegisters keypadRegisters;
    IO_InterruptRegisters interruptRegisters;
    IO_MiscRegisters miscRegisters;

    // Address - Register map
    std::unordered_map<uint32_t, IORegister*> ioMap;
};