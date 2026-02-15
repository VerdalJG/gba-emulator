#include "Core/CPU/ARM/Decoder.hpp"
#include "Core/GBA_CPU.hpp"

#include "Core/CPU/ARM/DataProcessing.hpp"


void GBA_CPU::Decode_ARM(u32 instruction, InstructionHandler& handler)
{
    switch (ExtractBits<27, 26>(instruction))
    {
        case 0b00: return Decode_ARM_Pattern00(instruction, handler);
        case 0b01: return Decode_ARM_Pattern01(instruction, handler);
        case 0b10: return Decode_ARM_Pattern10(instruction, handler);
        case 0b11: return Decode_ARM_Pattern11(instruction, handler);
    }
}

void GBA_CPU::Decode_ARM_Pattern00(u32 instruction, InstructionHandler& handler)
{
    // ARM 1. Data Processing Immediate
    if (IsBitSet<25>(instruction))
    {
        handler = &ARM_DataProcessing_Immediate;
        return;
    } 

    if ((ExtractBits<7, 4>(instruction) == 0b1001))
    {
        // ARM 2. Multiply
        if (ExtractBits<24, 22>(instruction) == 0b000)
        {
            handler = &ARM_Multiply;
            return;
        }

        // ARM 3. Multiply Long
        if (ExtractBits<24, 23>(instruction) == 0b01)
        {
            handler = &ARM_Multiply;
            return;
        }
    }

    if ()
}

void GBA_CPU::Decode_ARM_Pattern01(u32 instruction, InstructionHandler& handler)
{
    
}

void GBA_CPU::Decode_ARM_Pattern10(u32 instruction, InstructionHandler& handler)
{
    
}

void GBA_CPU::Decode_ARM_Pattern11(u32 instruction, InstructionHandler& handler)
{
    
}
