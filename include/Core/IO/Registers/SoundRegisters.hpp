#pragma once

#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Utils/Integers.hpp"

#include <array>

struct SoundControl_X
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset() { value = 0; }
    void ResetToPostBIOSValue() { value = 0x0080; }

    const u32 writeMask = 0x0080;

    union 
    {
        struct
        {
            u16 sound1 : 1; // ReadOnly
            u16 sound2 : 1; // ReadOnly
            u16 sound3 : 1; // ReadOnly
            u16 sound4 : 1; // ReadOnly
            u16 unused0 : 3;
            u16 psgMasterEnable : 1; // (0=Disable, 1=Enable)
            u16 unused1 : 8;
        } fields;
        u16 value;
    };
};

struct SoundBias
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write8Masked(this->value, byteToWrite, value, writeMask); }
    void Reset() { value = 0; }
    void ResetToPostBIOSValue() { value = 0x200; } 

    const u32 writeMask = 0xC3FE;

    union 
    {
        struct
        {
            u16 unused0 : 1;
            u16 biasLevel : 9; // (Default = 100h, converting signed samples into unsigned)
            u16 unused1 : 4;
            u16 amplitudeResolution : 2; // (Default = 0, see below)
        } fields;
        u16 value;
    };
    
    /*
        Amplitude Resolution/Sampling Cycle (0-3):
        0  9bit / 32.768kHz   (Default, best for DMA channels A,B)
        1  8bit / 65.536kHz
        2  7bit / 131.072kHz
        3  6bit / 262.144kHz  (Best for PSG channels 1-4)
    */
};

struct IO_SoundRegisters
{
    SoundControl_X soundcnt_x;
    SoundBias soundBias;

    void Reset(bool skipBios)
    {
        if (skipBios)
        {
            soundcnt_x.ResetToPostBIOSValue();
            soundBias.ResetToPostBIOSValue();
            
        }
        else
        {
            soundcnt_x.Reset();
            soundBias.Reset();
        }
    }
};