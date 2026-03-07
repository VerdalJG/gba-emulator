#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Shifts.hpp"
#include "Core/CPU/MultiplyTiming.hpp"

#include "Utils/BitOperations.hpp"

void GBA_CPU::ARM_Multiply(u32 instruction)
{
    u32 rdIndex = ExtractBits<19, 16>(instruction);
    u32 rnIndex = ExtractBits<15, 12>(instruction);
    u32 rsIndex = ExtractBits<11, 8>(instruction);
    u32 rmIndex = ExtractBits<3, 0>(instruction);

    u32 rm = ReadRegister(rmIndex);
    u32 rs = ReadRegister(rsIndex);

    bool accumulate = IsBitSet<21>(instruction);
    bool set_flags = IsBitSet<20>(instruction);

    // Check for Unpredictable conditions
    bool usingPC = rmIndex == 15 || rsIndex == 15 || rdIndex == 15 || rnIndex == 15;
    bool rdRmAreDifferent = rdIndex != rmIndex;

    if (usingPC || !rdIndex != rmIndex) 
    {
        // UNPREDICTABLE
    } 

    // MUL and MLA
    u64 result = rm * rs;

    if (accumulate)
    {
        u32 rn = ReadRegister(rnIndex);
        result += rn;
        AddInternalCycles(1);
    }
    
    u32 mulCycles = CalculateMultiplierCycles(rs);
    AddInternalCycles(mulCycles);

    cpuState.registers[rdIndex] = static_cast<u32>(result);

    if (set_flags)
    {
        UpdateNZFlags(result);
        // Carry flag is destroyed/unpredictable. ARM7TDMI may do something specific, but games
        // generally wont use it's value.
    }

    pipeline.access = Access::Code | Access::Nonsequential;

    if (rdIndex == 15)
    {
        FlushPipeline();
    }
    else
    {
        AdvanceProgramCounter();
    }
}

void GBA_CPU::ARM_MultiplyLong(u32 instruction)
{
    u32 rdHiIndex = ExtractBits<19, 16>(instruction);
    u32 rdLoIndex = ExtractBits<15, 12>(instruction);
    u32 rsIndex = ExtractBits<11, 8>(instruction);
    u32 rmIndex = ExtractBits<3, 0>(instruction);

    bool signed_flag = IsBitSet<22>(instruction);
    bool accumulate = IsBitSet<21>(instruction);
    bool set_flags = IsBitSet<20>(instruction);

    u32 rm = ReadRegister(rmIndex);
    u32 rs = ReadRegister(rsIndex);

    u32 rdHi = ReadRegister(rdHiIndex);
    u32 rdLo = ReadRegister(rdLoIndex);
    u64 product;

    // Check for Unpredictable conditions
    bool usingPC = rmIndex == 15 || rdHiIndex == 15 || rdLoIndex == 15 || rsIndex == 15;
    bool allDistinct =  rmIndex != rdHiIndex &&
                        rmIndex != rdLoIndex &&
                        rdHiIndex != rdLoIndex;

    if (usingPC || !allDistinct)
    {
        // UNPREDICTABLE - Cannot use R15 nor use the same registers
    } 

    if (signed_flag)
    {
        s64 signedRm = static_cast<s64>(static_cast<s32>(rm));
        s64 signedRs = static_cast<s64>(static_cast<s32>(rs));
        product = static_cast<u64>(signedRm * signedRs);
    }
    else
    {
        product = static_cast<u64>(rm) * static_cast<u64>(rs);
    }

    if (accumulate)
    {
        u64 accumulator = (static_cast<u64>(rdHi) << 32) | rdLo;
        product += accumulator;

        AddInternalCycles(1);
    }

    rdLo = static_cast<u32>(product);
    rdHi = static_cast<u32>(product >> 32);

    cpuState.registers[rdHiIndex] = rdHi;
    cpuState.registers[rdLoIndex] = rdLo;

    if (set_flags)
    {
        cpuState.cpsr.fields.n = rdHi >> 31;
        cpuState.cpsr.fields.z = rdLo == 0 && rdHi == 0;
        // Carry flag is destroyed/unpredictable. ARM7TDMI may do something specific, but games
        // generally wont use it's value.
    }

    // Cycle calculation
    pipeline.access = Access::Code | Access::Nonsequential;

    u32 mulCycles = CalculateMultiplierCycles(rs);
    AddInternalCycles(1);
    // TODO: Process cycles properly

    if (rdHiIndex == 15 || rdLoIndex == 15)
    {
        FlushPipeline();
    }
    else
    {
        AdvanceProgramCounter();
    }
}

