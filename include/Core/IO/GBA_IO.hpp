#pragma once    
#include "Core/Memory/GBA_Memory_Helpers.hpp"
#include "Core/IO/GBA_IO_Helpers.hpp"

#include "Utils/Integers.hpp"

#include <array>

class EmulatorCore;
class GBA_CPU;
class GBA_PPU;
class GBA_APU;
class GBA_DMAController;
class GBA_TimerController;
class GBA_InterruptController;
class GBA_Keypad;
class GBA_WaitstateController;
class GBA_Bus;

class GBA_IO
{
public:
    GBA_IO() = delete;
    explicit GBA_IO(EmulatorCore* core);
    ~GBA_IO() = default;

    void AttachSubsystems(GBA_CPU* cpu, GBA_PPU* ppu, GBA_APU* apu, GBA_DMAController* dma, 
        GBA_TimerController* timers, GBA_InterruptController* interrupts, GBA_Keypad* keypad, 
        GBA_WaitstateController* waitstates, GBA_Bus* bus);

    // void ResetSIORegisters();
    // void ResetSoundRegisters();
    // void ResetOtherIORegisters();

private:
    u8 Read8(u32 address);
    u16 Read16(u32 address);
    u32 Read32(u32 address);

    void Write8(u32 address, u8 value);
    void Write16(u32 address, u16 value);
    void Write32(u32 address, u32 value);
    

    EmulatorCore* core;

    // Components
    GBA_CPU* cpu;
    GBA_PPU* ppu;
    GBA_APU* apu;
    GBA_DMAController* dma;
    GBA_TimerController* timers;
    GBA_InterruptController* interrupts;
    GBA_Keypad* keypad;
    GBA_WaitstateController* waitstates;
    GBA_Bus* bus;

    // Halfwords = IO_SIZE / 2;
    std::array<HalfwordPermissions, IO_SIZE / 2> ioPermissions;

public:
    template <typename T>
    T Read(u32 address);

    template <typename T>
    void Write(u32 address, T value);
};

#include "Core/Memory/IOMemoryAccess.tpp"


/* Found:

IME on writing:
Bit 0 = master enable
0 → disable all interrupts
1 → enable interrupts

*/ 