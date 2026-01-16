#include <cstdint>

constexpr uint32_t LCD_END     = 0x04000060;
constexpr uint32_t SOUND_END   = 0x040000B0;
constexpr uint32_t DMA_END     = 0x04000100;
constexpr uint32_t TIMER_END   = 0x04000120;
constexpr uint32_t SIO_END     = 0x04000130;
constexpr uint32_t KEYPAD_END  = 0x04000134;
constexpr uint32_t IRQ_END     = 0x04000300;


struct LCDIORegisters
{
    // 0x04000000
    uint16_t DISPCNT    = 0; // LCD Control

    // 0x04000002
    uint16_t GREENSWAP = 0; // Undocumented - Green Swap

    // 0x04000004
    uint16_t DISPSTAT  = 0; // General LCD Status (STAT, LYC)

    // 0x04000006
    uint16_t VCOUNT    = 0; // Vertical Counter (LY) (Read-only)

    // 0x04000008
    uint16_t BG0CNT    = 0; // BG0 Control
    uint16_t BG1CNT    = 0; // BG1 Control
    uint16_t BG2CNT    = 0; // BG2 Control
    uint16_t BG3CNT    = 0; // BG3 Control

    // 0x04000010
    uint16_t BG0HOFS   = 0; // BG0 X-Offset (Write-only)
    uint16_t BG0VOFS   = 0; // BG0 Y-Offset (Write-only)
    uint16_t BG1HOFS   = 0; // BG1 X-Offset (Write-only)
    uint16_t BG1VOFS   = 0; // BG1 Y-Offset (Write-only)
    uint16_t BG2HOFS   = 0; // BG2 X-Offset (Write-only)
    uint16_t BG2VOFS   = 0; // BG2 Y-Offset (Write-only)
    uint16_t BG3HOFS   = 0; // BG3 X-Offset (Write-only)
    uint16_t BG3VOFS   = 0; // BG3 Y-Offset (Write-only)

    // 0x04000020
    uint16_t BG2PA     = 0; // BG2 Rotation/Scaling Parameter A (dx)
    uint16_t BG2PB     = 0; // BG2 Rotation/Scaling Parameter B (dmx)
    uint16_t BG2PC     = 0; // BG2 Rotation/Scaling Parameter C (dy)
    uint16_t BG2PD     = 0; // BG2 Rotation/Scaling Parameter D (dmy)

    // 0x04000028
    uint32_t BG2X      = 0; // BG2 Reference Point X-Coordinate
    uint32_t BG2Y      = 0; // BG2 Reference Point Y-Coordinate

    // 0x04000030
    uint16_t BG3PA     = 0; // BG3 Rotation/Scaling Parameter A (dx)
    uint16_t BG3PB     = 0; // BG3 Rotation/Scaling Parameter B (dmx)
    uint16_t BG3PC     = 0; // BG3 Rotation/Scaling Parameter C (dy)
    uint16_t BG3PD     = 0; // BG3 Rotation/Scaling Parameter D (dmy)

    // 0x04000038
    uint32_t BG3X      = 0; // BG3 Reference Point X-Coordinate
    uint32_t BG3Y      = 0; // BG3 Reference Point Y-Coordinate

    // 0x04000040
    uint16_t WIN0H     = 0; // Window 0 Horizontal Dimensions
    uint16_t WIN1H     = 0; // Window 1 Horizontal Dimensions
    uint16_t WIN0V     = 0; // Window 0 Vertical Dimensions
    uint16_t WIN1V     = 0; // Window 1 Vertical Dimensions

    // 0x04000048
    uint16_t WININ     = 0; // Inside of Window 0 and 1
    uint16_t WINOUT    = 0; // Inside of OBJ Window & Outside of Windows

    // 0x0400004C
    uint16_t MOSAIC    = 0; // Mosaic Size

    // 0x04000050
    uint16_t BLDCNT    = 0; // Color Special Effects Selection
    uint16_t BLDALPHA  = 0; // Alpha Blending Coefficients
    uint16_t BLDY      = 0; // Brightness (Fade-In/Out) Coefficient (Write-only)
};