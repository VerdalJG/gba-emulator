#pragma once

#include "Core/Memory/GBA_Memory_Helpers.hpp"
#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Utils/Integers.hpp"

#include <array>

struct WaitstateControl // Read/Write
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset(bool skipBios) { value = skipBios ? 0x4317 : 0; }

    // Returns total cycles (1 + waitstates) for this access.
    // isSequential is whether the access is sequential (S) or non-sequential (N).
    int GetCycles(RegionType region, bool isSequential) const;

    bool IsPrefetchEnabled() const { return fields.gamepakPrefetchBuffer; }

    const u16 writeMask = 0x6FFF;

    union
    {
        struct
        {
            u16 sramWaitControl : 2; // (0..3 = 4,3,2,8 cycles)
            u16 waitstate0FirstAccess : 2; // (0..3 = 4,3,2,8 cycles) - Nonsequential
            u16 waitstate0SecondAccess : 1; // (0..1 = 2,1 cycles) - Sequential
            u16 waitstate1FirstAccess : 2; // (0..3 = 4,3,2,8 cycles) - Nonsequential
            u16 waitstate1SecondAccess : 1; // (0..1 = 4,1 cycles; unlike above WS0) - Sequential
            u16 waitstate2FirstAccess : 2; // (0..3 = 4,3,2,8 cycles) - Nonsequential
            u16 waitstate2SecondAccess : 1; // (0..1 = 8,1 cycles; unlike above WS0,WS1) - Sequential
            u16 phiTerminalOutput : 2; // (0..3 = Disable, 4.19MHz, 8.38MHz, 16.78MHz)
            u16 unused : 1;
            u16 gamepakPrefetchBuffer : 1; // (Pipeline) (0 = Disable, 1 = Enable)
            u16 gamepakTypeFlag : 1; // (Read-only) (0=GBA, 1=CGB) (IN35 signal)
        } fields;
        u16 value;
    };
};

struct PostBootFlag
{
    u8 Read8() { return value; }
    void Write8(u8 value) { this->value = value & writeMask; }
    void Reset() { value = 0; }
    void ResetToPostBIOSValue() { value = 0x01; }

    const u8 writeMask = 0x01;

    union
    {
        struct
        {
            u8 firstBootFlag : 1; // (0 = First, 1 = Further)
            u8 unused : 7;
        } fields;
        u8 value;
    };
};

struct PowerDownControl
{
    void Write8(u8 value) { this->value = value & writeMask; }
    void Reset() { value = 0; }

    const u8 writeMask = 0x80;

    union
    {
        struct
        {
            u8 unused : 7; 
            u8 powerDownMode : 1; // (0=Halt, 1=Stop)
        } fields;
        u8 value;
    };
};

struct InternalMemoryControl // R/W
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset() { value = 0; }

    const u32 writeMask = 0xFF00002F;

    union
    {
        struct
        {
            u32 disableWRAM : 1;    // (0=Normal, 1=Disable) (when off: empty/prefetch) From endrift: bit0 swaps
                                    // 00000000h-01FFFFFFh and 02000000h-03FFFFFFh in GBA mode (but keeps BIOS protection)
            u32 unknown0 : 2; // (Read/Write-able)
            u32 disableCGBBootrom : 1; // (0=Normal, 1=Disable, start cart at 0000h)
            u32 unused0 : 1;
            u32 enableEWRAM : 1; // (0=Disable, 1=Normal) (when off: mirror of 32K WRAM)
            u32 unused1 : 18; // Hardwired to 0
            u32 waitControlEWRAM : 4; // 256K (0-14 = 15..1 Waitstates, 15=Lockup)
            u32 unknown1 : 4; // (Read/Write-able)
        } fields;
        u32 value;
    };
};

struct IO_SystemControlRegisters
{
    PostBootFlag postflg;
    PowerDownControl haltcnt;
    u8 undocumentedReg; // The BIOS writes the 8bit value 0FFh to this address. Purpose Unknown.
    InternalMemoryControl imemcnt;

    void Reset(bool skipBios)
    {
        if (skipBios)
        {
            postflg.ResetToPostBIOSValue();
        }
        else 
        {
            postflg.Reset();
        }
    
        haltcnt.Reset();
        undocumentedReg = 0;
        imemcnt.Reset();
    }
};