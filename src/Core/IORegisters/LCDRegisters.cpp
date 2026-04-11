#include "Core/IO/LCDRegisters.hpp"
#include "Core/GBA_PPU.hpp"

#include "Utils/BitOperations.hpp"

u8 DisplayStatus::Read8(int byteToRead) 
{ 
    u8 byte = IO::Read8(value, byteToRead); 

    if (byteToRead == 0)
    {
        byte &= ~0x7; // Keep everything excepts bits 0-2;

        byte |= (ppu->GetState() == PPUState::VBlank);
        byte |= (ppu->GetState() == PPUState::HBlank) << 1;
        //byte |= (ppu->vCount == fields.vCountSetting) << 2; TODO: Uncomment once vcount is added
    }

    return byte;
}

u16 DisplayStatus::Read16() 
{ 
    u16 result = value;

    result &= ~0x7; // Keep everything excepts bits 0-2;

    result |= (ppu->GetState() == PPUState::VBlank);
    result |= (ppu->GetState() == PPUState::HBlank) << 1;
    //result |= (ppu->vCount == fields.vCountSetting) << 2; TODO: Uncomment once vcount is added

    return result;
}