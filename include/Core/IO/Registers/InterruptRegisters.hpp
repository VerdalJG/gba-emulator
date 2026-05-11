#pragma once

#include "Core/IO/GBA_IO_Helpers.hpp"
#include "Utils/Integers.hpp"

#include <array>

struct InterruptEnable // R/W
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write16ByBytes(this->value, value, writeMask); }
    void Reset() { value = 0; }

    const u16 writeMask = 0x3FFF;

    union 
    {
        struct
        {
            u16 vBlank : 1; // (0 = disable, 1 = enable)
            u16 hBlank : 1;
            u16 vCounterMatch : 1;
            u16 timer0Overflow : 1;
            u16 timer1Overflow : 1;
            u16 timer2Overflow : 1;
            u16 timer3Overflow : 1;
            u16 serialCommunication : 1;
            u16 dma0 : 1;
            u16 dma1 : 1;
            u16 dma2 : 1;
            u16 dma3 : 1;
            u16 keypad : 1;
            u16 gamepak : 1; // (External IRQ source)
            u16 unused : 2;
        } fields;
        u16 value;
    };
};

struct InterruptRequestFlags // R/W
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write16ByBytes(this->value, value, writeMask); }
    void Reset() { value = 0; }

    const u16 writeMask = 0x3FFF;

    union 
    {
        struct
        {
            u16 vBlank : 1; // (1 = request interrupt)
            u16 hBlank : 1;
            u16 vCounterMatch : 1;
            u16 timer0Overflow : 1;
            u16 timer1Overflow : 1;
            u16 timer2Overflow : 1;
            u16 timer3Overflow : 1;
            u16 serialCommunication : 1;
            u16 dma0 : 1;
            u16 dma1 : 1;
            u16 dma2 : 1;
            u16 dma3 : 1;
            u16 keypad : 1;
            u16 gamepak : 1; // (External IRQ source)
            u16 unused : 2;
        } fields;
        u16 value;
    };
};

struct InterruptMasterEnable // R/W
{
    u8 Read8(int byteToRead) { return IO::Read8(value, byteToRead); }
    void Write8(int byteToWrite, u8 value) { IO::Write16ByBytes(this->value, value, writeMask); }
    void Reset() { value = 0; }

    const u16 writeMask = 0x0001;

    union 
    {
        struct
        {
            u16 enableInterrupts : 1;
            u16 unused : 15;
        } fields;
        u16 value;
    };
};

struct IO_InterruptRegisters
{
    InterruptEnable ie;
    InterruptRequestFlags irf;
    InterruptMasterEnable ime; 

    void Reset()
    {
        ie.Reset();
        irf.Reset();
        ime.Reset();
    }
};