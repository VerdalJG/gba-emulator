#include "Core/IORegisters/LCDRegisters.hpp"
#include "Core/GBA_PPU.hpp"

#include "Utils/BitOperations.hpp"

DisplayControl::DisplayControl() : IORegisterBase()
{

}

void DisplayControl::Reset()
{
    Write16(0);
}

void DisplayControl::ResetToPostBIOSValue() 
{
    Write16(0x80);
}

u8 DisplayControl::Read8(int byteToRead) 
{ 
    return (value >> (8 * byteToRead)) & 0xFF;
}

u16 DisplayControl::Read16() 
{
    return (Read8(1) << 8) | Read8(0);
}

void DisplayControl::Write8(int byteToWrite, u8 value) 
{   
    if (byteToWrite == 0)
    {
        // CGB bit is read-only so we preserve it
        u8 oldLowByte = this->value & 0xFF;
        value = (value & ~(1 << 3)) | (oldLowByte & (1 << 3)); 
    }
    
    this->value &= ~(0xFF << (byteToWrite * 8)); // Clear
    this->value |= (value << (byteToWrite * 8));
}

void DisplayControl::Write16(u16 value) 
{
    Write8(0, value & 0xFF);
    Write8(1, (value >> 8) & 0xFF);
}

