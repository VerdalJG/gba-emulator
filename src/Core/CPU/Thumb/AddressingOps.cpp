#include "Core/GBA_CPU.hpp"

#include "Utils/BitOperations.hpp"

void GBA_CPU::Thumb_GetRelativeAddress(u16 instruction)
{
    const bool getSP = IsBitSet<11>(instruction);
    const u16 rdIndex = ExtractBits<10, 8>(instruction);

    u32 rd = ReadRegister(rdIndex);
    const u16 offset_8 = ExtractBits<7, 0>(instruction) * 4; // Stepped by 4, word accesses

    if (getSP) // Stack pointer
    {
        cpuState.registers[rdIndex] = ADD(cpuState.r13, offset_8, false);
    }
    else // Program counter
    {
        cpuState.registers[rdIndex] = ADD((cpuState.r15 & ~2), offset_8, false);
    }
    
    pipeline.access = Access::Code | Access::Sequential;
    AdvanceProgramCounter();
}

void GBA_CPU::Thumb_AddOffsetToStackPointer(u16 instruction)
{
    const bool sub = IsBitSet<7>(instruction);
    const u16 offset_7 = ExtractBits<6, 0>(instruction) * 4; // Stepped by 4, word accesses
    cpuState.r13 = sub ? SUB(cpuState.r13, offset_7, false) : ADD(cpuState.r13, offset_7, false);

    pipeline.access = Access::Code | Access::Sequential;
    AdvanceProgramCounter();
}