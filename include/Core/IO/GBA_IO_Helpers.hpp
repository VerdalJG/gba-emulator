#pragma once
#include "Core/Memory/GBA_Memory_Helpers.hpp"

#include "Utils/Integers.hpp"

struct HalfwordPermissions
{
    uint readMask;
    uint writeMask;

    constexpr bool CanReadLo() const { return readMask & 0b01; }
    constexpr bool CanReadHi() const { return readMask & 0b10; }

    constexpr bool CanWriteLo() const { return writeMask & 0b01; }
    constexpr bool CanWriteHi() const { return writeMask & 0b10; }

    constexpr bool CanReadHalf() const { return readMask == 0b11; }
    constexpr bool CanWriteHalf() const { return writeMask == 0b11; }
};

namespace IO
{
    inline u8 Read8(u16& reg, int byte)
    {
        return (reg >> (8 * byte)) & 0xFF;
    }

    inline u8 Read8(u32& reg, int byte)
    {
        return (reg >> (8 * byte)) & 0xFF;
    }

    inline void Write8Masked(u16& reg, int byte, u8 value, u16 writeableMask = 0xFFFF)
    {
        const int shift = byte * 8;

        u8 old = (reg >> shift) & 0xFF;
        u8 mask = (writeableMask >> shift) & 0xFF;

        u8 result = (value & mask) | (old & ~mask);
        
        reg &= ~(0xFF << shift); // Clear
        reg |= result << shift;
    }

    inline void Write8Masked(u32& reg, int byte, u8 value, u32 writeableMask = 0xFFFFFFFF)
    {
        const int shift = byte * 8;

        u8 old = (reg >> shift) & 0xFF;
        u8 mask = (writeableMask >> shift) & 0xFF;

        u8 result = (value & mask) | (old & ~mask);
        
        reg &= ~(0xFF << shift); // Clear
        reg |= result << shift;
    }

    inline void Write16ByBytes(u16& reg, u16 value, u16 writeableMask = 0xFFFF)
    {
        Write8Masked(reg, 0, (value & 0xFF), writeableMask);
        Write8Masked(reg, 1, (value >> 8) & 0xFF, writeableMask);
    }

    inline void Write16Masked(u32& reg, int halfword, u16 value, u32 writeableMask = 0xFFFFFFFF)
    {
        const int shift = halfword * 16;

        u16 old = (reg >> shift) & 0xFFFF;
        u16 mask = (writeableMask >> shift) & 0xFFFF;

        u16 result = (value & mask) | (old & ~mask);
        
        reg &= ~(0xFFFF << shift); // Clear
        reg |= result << shift;
    }

    inline void Write32ByBytes(u32& reg, u32 value, u32 writeableMask = 0xFFFFFFFF)
    {
        
    }
}

// struct IO_SoundRegisters
// {
//     IORegister SOUND1CNT_L { 0x04000060, AccessSize::Halfword, true, true, 0x0000 }; // Channel 1 Sweep register (NR10)
//     IORegister SOUND1CNT_H { 0x04000062, AccessSize::Halfword, true, true, 0x0000 }; // Channel 1 Duty/Length/Envelope (NR11, NR12)
//     IORegister SOUND1CNT_X { 0x04000064, AccessSize::Halfword, true, true, 0x0000 }; // Channel 1 Frequency/Control (NR13, NR14)

//     IORegister SOUND2CNT_L { 0x04000068, AccessSize::Halfword, true, true, 0x0000 }; // Channel 2 Duty/Length/Envelope (NR21, NR22)
//     IORegister SOUND2CNT_H { 0x0400006C, AccessSize::Halfword, true, true, 0x0000 }; // Channel 2 Frequency/Control (NR23, NR24)

//     IORegister SOUND3CNT_L { 0x04000070, AccessSize::Halfword, true, true, 0x0000 }; // Channel 3 Stop/Wave RAM select (NR30)
//     IORegister SOUND3CNT_H { 0x04000072, AccessSize::Halfword, true, true, 0x0000 }; // Channel 3 Length/Volume (NR31, NR32)
//     IORegister SOUND3CNT_X { 0x04000074, AccessSize::Halfword, true, true, 0x0000 }; // Channel 3 Frequency/Control (NR33, NR34)

//     IORegister SOUND4CNT_L { 0x04000078, AccessSize::Halfword, true, true, 0x0000 }; // Channel 4 Length/Envelope (NR41, NR42)
//     IORegister SOUND4CNT_H { 0x0400007C, AccessSize::Halfword, true, true, 0x0000 }; // Channel 4 Frequency/Control (NR43, NR44)

//     IORegister SOUNDCNT_L { 0x04000080, AccessSize::Halfword, true, true, 0x0080 }; // Control Stereo/Volume/Enable (NR50, NR51)
//     IORegister SOUNDCNT_H { 0x04000082, AccessSize::Halfword, true, true, 0x0000 }; // Control Mixing/DMA Control
//     IORegister SOUNDCNT_X { 0x04000084, AccessSize::Halfword, true, true, 0x0080 }; // Control Sound on/off (NR52)

//     IORegister SOUNDBIAS { 0x04000088, AccessSize::Halfword, true, true, 0x0200 }; // Sound PWM Control (BIOS)
//     IORegister WAVE_RAM  { 0x04000090, AccessSize::Word, true, true, 0x00000000 }; // Channel 3 Wave Pattern RAM (2 banks!!)

//     IORegister FIFO_A    { 0x040000A0, AccessSize::Word, false, true, 0x00000000 }; // Channel A FIFO, Data 0-3
//     IORegister FIFO_B    { 0x040000A4, AccessSize::Word, false, true, 0x00000000 }; // Channel B FIFO, Data 0-3
// };

// struct IO_DMARegisters
// {
//     IORegister DMA0SAD   { 0x040000B0, AccessSize::Word, false, true, 0x00000000 }; // DMA 0 Source Address
//     IORegister DMA0DAD   { 0x040000B4, AccessSize::Word, false, true, 0x00000000 }; // DMA 0 Destination Address
//     IORegister DMA0CNT_L { 0x040000B8, AccessSize::Halfword, false, true, 0x0000 }; // DMA 0 Word Count
//     IORegister DMA0CNT_H { 0x040000BA, AccessSize::Halfword, true, true, 0x0000 }; // DMA 0 Control

//     IORegister DMA1SAD   { 0x040000BC, AccessSize::Word, false, true, 0x00000000 }; // DMA 1 Source Address
//     IORegister DMA1DAD   { 0x040000C0, AccessSize::Word, false, true, 0x00000000 }; // DMA 1 Destination Address
//     IORegister DMA1CNT_L { 0x040000C4, AccessSize::Halfword, false, true, 0x0000 }; // DMA 1 Word Count
//     IORegister DMA1CNT_H { 0x040000C6, AccessSize::Halfword, true, true, 0x0000 }; // DMA 1 Control

//     IORegister DMA2SAD   { 0x040000C8, AccessSize::Word, false, true, 0x00000000 }; // DMA 2 Source Address
//     IORegister DMA2DAD   { 0x040000CC, AccessSize::Word, false, true, 0x00000000 }; // DMA 2 Destination Address
//     IORegister DMA2CNT_L { 0x040000D0, AccessSize::Halfword, false, true, 0x0000 }; // DMA 2 Word Count
//     IORegister DMA2CNT_H { 0x040000D2, AccessSize::Halfword, true, true, 0x0000 }; // DMA 2 Control

//     IORegister DMA3SAD   { 0x040000D4, AccessSize::Word, false, true, 0x00000000 }; // DMA 3 Source Address
//     IORegister DMA3DAD   { 0x040000D8, AccessSize::Word, false, true, 0x00000000 }; // DMA 3 Destination Address
//     IORegister DMA3CNT_L { 0x040000DC, AccessSize::Halfword, false, true, 0x0000 }; // DMA 3 Word Count
//     IORegister DMA3CNT_H { 0x040000DE, AccessSize::Halfword, true, true, 0x0000 }; // DMA 3 Control
// };

// struct IO_TimerRegisters
// {
//     IORegister TM0CNT_L { 0x04000100, AccessSize::Halfword, false, true, 0x0000 }; // Timer 0 Counter/Reload
//     IORegister TM0CNT_H { 0x04000102, AccessSize::Halfword, true, true, 0x0000 }; // Timer 0 Control

//     IORegister TM1CNT_L { 0x04000104, AccessSize::Halfword, false, true, 0x0000 }; // Timer 1 Counter/Reload
//     IORegister TM1CNT_H { 0x04000106, AccessSize::Halfword, true, true, 0x0000 }; // Timer 1 Control

//     IORegister TM2CNT_L { 0x04000108, AccessSize::Halfword, false, true, 0x0000 }; // Timer 2 Counter/Reload
//     IORegister TM2CNT_H { 0x0400010A, AccessSize::Halfword, true, true, 0x0000 }; // Timer 2 Control

//     IORegister TM3CNT_L { 0x0400010C, AccessSize::Halfword, false, true, 0x0000 }; // Timer 3 Counter/Reload
//     IORegister TM3CNT_H { 0x0400010E, AccessSize::Halfword, true, true, 0x0000 }; // Timer 3 Control
// };

// struct IO_SerialRegisters
// {
//     // ===============================
//     // Serial Communication (1)
//     // 0x0400120 → 0x040012C
//     // ===============================

//     // Normal 32-bit mode (shared with multiplayer)
//     IORegister SIODATA32 {0x04000120, AccessSize::Word, true, true, 0x00000000};

//     // Multiplayer mode (2-byte halfword registers)
//     IORegister SIOMULTI0 {0x04000120, AccessSize::Halfword, true, true, 0x0000}; // Parent
//     IORegister SIOMULTI1 {0x04000122, AccessSize::Halfword, true, true, 0x0000}; // 1st child
//     IORegister SIOMULTI2 {0x04000124, AccessSize::Halfword, true, true, 0x0000}; // 2nd child
//     IORegister SIOMULTI3 {0x04000126, AccessSize::Halfword, true, true, 0x0000}; // 3rd child

//     // Control register
//     IORegister SIOCNT {0x04000128, AccessSize::Halfword, true, true, 0x0000};

//     // Local multiplayer send / 8-bit UART
//     IORegister SIOMLT_SEND {0x0400012A, AccessSize::Halfword, false, true, 0x0000};
//     IORegister SIODATA8    {0x0400012A, AccessSize::Byte, true, true, 0x00};

//     // ===============================
//     // Serial Communication (2)
//     // 0x0400134 → 0x040015A
//     // ===============================

//     IORegister RCNT {0x04000134, AccessSize::Halfword, true, true, 0x0000};

//     // Ancient infrared register (prototypes only)
//     IORegister IR {0x04000136, AccessSize::Halfword, false, false, 0x0000};

//     // SIO JOY Control
//     IORegister JOYCNT {0x04000140, AccessSize::Halfword, true, true, 0x0000};
//     IORegister JOY_RECV {0x04000150, AccessSize::Word, true, true, 0x00000000};
//     IORegister JOY_TRANS {0x04000154, AccessSize::Word, true, true, 0x00000000};
//     IORegister JOYSTAT {0x04000158, AccessSize::Halfword, true, false, 0x0000};
// };

// struct IO_KeypadRegisters
// {
//     IORegister KEYINPUT { 0x04000130, AccessSize::Halfword, true, false, 0x03FF }; // Key Status (Read-only)
//     IORegister KEYCNT   { 0x04000132, AccessSize::Halfword, true, true, 0x0000 };  // Key Interrupt Control
// };

// struct IO_InterruptRegisters
// {
//     IORegister IE  { 0x04000200, AccessSize::Halfword, true, true, 0x0000 }; // Interrupt Enable Register
//     IORegister IF  { 0x04000202, AccessSize::Halfword, true, true, 0x0000 }; // Interrupt Request Flags / IRQ Acknowledge
//     IORegister IME { 0x04000208, AccessSize::Halfword, true, true, 0x0000 }; // Interrupt Master Enable Register
// };

// struct IO_MiscRegisters
// {
//     IORegister WAITCNT { 0x04000204, AccessSize::Halfword, true, true, 0x0000 }; // Game Pak Waitstate Control
//     IORegister POSTFLG { 0x04000300, AccessSize::Byte, true, true, 0x80 };     // Undocumented - Post Boot Flag
//     IORegister HALTCNT { 0x04000301, AccessSize::Byte, false, true, 0x00 };    // Undocumented - Power Down Control
// };
