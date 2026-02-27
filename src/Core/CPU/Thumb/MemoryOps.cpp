#include "Core/GBA_CPU.hpp"

#include "Utils/BitOperations.hpp"

#include <assert.h>

void GBA_CPU::Thumb_LoadPCRelative(u16 instruction)
{   
    const u16 rdIndex = ExtractBits<10, 8>(instruction);
    const u16 offset_8 = ExtractBits<7, 0>(instruction); 
    const u32 address = (cpuState.r15 & ~ 2) + (offset_8 << 2); // Offset is in steps of 4 (0-1020)

    // Load the value
    u32 rd = cpuState.registers[rdIndex];
    cpuState.registers[rdIndex] = Read32(address, Access::Data | Access::Nonsequential);

    pipeline.access = Access::Code | Access::Sequential;
    AdvanceProgramCounter();
    // TODO: Add one I cycle
}

void GBA_CPU::Thumb_LoadStoreRegisterOffset(u16 instruction)
{
    const u16 opcode = ExtractBits<11, 10>(instruction);
    const u16 roIndex = ExtractBits<8, 6>(instruction);
    
    const u16 rbIndex = ExtractBits<5, 3>(instruction);
    const u16 rdIndex = ExtractBits<2, 0>(instruction);

    u32 ro = ReadRegister(roIndex);
    u32 rb = ReadRegister(rbIndex);
    u32 rd = ReadRegister(rdIndex);

    u32 address =  rb + ro;

    

    switch (opcode)
    {
        case 0: // STR
        {
            Write32(address, rd, Access::Data | Access::Nonsequential);
            break;
        }

        case 1: // STRB
        {
            Write8(address, rd & 0xFF, Access::Data | Access::Nonsequential);
            break;
        }

        case 2: // LDR
        {
            cpuState.registers[rdIndex] = Read32_Rotated(address, Access::Data | Access::Nonsequential);
            // TODO: Add one I cycle
            break;
        }

        case 3: // LDRB
        {
            cpuState.registers[rdIndex] = Read8(address, Access::Data | Access::Nonsequential);
            // TODO: Add one I cycle
            break;
        }
    }

    if (opcode < 2)
    {
        pipeline.access = Access::Code | Access::Nonsequential;
    }
    else
    {
        pipeline.access = Access::Code | Access::Sequential;
    }

    AdvanceProgramCounter();
}

void GBA_CPU::Thumb_LoadStoreSignExtended(u16 instruction)
{
    const u16 opcode = ExtractBits<11, 10>(instruction);
    const u16 roIndex = ExtractBits<8, 6>(instruction);

    u16 rbIndex = ExtractBits<5, 3>(instruction);
    u16 rdIndex = ExtractBits<2, 0>(instruction);

    u32 ro = ReadRegister(roIndex);
    u32 rb = ReadRegister(rbIndex);
    u32 rd = ReadRegister(rdIndex);

    u32 address = rb + ro;

    switch (opcode)
    {
        case 0: // STRH
        {
            Write16(address, rd, Access::Data | Access::Nonsequential);
            break;
        }

        case 1: // LDRSB
        {
            cpuState.registers[rdIndex] = Read8_Signed(address, Access::Data | Access::Nonsequential);
            // TODO: Add one I cycle
            break;
        }

        case 2: // LDRH
        {
            cpuState.registers[rdIndex] = Read32_Rotated(address, Access::Data | Access::Nonsequential);
            // TODO: Add one I cycle
            break;
        }

        case 3: // LDRSH
        {
            cpuState.registers[rdIndex] = Read16_Signed(address, Access::Data | Access::Nonsequential);
            // TODO: Add one I cycle
            break;
        }
    }

    if (opcode < 1)
    {
        pipeline.access = Access::Code | Access::Nonsequential;
    }
    else
    {
        pipeline.access = Access::Code | Access::Sequential;
    }

    AdvanceProgramCounter();
}

void GBA_CPU::Thumb_LoadStoreImmediateOffset(u16 instruction)
{
    const u16 opcode = ExtractBits<12, 11>(instruction);
    u16 offset_5 = ExtractBits<10, 6>(instruction);

    const u16 rbIndex = ExtractBits<5, 3>(instruction);
    const u16 rdIndex = ExtractBits<2, 0>(instruction);

    u32 rb = ReadRegister(rbIndex);
    u32 rd = ReadRegister(rdIndex);

    if (opcode < 2) // Offset is 0-31 for byte and 0-124 for word
    {
        offset_5 *= 4; 
    }

    u32 address = rb + offset_5;

    switch (opcode)
    {
        case 0: // STR 
        {
            Write32(address, rd, Access::Data | Access::Nonsequential);
        }

        case 1: // LDR
        {
            cpuState.registers[rdIndex] = Read32_Rotated(address, Access::Data | Access::Nonsequential);
            // TODO: Add one I cycle
        }

        case 2: // STRB
        {
            Write8(address, rd, Access::Data | Access::Nonsequential);
        }

        case 3: // LDRB
        {
            cpuState.registers[rdIndex] = Read8(address, Access::Data | Access::Nonsequential);
            // TODO: Add one I cycle
        }
    }

    if (opcode % 2 == 0) // Stores
    {
        pipeline.access = Access::Code | Access::Nonsequential;
    }
    else // Loads
    {
        pipeline.access = Access::Code | Access::Sequential;
    }

    AdvanceProgramCounter();
}

void GBA_CPU::Thumb_LoadStoreHalfword(u16 instruction)
{
    const bool load = IsBitSet<11>(instruction);
    u16 offset_5 = ExtractBits<10, 6>(instruction);

    const u16 rbIndex = ExtractBits<5, 3>(instruction);
    const u16 rdIndex = ExtractBits<2, 0>(instruction);

    u32 rb = ReadRegister(rbIndex);
    u32 rd = ReadRegister(rdIndex);

    offset_5 *= 2; // Stepped by 2 because this function works with halfwords
    u32 address = rb + offset_5;

    if (load) // LDRH
    {
        cpuState.registers[rdIndex] = Read16_Rotated(address, Access::Data | Access::Nonsequential);
        pipeline.access = Access::Code | Access::Sequential;
        // TODO: Add one I cycle
    }
    else // STRH
    {
        Write16(address, rd & 0xFFFF, Access::Data | Access::Nonsequential);
        pipeline.access = Access::Code | Access::Nonsequential;
    }

    AdvanceProgramCounter();
}

void GBA_CPU::Thumb_LoadStoreSPRelative(u16 instruction)
{
    const bool load = IsBitSet<11>(instruction);
    const u16 rdIndex = ExtractBits<10, 8>(instruction);

    u32 rd = ReadRegister(rdIndex);
    const u16 offset_8 = ExtractBits<7, 0>(instruction) * 4; // Stepped by 4, word accesses

    u32 address = cpuState.r13 + offset_8;

    if (load) // STR
    {
        Write32(address, rd, Access::Data | Access::Nonsequential);
        pipeline.access = Access::Code | Access::Nonsequential;
    }
    else // LDR
    {
        cpuState.registers[rdIndex] = Read32_Rotated(address, Access::Data | Access::Nonsequential);
        pipeline.access = Access::Code | Access::Sequential;
        // TODO: Add one I cycle
    }

    AdvanceProgramCounter();
}