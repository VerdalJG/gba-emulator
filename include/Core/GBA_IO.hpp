#pragma once    
#include "Core/Memory/GBA_Memory_Helpers.hpp"
#include "Core/GBA_IO_Helpers.hpp"

#include "Utils/Integers.hpp"

#include <unordered_map>

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

    MemReadResult<u8> Read8(u32 address);
    MemReadResult<u16> Read16(u32 address);
    MemReadResult<u32> Read32(u32 address);

    void Write8(u32 address, u8 value);
    void Write16(u32 address, u16 value);
    void Write32(u32 address, u32 value);

    // void ResetSIORegisters();
    // void ResetSoundRegisters();
    // void ResetOtherIORegisters();

    bool IsValidIORegister(u32 address);

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
    std::array<IORegister*, 0x800> ioRegisters;
};


/* Found:

IME on writing:
Bit 0 = master enable
0 → disable all interrupts
1 → enable interrupts


*/ 