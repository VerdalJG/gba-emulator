#pragma once

#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Utils/Integers.hpp"

#include <array>

struct DMAControl_H
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write16ByBytes(this->value, value, writeMask); }
    void Reset() { value = 0; }

    const u16 writeMask = 0xFFE0;

    union 
    {
        struct
        {
            u16 unused : 5;
            u16 destAddressControl : 2; // (0=Increment,1=Decrement,2=Fixed,3=Increment/Reload)
            u16 sourceAddressControl : 2; // (0=Increment,1=Decrement,2=Fixed,3=Prohibited)
            u16 dmaRepeat : 1; // (0=Off, 1=On) (Must be zero if Bit 11 set)
            u16 dmaTransferType: 1; // (0=16bit, 1=32bit)
            u16 gamePakDRQ : 1; // (0=Normal, 1=DRQ <from> Game Pak, DMA3)
            u16 dmaStartTiming : 2; // (0=Immediately, 1=VBlank, 2=HBlank, 3=Special)
            /*
                The 'Special' setting (Start Timing=3) depends on the DMA channel:
                DMA0=Prohibited, DMA1/DMA2=Sound FIFO, DMA3=Video Capture
            */
            u16 irqAfterWordCount : 1; // (0=Disable, 1=Enable)
            u16 dmaEnable : 1; // (0=Off, 1=On)
        } fields;
        u16 value;
    };

    /*
        Other Notes:
        - After changing the Enable bit from 0 to 1, wait 2 clock cycles before accessing any DMA related registers.
        - When accessing OAM (7000000h) or OBJ VRAM (6010000h) by HBlank Timing, 
          then the "H-Blank Interval Free" bit in DISPCNT register must be set.
    */
};

struct DMARegisters
{
    std::array<u32, 4> dmasad; // DMA Source Address
    std::array<u32, 4> dmadad; // DMA Destination Address
    std::array<u16, 4> dmacnt_L; // DMA Word Count
    std::array<DMAControl_H, 4> dmacnt_H; // DMA Control
};
