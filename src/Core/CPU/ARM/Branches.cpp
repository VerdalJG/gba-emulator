#include "Core/GBA_CPU.hpp"

#include "Utils/BitOperations.hpp"

void GBA_CPU::ARM_Branch(u32 instruction)
{
    const bool link = IsBitSet<24>(instruction);
    u32 offset_24 = ExtractBits<23, 0>(instruction);

    // Sign extend
    if (offset_24 & 0x800000)
    {  
        offset_24 |= 0xFF000000;
    }

    if (link)
    {
        // Current instruction address is PC - 8, so return address is PC - 4
        u32 returnAddress = cpuState.r15 - 4;
        cpuState.r14 = returnAddress;
    }

    u32 newPC = cpuState.r15 + offset_24 * 4; // Offset is in steps of 4
    cpuState.r15 = newPC & ~3; // Word-align
    FlushPipeline();
}

void GBA_CPU::ARM_BranchAndExchange(u32 instruction)
{
    u32 rnIndex = ExtractBits<3, 0>(instruction);
    u32 rn = ReadRegister(rnIndex);

    // Switch to thumb based on Rn bit 0
    cpuState.cpsr.fields.thumb = rn & 1;
    
    // Branch
    cpuState.r15 = rn & (IsThumbMode() ? ~1u : ~3u); // Word/halfword align based on thumb mode
    FlushPipeline();
}