#pragma once
#include "Core/GBA_Memory_Helpers.hpp"

#include "Utils/Integers.hpp"

#include <string>
#include <functional>

// TODO: probably not needed
// constexpr u32 LCD_END     = 0x04000060;
// constexpr u32 SOUND_END   = 0x040000B0;
// constexpr u32 DMA_END     = 0x04000100;
// constexpr u32 TIMER_END   = 0x04000120;
// constexpr u32 SIO_END     = 0x04000130;
// constexpr u32 KEYPAD_END  = 0x04000134;
// constexpr u32 IRQ_END     = 0x04000300;

using ReadHandler  = std::function<u32(u32 busAddress)>;
using WriteHandler = std::function<void(u32 busAddress, u32 value)>;

struct IORegister
{
    u32 address;
    size_t width;
    bool readable;
    bool writeable;
    u32 value = 0;

    ReadHandler onRead = nullptr;
    WriteHandler onWrite = nullptr;
};

struct IO_LCDRegisters
{
    IORegister DISPCNT   { 0x04000000, size_t::Halfword, true, true, 0x0080 }; // LCD Control
    IORegister GREENSWAP { 0x04000002, size_t::Halfword, true, true, 0x0000 }; // Undocumented - Green Swap
    IORegister DISPSTAT  { 0x04000004, size_t::Halfword, true, true, 0x0000 }; // General LCD Status (STAT, LYC)
    IORegister VCOUNT    { 0x04000006, size_t::Halfword, true, false, 0x0000 }; // Vertical Counter (LY) (Read-only)

    IORegister BG0CNT { 0x04000008, size_t::Halfword, true, true, 0x0000 }; // BG0 Control
    IORegister BG1CNT { 0x0400000A, size_t::Halfword, true, true, 0x0000 }; // BG1 Control
    IORegister BG2CNT { 0x0400000C, size_t::Halfword, true, true, 0x0000 }; // BG2 Control
    IORegister BG3CNT { 0x0400000E, size_t::Halfword, true, true, 0x0000 }; // BG3 Control

    IORegister BG0HOFS { 0x04000010, size_t::Halfword, false, true, 0x0000 }; // BG0 X-Offset (Write-only)
    IORegister BG0VOFS { 0x04000012, size_t::Halfword, false, true, 0x0000 }; // BG0 Y-Offset (Write-only)
    IORegister BG1HOFS { 0x04000014, size_t::Halfword, false, true, 0x0000 }; // BG1 X-Offset (Write-only)
    IORegister BG1VOFS { 0x04000016, size_t::Halfword, false, true, 0x0000 }; // BG1 Y-Offset (Write-only)
    IORegister BG2HOFS { 0x04000018, size_t::Halfword, false, true, 0x0000 }; // BG2 X-Offset (Write-only)
    IORegister BG2VOFS { 0x0400001A, size_t::Halfword, false, true, 0x0000 }; // BG2 Y-Offset (Write-only)
    IORegister BG3HOFS { 0x0400001C, size_t::Halfword, false, true, 0x0000 }; // BG3 X-Offset (Write-only)
    IORegister BG3VOFS { 0x0400001E, size_t::Halfword, false, true, 0x0000 }; // BG3 Y-Offset (Write-only)

    IORegister BG2PA { 0x04000020, size_t::Halfword, false, true, 0x0100 }; // BG2 Rotation/Scaling Parameter A (dx)
    IORegister BG2PB { 0x04000022, size_t::Halfword, false, true, 0x0000 }; // BG2 Rotation/Scaling Parameter B (dmx)
    IORegister BG2PC { 0x04000024, size_t::Halfword, false, true, 0x0000 }; // BG2 Rotation/Scaling Parameter C (dy)
    IORegister BG2PD { 0x04000026, size_t::Halfword, false, true, 0x0100 }; // BG2 Rotation/Scaling Parameter D (dmy)

    IORegister BG2X { 0x04000028, size_t::Word, false, true, 0x00000000 }; // BG2 Reference Point X-Coordinate
    IORegister BG2Y { 0x0400002C, size_t::Word, false, true, 0x00000000 }; // BG2 Reference Point Y-Coordinate

    IORegister BG3PA { 0x04000030, size_t::Halfword, false, true, 0x0100 }; // BG3 Rotation/Scaling Parameter A (dx)
    IORegister BG3PB { 0x04000032, size_t::Halfword, false, true, 0x0000 }; // BG3 Rotation/Scaling Parameter B (dmx)
    IORegister BG3PC { 0x04000034, size_t::Halfword, false, true, 0x0000 }; // BG3 Rotation/Scaling Parameter C (dy)
    IORegister BG3PD { 0x04000036, size_t::Halfword, false, true, 0x0100 }; // BG3 Rotation/Scaling Parameter D (dmy)

    IORegister BG3X { 0x04000038, size_t::Word, false, true, 0x00000000 }; // BG3 Reference Point X-Coordinate
    IORegister BG3Y { 0x0400003C, size_t::Word, false, true, 0x00000000 }; // BG3 Reference Point Y-Coordinate

    IORegister WIN0H { 0x04000040, size_t::Halfword, false, true, 0x0000 }; // Window 0 Horizontal Dimensions
    IORegister WIN1H { 0x04000042, size_t::Halfword, false, true, 0x0000 }; // Window 1 Horizontal Dimensions
    IORegister WIN0V { 0x04000044, size_t::Halfword, false, true, 0x0000 }; // Window 0 Vertical Dimensions
    IORegister WIN1V { 0x04000046, size_t::Halfword, false, true, 0x0000 }; // Window 1 Vertical Dimensions

    IORegister WININ  { 0x04000048, size_t::Halfword, true, true, 0x0000 }; // Inside of Window 0 and 1
    IORegister WINOUT { 0x0400004A, size_t::Halfword, true, true, 0x0000 }; // Inside of OBJ Window & Outside of Windows

    IORegister MOSAIC { 0x0400004C, size_t::Halfword, false, true, 0x0000 }; // Mosaic Size

    IORegister BLDCNT  { 0x04000050, size_t::Halfword, true, true, 0x0000 }; // Color Special Effects Selection
    IORegister BLDALPHA { 0x04000052, size_t::Halfword, true, true, 0x0000 }; // Alpha Blending Coefficients
    IORegister BLDY     { 0x04000054, size_t::Halfword, false, true, 0x0000 }; // Brightness (Fade-In/Out) Coefficient (Write-only)
};

struct IO_SoundRegisters
{
    IORegister SOUND1CNT_L { 0x04000060, size_t::Halfword, true, true, 0x0000 }; // Channel 1 Sweep register (NR10)
    IORegister SOUND1CNT_H { 0x04000062, size_t::Halfword, true, true, 0x0000 }; // Channel 1 Duty/Length/Envelope (NR11, NR12)
    IORegister SOUND1CNT_X { 0x04000064, size_t::Halfword, true, true, 0x0000 }; // Channel 1 Frequency/Control (NR13, NR14)

    IORegister SOUND2CNT_L { 0x04000068, size_t::Halfword, true, true, 0x0000 }; // Channel 2 Duty/Length/Envelope (NR21, NR22)
    IORegister SOUND2CNT_H { 0x0400006C, size_t::Halfword, true, true, 0x0000 }; // Channel 2 Frequency/Control (NR23, NR24)

    IORegister SOUND3CNT_L { 0x04000070, size_t::Halfword, true, true, 0x0000 }; // Channel 3 Stop/Wave RAM select (NR30)
    IORegister SOUND3CNT_H { 0x04000072, size_t::Halfword, true, true, 0x0000 }; // Channel 3 Length/Volume (NR31, NR32)
    IORegister SOUND3CNT_X { 0x04000074, size_t::Halfword, true, true, 0x0000 }; // Channel 3 Frequency/Control (NR33, NR34)

    IORegister SOUND4CNT_L { 0x04000078, size_t::Halfword, true, true, 0x0000 }; // Channel 4 Length/Envelope (NR41, NR42)
    IORegister SOUND4CNT_H { 0x0400007C, size_t::Halfword, true, true, 0x0000 }; // Channel 4 Frequency/Control (NR43, NR44)

    IORegister SOUNDCNT_L { 0x04000080, size_t::Halfword, true, true, 0x0080 }; // Control Stereo/Volume/Enable (NR50, NR51)
    IORegister SOUNDCNT_H { 0x04000082, size_t::Halfword, true, true, 0x0000 }; // Control Mixing/DMA Control
    IORegister SOUNDCNT_X { 0x04000084, size_t::Halfword, true, true, 0x0080 }; // Control Sound on/off (NR52)

    IORegister SOUNDBIAS { 0x04000088, size_t::Halfword, true, true, 0x0200 }; // Sound PWM Control (BIOS)
    IORegister WAVE_RAM  { 0x04000090, size_t::Word, true, true, 0x00000000 }; // Channel 3 Wave Pattern RAM (2 banks!!)

    IORegister FIFO_A    { 0x040000A0, size_t::Word, false, true, 0x00000000 }; // Channel A FIFO, Data 0-3
    IORegister FIFO_B    { 0x040000A4, size_t::Word, false, true, 0x00000000 }; // Channel B FIFO, Data 0-3
};

struct IO_DMARegisters
{
    IORegister DMA0SAD   { 0x040000B0, size_t::Word, false, true, 0x00000000 }; // DMA 0 Source Address
    IORegister DMA0DAD   { 0x040000B4, size_t::Word, false, true, 0x00000000 }; // DMA 0 Destination Address
    IORegister DMA0CNT_L { 0x040000B8, size_t::Halfword, false, true, 0x0000 }; // DMA 0 Word Count
    IORegister DMA0CNT_H { 0x040000BA, size_t::Halfword, true, true, 0x0000 }; // DMA 0 Control

    IORegister DMA1SAD   { 0x040000BC, size_t::Word, false, true, 0x00000000 }; // DMA 1 Source Address
    IORegister DMA1DAD   { 0x040000C0, size_t::Word, false, true, 0x00000000 }; // DMA 1 Destination Address
    IORegister DMA1CNT_L { 0x040000C4, size_t::Halfword, false, true, 0x0000 }; // DMA 1 Word Count
    IORegister DMA1CNT_H { 0x040000C6, size_t::Halfword, true, true, 0x0000 }; // DMA 1 Control

    IORegister DMA2SAD   { 0x040000C8, size_t::Word, false, true, 0x00000000 }; // DMA 2 Source Address
    IORegister DMA2DAD   { 0x040000CC, size_t::Word, false, true, 0x00000000 }; // DMA 2 Destination Address
    IORegister DMA2CNT_L { 0x040000D0, size_t::Halfword, false, true, 0x0000 }; // DMA 2 Word Count
    IORegister DMA2CNT_H { 0x040000D2, size_t::Halfword, true, true, 0x0000 }; // DMA 2 Control

    IORegister DMA3SAD   { 0x040000D4, size_t::Word, false, true, 0x00000000 }; // DMA 3 Source Address
    IORegister DMA3DAD   { 0x040000D8, size_t::Word, false, true, 0x00000000 }; // DMA 3 Destination Address
    IORegister DMA3CNT_L { 0x040000DC, size_t::Halfword, false, true, 0x0000 }; // DMA 3 Word Count
    IORegister DMA3CNT_H { 0x040000DE, size_t::Halfword, true, true, 0x0000 }; // DMA 3 Control
};

struct IO_TimerRegisters
{
    IORegister TM0CNT_L { 0x04000100, size_t::Halfword, false, true, 0x0000 }; // Timer 0 Counter/Reload
    IORegister TM0CNT_H { 0x04000102, size_t::Halfword, true, true, 0x0000 }; // Timer 0 Control

    IORegister TM1CNT_L { 0x04000104, size_t::Halfword, false, true, 0x0000 }; // Timer 1 Counter/Reload
    IORegister TM1CNT_H { 0x04000106, size_t::Halfword, true, true, 0x0000 }; // Timer 1 Control

    IORegister TM2CNT_L { 0x04000108, size_t::Halfword, false, true, 0x0000 }; // Timer 2 Counter/Reload
    IORegister TM2CNT_H { 0x0400010A, size_t::Halfword, true, true, 0x0000 }; // Timer 2 Control

    IORegister TM3CNT_L { 0x0400010C, size_t::Halfword, false, true, 0x0000 }; // Timer 3 Counter/Reload
    IORegister TM3CNT_H { 0x0400010E, size_t::Halfword, true, true, 0x0000 }; // Timer 3 Control
};

struct IO_SerialRegisters
{
    // ===============================
    // Serial Communication (1)
    // 0x0400120 → 0x040012C
    // ===============================

    // Normal 32-bit mode (shared with multiplayer)
    IORegister SIODATA32 {0x04000120, size_t::Word, true, true, 0x00000000};

    // Multiplayer mode (2-byte halfword registers)
    IORegister SIOMULTI0 {0x04000120, size_t::Halfword, true, true, 0x0000}; // Parent
    IORegister SIOMULTI1 {0x04000122, size_t::Halfword, true, true, 0x0000}; // 1st child
    IORegister SIOMULTI2 {0x04000124, size_t::Halfword, true, true, 0x0000}; // 2nd child
    IORegister SIOMULTI3 {0x04000126, size_t::Halfword, true, true, 0x0000}; // 3rd child

    // Control register
    IORegister SIOCNT {0x04000128, size_t::Halfword, true, true, 0x0000};

    // Local multiplayer send / 8-bit UART
    IORegister SIOMLT_SEND {0x0400012A, size_t::Halfword, false, true, 0x0000};
    IORegister SIODATA8    {0x0400012A, size_t::Byte, true, true, 0x00};

    // ===============================
    // Serial Communication (2)
    // 0x0400134 → 0x040015A
    // ===============================

    IORegister RCNT {0x04000134, size_t::Halfword, true, true, 0x0000};

    // Ancient infrared register (prototypes only)
    IORegister IR {0x04000136, size_t::Halfword, false, false, 0x0000};

    // SIO JOY Control
    IORegister JOYCNT {0x04000140, size_t::Halfword, true, true, 0x0000};
    IORegister JOY_RECV {0x04000150, size_t::Word, true, true, 0x00000000};
    IORegister JOY_TRANS {0x04000154, size_t::Word, true, true, 0x00000000};
    IORegister JOYSTAT {0x04000158, size_t::Halfword, true, false, 0x0000};
};

struct IO_KeypadRegisters
{
    IORegister KEYINPUT { 0x04000130, size_t::Halfword, true, false, 0x03FF }; // Key Status (Read-only)
    IORegister KEYCNT   { 0x04000132, size_t::Halfword, true, true, 0x0000 };  // Key Interrupt Control
};

struct IO_InterruptRegisters
{
    IORegister IE  { 0x04000200, size_t::Halfword, true, true, 0x0000 }; // Interrupt Enable Register
    IORegister IF  { 0x04000202, size_t::Halfword, true, true, 0x0000 }; // Interrupt Request Flags / IRQ Acknowledge
    IORegister IME { 0x04000208, size_t::Halfword, true, true, 0x0000 }; // Interrupt Master Enable Register
};

struct IO_MiscRegisters
{
    IORegister WAITCNT { 0x04000204, size_t::Halfword, true, true, 0x0000 }; // Game Pak Waitstate Control
    IORegister POSTFLG { 0x04000300, size_t::Byte, true, true, 0x80 };     // Undocumented - Post Boot Flag
    IORegister HALTCNT { 0x04000301, size_t::Byte, false, true, 0x00 };    // Undocumented - Power Down Control
};
