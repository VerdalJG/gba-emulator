#include "Core/IORegisters/LCDRegisters.hpp"

#include "Utils/BitOperations.hpp"

void DisplayControl::Reset() 
{
    Write16(0);
}

u8 DisplayControl::Read8(int byteToRead) { return u8(); }

u16 DisplayControl::Read16() { return u16(); }

void DisplayControl::Write8(int byteToWrite, u8 value) {}

void DisplayControl::Write16(u16 value) {}

