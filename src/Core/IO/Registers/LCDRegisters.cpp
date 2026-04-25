#include "Core/IO/Registers/LCDRegisters.hpp"
#include "Core/GBA_PPU.hpp"

#include "Utils/BitOperations.hpp"

u8 DisplayStatus::Read8(int byteToRead) 
{ 
    u8 byte = IO::Read8(value, byteToRead); 

    if (byteToRead == 0)
    {
        byte &= ~0x7; // Keep everything excepts bits 0-2;

        byte |= static_cast<u8>(ppu->InVBlank());
        byte |= static_cast<u8>(ppu->InHBlank()) << 1;
        byte |= static_cast<u8>((ppu->GetVCount() == fields.vCountSetting)) << 2;
    }

    return byte;
}