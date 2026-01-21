#include <cstdint>
#include <string>
#include "Core/GBA_Memory_Helpers.hpp"

constexpr uint32_t LCD_END     = 0x04000060;
constexpr uint32_t SOUND_END   = 0x040000B0;
constexpr uint32_t DMA_END     = 0x04000100;
constexpr uint32_t TIMER_END   = 0x04000120;
constexpr uint32_t SIO_END     = 0x04000130;
constexpr uint32_t KEYPAD_END  = 0x04000134;
constexpr uint32_t IRQ_END     = 0x04000300;

struct IORegister
{
    uint32_t address;
    BusAccessSize width;
    bool readable;
    bool writeable;
};

struct IO_LCDRegisters
{
    IORegister DISPCNT { 0x04000000, BusAccessSize::Halfword, true, true }; // LCD Control
    IORegister GREENSWAP { 0x04000002, BusAccessSize::Halfword, true, true }; // Undocumented - Green Swap
    IORegister DISPSTAT { 0x04000004, BusAccessSize::Halfword, true, true }; // General LCD Status (STAT, LYC)
    IORegister VCOUNT { 0x04000006, BusAccessSize::Halfword, true, false }; // Vertical Counter (LY) (Read-only)

    IORegister BG0CNT { 0x04000008, BusAccessSize::Halfword, true, true }; // BG0 Control
    IORegister BG1CNT { 0x0400000A, BusAccessSize::Halfword, true, true }; // BG1 Control
    IORegister BG2CNT { 0x0400000C, BusAccessSize::Halfword, true, true }; // BG2 Control
    IORegister BG3CNT { 0x0400000E, BusAccessSize::Halfword, true, true }; // BG3 Control

    IORegister BG0HOFS { 0x04000010, BusAccessSize::Halfword, false, true }; // BG0 X-Offset (Write-only)
    IORegister BG0VOFS { 0x04000012, BusAccessSize::Halfword, false, true }; // BG0 Y-Offset (Write-only)
    IORegister BG1HOFS { 0x04000014, BusAccessSize::Halfword, false, true }; // BG1 X-Offset (Write-only)
    IORegister BG1VOFS { 0x04000016, BusAccessSize::Halfword, false, true }; // BG1 Y-Offset (Write-only)
    IORegister BG2HOFS { 0x04000018, BusAccessSize::Halfword, false, true }; // BG2 X-Offset (Write-only)
    IORegister BG2VOFS { 0x0400001A, BusAccessSize::Halfword, false, true }; // BG2 Y-Offset (Write-only)
    IORegister BG3HOFS { 0x0400001C, BusAccessSize::Halfword, false, true }; // BG3 X-Offset (Write-only)
    IORegister BG3VOFS { 0x0400001E, BusAccessSize::Halfword, false, true }; // BG3 Y-Offset (Write-only)

    IORegister BG2PA { 0x04000020, BusAccessSize::Halfword, false, true }; // BG2 Rotation/Scaling Parameter A (dx)
    IORegister BG2PB { 0x04000022, BusAccessSize::Halfword, false, true }; // BG2 Rotation/Scaling Parameter B (dmx)
    IORegister BG2PC { 0x04000024, BusAccessSize::Halfword, false, true }; // BG2 Rotation/Scaling Parameter C (dy)
    IORegister BG2PD { 0x04000026, BusAccessSize::Halfword, false, true }; // BG2 Rotation/Scaling Parameter D (dmy)

    IORegister BG2X { 0x04000028, BusAccessSize::Word, false, true }; // BG2 Reference Point X-Coordinate
    IORegister BG2Y { 0x0400002C, BusAccessSize::Word, false, true }; // BG2 Reference Point Y-Coordinate

    IORegister BG3PA { 0x04000030, BusAccessSize::Halfword, false, true }; // BG3 Rotation/Scaling Parameter A (dx)
    IORegister BG3PB { 0x04000032, BusAccessSize::Halfword, false, true }; // BG3 Rotation/Scaling Parameter B (dmx)
    IORegister BG3PC { 0x04000034, BusAccessSize::Halfword, false, true }; // BG3 Rotation/Scaling Parameter C (dy)
    IORegister BG3PD { 0x04000036, BusAccessSize::Halfword, false, true }; // BG3 Rotation/Scaling Parameter D (dmy)

    IORegister BG3X { 0x04000038, BusAccessSize::Word, false, true }; // BG3 Reference Point X-Coordinate
    IORegister BG3Y { 0x0400003C, BusAccessSize::Word, false, true }; // BG3 Reference Point Y-Coordinate

    IORegister WIN0H { 0x04000040, BusAccessSize::Halfword, false, true }; // Window 0 Horizontal Dimensions
    IORegister WIN1H { 0x04000042, BusAccessSize::Halfword, false, true }; // Window 1 Horizontal Dimensions
    IORegister WIN0V { 0x04000044, BusAccessSize::Halfword, false, true }; // Window 0 Vertical Dimensions
    IORegister WIN1V { 0x04000046, BusAccessSize::Halfword, false, true }; // Window 1 Vertical Dimensions

    IORegister WININ { 0x04000048, BusAccessSize::Halfword, true, true }; // Inside of Window 0 and 1
    IORegister WINOUT { 0x0400004A, BusAccessSize::Halfword, true, true }; // Inside of OBJ Window & Outside of Windows

    IORegister MOSAIC { 0x0400004C, BusAccessSize::Halfword, false, true }; // Mosaic Size

    IORegister BLDCNT { 0x04000050, BusAccessSize::Halfword, true, true }; // Color Special Effects Selection
    IORegister BLDALPHA { 0x04000052, BusAccessSize::Halfword, true, true }; // Alpha Blending Coefficients
    IORegister BLDY { 0x04000054, BusAccessSize::Halfword, false, true }; // Brightness (Fade-In/Out) Coefficient (Write-only)
};

struct IO_SoundRegisters
{
    IORegister SOUND1CNT_L { 0x04000060, BusAccessSize::Halfword, true, true }; // Channel 1 Sweep register (NR10)
    IORegister SOUND1CNT_H { 0x04000062, BusAccessSize::Halfword, true, true }; // Channel 1 Duty/Length/Envelope (NR11, NR12)
    IORegister SOUND1CNT_X { 0x04000064, BusAccessSize::Halfword, true, true }; // Channel 1 Frequency/Control (NR13, NR14)
    IORegister SOUND2CNT_L { 0x04000068, BusAccessSize::Halfword, true, true }; // Channel 2 Duty/Length/Envelope (NR21, NR22)
    IORegister SOUND2CNT_H { 0x0400006C, BusAccessSize::Halfword, true, true }; // Channel 2 Frequency/Control (NR23, NR24)
    IORegister SOUND3CNT_L { 0x04000070, BusAccessSize::Halfword, true, true }; // Channel 3 Stop/Wave RAM select (NR30)
    IORegister SOUND3CNT_H { 0x04000072, BusAccessSize::Halfword, true, true }; // Channel 3 Length/Volume (NR31, NR32)
    IORegister SOUND3CNT_X { 0x04000074, BusAccessSize::Halfword, true, true }; // Channel 3 Frequency/Control (NR33, NR34)
    IORegister SOUND4CNT_L { 0x04000078, BusAccessSize::Halfword, true, true }; // Channel 4 Length/Envelope (NR41, NR42)
    IORegister SOUND4CNT_H { 0x0400007C, BusAccessSize::Halfword, true, true }; // Channel 4 Frequency/Control (NR43, NR44)
    IORegister SOUNDCNT_L { 0x04000080, BusAccessSize::Halfword, true, true }; // Control Stereo/Volume/Enable (NR50, NR51)
    IORegister SOUNDCNT_H { 0x04000082, BusAccessSize::Halfword, true, true }; // Control Mixing/DMA Control
    IORegister SOUNDCNT_X { 0x04000084, BusAccessSize::Halfword, true, true }; // Control Sound on/off (NR52)
    IORegister SOUNDBIAS { 0x04000088, BusAccessSize::Halfword, true, true }; // Sound PWM Control (BIOS)
    IORegister WAVE_RAM { 0x04000090, BusAccessSize::Word, true, true }; // Channel 3 Wave Pattern RAM (2 banks!!)
    IORegister FIFO_A { 0x040000A0, BusAccessSize::Word, false, true }; // Channel A FIFO, Data 0-3
    IORegister FIFO_B { 0x040000A4, BusAccessSize::Word, false, true }; // Channel B FIFO, Data 0-3
};

struct IO_DMARegisters
{
    IORegister DMA0SAD { 0x04000B0, BusAccessSize::Word, false, true }; // DMA 0 Source Address
    IORegister DMA0DAD { 0x04000B4, BusAccessSize::Word, false, true }; // DMA 0 Destination Address
    IORegister DMA0CNT_L { 0x04000B8, BusAccessSize::Halfword, false, true }; // DMA 0 Word Count
    IORegister DMA0CNT_H { 0x04000BA, BusAccessSize::Halfword, true, true }; // DMA 0 Control
    IORegister DMA1SAD { 0x04000BC, BusAccessSize::Word, false, true }; // DMA 1 Source Address
    IORegister DMA1DAD { 0x04000C0, BusAccessSize::Word, false, true }; // DMA 1 Destination Address
    IORegister DMA1CNT_L { 0x04000C4, BusAccessSize::Halfword, false, true }; // DMA 1 Word Count
    IORegister DMA1CNT_H { 0x04000C6, BusAccessSize::Halfword, true, true }; // DMA 1 Control
    IORegister DMA2SAD { 0x04000C8, BusAccessSize::Word, false, true }; // DMA 2 Source Address
    IORegister DMA2DAD { 0x04000CC, BusAccessSize::Word, false, true }; // DMA 2 Destination Address
    IORegister DMA2CNT_L { 0x04000D0, BusAccessSize::Halfword, false, true }; // DMA 2 Word Count
    IORegister DMA2CNT_H { 0x04000D2, BusAccessSize::Halfword, true, true }; // DMA 2 Control
    IORegister DMA3SAD { 0x04000D4, BusAccessSize::Word, false, true }; // DMA 3 Source Address
    IORegister DMA3DAD { 0x04000D8, BusAccessSize::Word, false, true }; // DMA 3 Destination Address
    IORegister DMA3CNT_L { 0x04000DC, BusAccessSize::Halfword, false, true }; // DMA 3 Word Count
    IORegister DMA3CNT_H { 0x04000DE, BusAccessSize::Halfword, true, true }; // DMA 3 Control
};

struct IO_TimerRegisters
{
    IORegister TM0CNT_L { 0x0400100, BusAccessSize::Halfword, false, true }; // Timer 0 Counter/Reload
    IORegister TM0CNT_H { 0x0400102, BusAccessSize::Halfword, true, true }; // Timer 0 Control
    IORegister TM1CNT_L { 0x0400104, BusAccessSize::Halfword, false, true }; // Timer 1 Counter/Reload
    IORegister TM1CNT_H { 0x0400106, BusAccessSize::Halfword, true, true }; // Timer 1 Control
    IORegister TM2CNT_L { 0x0400108, BusAccessSize::Halfword, false, true }; // Timer 2 Counter/Reload
    IORegister TM2CNT_H { 0x040010A, BusAccessSize::Halfword, true, true }; // Timer 2 Control
    IORegister TM3CNT_L { 0x040010C, BusAccessSize::Halfword, false, true }; // Timer 3 Counter/Reload
    IORegister TM3CNT_H { 0x040010E, BusAccessSize::Halfword, true, true }; // Timer 3 Control
};

struct IO_SerialRegisters
{
    // ===============================
    // Serial Communication (1)
    // 0x0400120 → 0x040012C
    // ===============================

    // Normal 32-bit mode (shared with multiplayer)
    IORegister SIODATA32 {0x0400120, BusAccessSize::Word, true, true};

    // Multiplayer mode (2-byte halfword registers)
    IORegister SIOMULTI0 {0x0400120, BusAccessSize::Halfword, true, true}; // Parent
    IORegister SIOMULTI1 {0x0400122, BusAccessSize::Halfword, true, true}; // 1st child
    IORegister SIOMULTI2 {0x0400124, BusAccessSize::Halfword, true, true}; // 2nd child
    IORegister SIOMULTI3 {0x0400126, BusAccessSize::Halfword, true, true}; // 3rd child

    // Control register
    IORegister SIOCNT {0x0400128, BusAccessSize::Halfword, true, true};

    // Local multiplayer send / 8-bit UART
    IORegister SIOMLT_SEND {0x040012A, BusAccessSize::Halfword, false, true};
    IORegister SIODATA8 {0x040012A, BusAccessSize::Byte, true, true};

    // ===============================
    // Serial Communication (2)
    // 0x0400134 → 0x040015A
    // ===============================

    IORegister RCNT {0x0400134, BusAccessSize::Halfword, true, true};

    // Ancient infrared register (prototypes only)
    IORegister IR {0x0400136, BusAccessSize::Halfword, false, false};

    // SIO JOY Control
    IORegister JOYCNT {0x0400140, BusAccessSize::Halfword, true, true};
    IORegister JOY_RECV {0x0400150, BusAccessSize::Word, true, true};
    IORegister JOY_TRANS {0x0400154, BusAccessSize::Word, true, true};
    IORegister JOYSTAT {0x0400158, BusAccessSize::Halfword, true, false};
};

struct IO_KeypadRegisters
{
    IORegister KEYINPUT { 0x0400130, BusAccessSize::Halfword, true, false }; // Key Status (Read-only)
    IORegister KEYCNT { 0x0400132, BusAccessSize::Halfword, true, true }; // Key Interrupt Control
};

struct IO_InterruptRegisters
{
    IORegister IE { 0x0400200, BusAccessSize::Halfword, true, true }; // Interrupt Enable Register
    IORegister IF { 0x0400202, BusAccessSize::Halfword, true, true }; // Interrupt Request Flags / IRQ Acknowledge
    IORegister IME { 0x0400208, BusAccessSize::Halfword, true, true }; // Interrupt Master Enable Register
};

struct IO_MiscRegisters
{
    IORegister WAITCNT { 0x0400204, BusAccessSize::Halfword, true, true }; // Game Pak Waitstate Control
    IORegister POSTFLG { 0x0400300, BusAccessSize::Byte, true, true }; // Undocumented - Post Boot Flag
    IORegister HALTCNT { 0x0400301, BusAccessSize::Byte, false, true }; // Undocumented - Power Down Control
};