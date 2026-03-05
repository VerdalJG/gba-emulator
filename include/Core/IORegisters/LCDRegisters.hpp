#pragma once

#include "Core/IORegisters/IORegisterBase.hpp"

struct DisplayControl : IORegisterBase
{
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

    void Reset();

    u8 Read8(int byteToRead);
    u16 Read16();

    void Write8(int byteToWrite, u8 value);
    void Write16(u16 value);
};