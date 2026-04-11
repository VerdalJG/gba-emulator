#pragma once

#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Utils/Integers.hpp"

#include <array>

class GBA_PPU;

struct IO_LCDRegisters
{
    DisplayControl dispcnt;
    GreenSwap greenSwap;
    DisplayStatus dispstat;
    VerticalCounter vcount;
    std::array<BackgroundControl, 4> bgcnt = { 0, 1, 2, 3}; // ID's for each bgcnt

    std::array<BackgroundOffset, 4> bghofs; // Background X-offsets
    std::array<BackgroundOffset, 4> bgvofs; // Background Y-offsets

    BackgroundRefPointCoords bg2Coords; // BG2 Reference point Coordinates (low 16 bits and high 12 bits)
    BackgroundScalingParameters bg2Params; // BG2 Scaling params A-D
    
    BackgroundRefPointCoords bg3Coords; // BG3 Reference point Coordinates (low 16 bits and high 12 bits)
    BackgroundScalingParameters bg3Params; // BG3 Scaling params A-D

};

struct DisplayControl
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    u16 Read16() { return value; }

    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Write16(u16 value) { IO::Write16ByBytes(this->value, value, writeMask); }

    void Reset() { Write16(0); }
    void ResetToPostBIOSValue() { Write16(0x80); }

    union
    {
        struct 
        {
            u16 bgMode : 3; // 6 & 7 = prohibited
            u16 cgbMode : 1; // Can only be set by BIOS opcodes
            u16 frameSelect : 1; // For BG modes 4, 5 only
            u16 hBlankIntervalFree : 1; // 1 = Allow access to OAM during HBlank
            u16 objCharacterVramMapping : 1; // 0 = two dimensional, 1 = one dimensional
            u16 forcedBlank : 1; // 1 = Allow fast access to VRAM, Palette, OAM
            u16 screenDisplayBG : 4; // BG 0-3, 0 = off, 1 = on
            u16 screenDisplayOBJ : 1; 
            u16 windowDisplayFlag : 2;
            u16 objWindowDisplayFlag : 1;
        } fields;
        u16 value;
    };

    const u16 writeMask = 0xFFF7;
};

struct GreenSwap
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    u16 Read16() { return value; }

    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); } 
    void Write16(u16 value) { IO::Write16ByBytes(this->value, value, writeMask); }

    void Reset() { Write16(0); }

    union
    {
        struct 
        {
            u16 greenSwap : 1; // (0 = Normal, 1 = Swap)
            u16 unused : 15 = 0;
        } fields;
        u16 value;
    };

    const u16 writeMask = 0x1;
};

struct DisplayStatus
{
    u8 Read8(int byteToRead);
    u16 Read16();

    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); } 
    void Write16(u16 value) { IO::Write16ByBytes(this->value, value, writeMask); }
    
    void Reset() { Write16(0); }

    union
    {
        struct 
        {
            u16 vBlank : 1; // (Read only) (1=VBlank) (set in line 160..226; not 227)
            u16 hBlank : 1; // (Read only) (1=HBlank) (toggled in all lines, 0..227)
            u16 vCounter : 1; // (Read only) (1=Match)  (set in selected line)
            u16 vBlankIRQ : 1; // (1=Enable) (R/W)
            u16 hBlankIRQ : 1; // (1=Enable) (R/W)
            u16 vCounterIRQ : 1; // (1=Enable) (R/W)
            u16 unused : 2; // has NDS behavior on these 2 bits, unused in GBA
            u16 vCountSetting : 8; // (LYC) (0..227) (R/W)
        } fields;
        u16 value;

        /*
            Notes:
            The V-Count-Setting value is much the same as LYC of older gameboys, 
            when its value is identical to the content of the VCOUNT register then the
            V-Counter flag is set (Bit 2), and (if enabled in Bit 5) an interrupt is requested.

            Although the drawing time is only 960 cycles (240*4), the H-Blank flag is "0" for a total of 1006 cycles.
        */
    };

    const u16 writeMask = 0xFF38;
    GBA_PPU* ppu = nullptr;
};

struct VerticalCounter // Read-only
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    u16 Read16() { return value & 0xFF; } // Clear top byte

    void Reset() { value = 0;}

    union
    {
        struct 
        {
            u16 currentScanline : 8; // (LY) (0-227)
            u16 unused : 8 = 0; // Hardwired to 0
        } fields;
        u16 value;

        /*
            Indicates the currently drawn scanline, values in range from 160..227 indicate 'hidden' scanlines within VBlank
            Note: This is very similar to the 'LY' register of older gameboys.
        */
    };
};

struct BackgroundControl // R/W
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    u16 Read16() { return value; }

    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Write16(u16 value) { IO::Write16ByBytes(this->value, value, writeMask); }

    void Reset() { Write16(0); }

    BackgroundControl(int id) : id(id) {};

    const u16 writeMask = 0xFFCF;
    const int id;

    union
    {
        struct
        {
            u16 bgPriority : 2; // (0-3, 0=Highest) If equal priority then BG0 is the highest, and BG3 the lowest priority.
            u16 characterBaseBlock : 2; // (0-3, in units of 16 KBytes) (=BG Tile Data)
            u16 unused : 2 = 0; // (must be zero) (except in NDS mode: MSBs of char base)
            u16 mosaic : 1; // (0=Disable, 1=Enable)
            u16 colors : 1; // (0=16/16, 1=256/1)
            u16 screenBaseBlock : 5; // (0-31, in units of 2 KBytes) (=BG Map Data)
            u16 displayAreaOverflow : 1; // Not used in BG0/BG1 (unless NDS), for BG2/BG3: (0=Transparent, 1=Wraparound)
            u16 screenSize : 2; // (0-3)
        } fields;
        u16 value;

            /*
        Internal Screen Size (dots) and size of BG Map (bytes):
        Value  Text Mode      Rotation/Scaling Mode
        0      256x256 (2K)   128x128   (256 bytes)
        1      512x256 (4K)   256x256   (1K)
        2      256x512 (4K)   512x512   (4K)
        3      512x512 (8K)   1024x1024 (16K)
        
        In 'Text Modes', the screen size is organized as follows: 
        The screen consists of one or more 256x256 pixel (32x32 tiles) areas. 
        When Size=0: only 1 area (SC0), 
        when Size=1 or Size=2: two areas (SC0,SC1 either horizontally or vertically arranged next to each other), 
        when Size=3: four areas (SC0,SC1 in upper row, SC2,SC3 in lower row). 
        Whereas SC0 is defined by the normal BG Map base address (Bit 8-12 of BGxCNT), 
        SC1 uses same address +2K, SC2 address +4K, SC3 address +6K. When the screen is scrolled it'll always wraparound.

        In 'Rotation/Scaling Modes', the screen size is organized as follows, 
        only one area (SC0) of variable size 128x128..1024x1024 pixels (16x16..128x128 tiles) exists. 
        When the screen is rotated/scaled (or scrolled?) so that the LCD viewport reaches outside of the background/screen area, 
        then BG may be either displayed as transparent or wraparound (Bit 13 of BGxCNT).
    */
    };
};

struct BackgroundOffset // Write only
{
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Write16(u16 value) { IO::Write16ByBytes(this->value, value, writeMask); }

    void Reset() { Write16(0); }

    const u16 writeMask = 0x01FF;

    union
    {
        struct 
        {
            u16 offset : 9; // (0-511)
            u16 unused : 7 = 0;
        } fields;
        u16 value;

        /*
            Specifies the coordinate of the upperleft first visible dot of BG0 background layer, 
            ie. used to scroll the BG0 area.

            The above BG scrolling registers are exclusively used in Text modes, ie. for all layers in BG Mode 0, 
            and for the first two layers in BG mode 1.
            In other BG modes (Rotation/Scaling and Bitmap modes) above registers are ignored. 
            Instead, the screen may be scrolled by modifying the BG Rotation/Scaling Reference Point registers.
        */
    };
};

struct BackgroundRefPointCoords // Write-only
{
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Write16(u16 value) { IO::Write16ByBytes(this->value, value, writeMask); }
    void Write32(u32 value) { IO::Write32}

    union
    {
        struct 
        {
            u32 fraction : 8;
            u32 integer : 19;
            u32 sign : 1;
            u32 unused : 4;
        } fields;
        u32 value;

        /*
            Specifies the coordinate of the upperleft first visible dot of BG0 background layer, 
            ie. used to scroll the BG0 area.

            The above BG scrolling registers are exclusively used in Text modes, ie. for all layers in BG Mode 0, 
            and for the first two layers in BG mode 1.
            In other BG modes (Rotation/Scaling and Bitmap modes) above registers are ignored. 
            Instead, the screen may be scrolled by modifying the BG Rotation/Scaling Reference Point registers.
        */
    };
};

struct BackgroundScalingParameters // Write-only
{

};



