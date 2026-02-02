#include "Core/CPU/Instructions/DataProcessingMisc.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/CPU/CPU_CPSR.hpp"
#include "Core/CPU/Instructions/Shifts.hpp"
#include "Core/CPU/CPU_Timings.hpp"
#include "Core/CPU/Instructions/InstructionHelpers.hpp"

void Multiply(uint32_t instruction, GBA_CPU& cpu)
{
    Multiply_Decoded values = Multiply_Decode(instruction);

    uint32_t rm = cpu.GetValueAtRegister(values.rmIndex);
    uint32_t rs = cpu.GetValueAtRegister(values.rsIndex);
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);
    
    // Check for Unpredictable conditions
    bool usingPC = values.rmIndex == 15 || values.rsIndex == 15 || values.rdIndex == 15 || values.rnIndex == 15;
    bool rdRmAreDifferent = values.rdIndex != values.rmIndex;

    if (usingPC || !rdRmAreDifferent) return; // UNPREDICTABLE

    // MUL and MLA
    uint32_t result = values.accumulateFlag ? (rm * rs + rn) : (rm * rs);
    cpu.SetValueAtRegister(values.rdIndex, result);

    // CPU Cycles calculation
    uint32_t multiplierCycles = CalculateMultiplierCycles(rs);
    uint32_t totalCycles = multiplierCycles + values.accumulateFlag;
    cpu.AddCycles(totalCycles);

    // CPSR update
    if (!values.setCPSRFlag) return;

    uint32_t N = CPSR_IsNegative(result);
    uint32_t Z = CPSR_IsZero(result);

    // Combine flags
    uint32_t flags = N | Z;

    // Update cpsr
    cpu.UpdateCPSR(flags, 0xC0000000);
    
    // C flag is technically unpredictable after a MULS in versions below ARM5
    // MUL truncates to lower 32 bits, so the result is the same for signed and unsigned numbers
}

void MultiplyLong(uint32_t instruction, GBA_CPU& cpu)
{
    MultiplyLong_Decoded values = MultiplyLong_Decode(instruction);

    uint32_t rdHi = cpu.GetValueAtRegister(values.rdHiIndex);
    uint32_t rdLo = cpu.GetValueAtRegister(values.rdLoIndex);
    uint32_t rm = cpu.GetValueAtRegister(values.rmIndex);
    uint32_t rs = cpu.GetValueAtRegister(values.rsIndex);

    // Check for Unpredictable conditions
    bool usingPC = values.rmIndex == 15 || values.rdHiIndex == 15 || values.rdLoIndex == 15 || values.rsIndex == 15;
    bool allDistinct =  values.rmIndex != values.rdHiIndex &&
                        values.rmIndex != values.rdLoIndex &&
                        values.rdHiIndex != values.rdLoIndex;

    if (usingPC || !allDistinct) return; // UNPREDICTABLE - Cannot use R15 nor use the same registers for any

    if (values.signedFlag)
    {
        int64_t signedRm = static_cast<int64_t>(static_cast<int32_t>(rm));
        int64_t signedRs = static_cast<int64_t>(static_cast<int32_t>(rs));
        int64_t product = signedRm * signedRs;
        if (values.accumulateFlag)
        {
            SMLAL(rdLo, rdHi, product);
        }
        else
        {
            SMULL(rdLo, rdHi, product);
        }
    }
    else // Unsigned
    {
        uint64_t product = static_cast<uint64_t>(rm) * static_cast<uint64_t>(rs);
        if (values.accumulateFlag)
        {
            UMLAL(rdLo, rdHi, product);
        }
        else
        {
            UMULL(rdLo, rdHi, product);
        }
    }

    cpu.SetValueAtRegister(values.rdHiIndex, rdHi);
    cpu.SetValueAtRegister(values.rdLoIndex, rdLo);

    // CPU Cycles calculation
    uint32_t multiplierCycles = CalculateMultiplierCycles(rs);
    uint32_t accumulateCycles = values.accumulateFlag ? 2u : 1u;
    uint32_t totalCycles = multiplierCycles + accumulateCycles;
    cpu.AddCycles(totalCycles);

    if (!values.setCPSRFlag) return;
    
    uint32_t N = CPSR_IsNegative(rdHi);
    uint32_t Z = CPSR_IsZero(rdHi) && CPSR_IsZero(rdLo);

    // Combine flags
    uint32_t flags = N | Z;

    // Update cpsr
    cpu.UpdateCPSR(flags, 0xC0000000);  // Preserve C and V flags
}

// Signed multiply long
void SMULL(uint32_t &rdLo, uint32_t &rdHi, int64_t& product)
{
    rdLo = static_cast<uint32_t>(product);
    rdHi = static_cast<uint32_t>(product >> 32);
}

// Signed multiply accumulate long
void SMLAL(uint32_t &rdLo, uint32_t &rdHi, int64_t& product)
{
    uint64_t lowSum = (product & 0xFFFFFFFF) + static_cast<uint64_t>(rdLo);
    rdLo = static_cast<uint32_t>(lowSum);
    rdHi += static_cast<uint32_t>(product >> 32) + CarryFrom(lowSum);
}

// Unsigned multiply long
void UMULL(uint32_t &rdLo, uint32_t &rdHi, uint64_t& product)
{
    rdLo = static_cast<uint32_t>(product);
    rdHi = static_cast<uint32_t>(product >> 32);
}

// Unsigned multiply accumulate long
void UMLAL(uint32_t &rdLo, uint32_t &rdHi, uint64_t& product)
{
    uint64_t lowSum = (product & 0xFFFFFFFF) + static_cast<uint64_t>(rdLo);
    rdLo = static_cast<uint32_t>(lowSum);
    rdHi += static_cast<uint32_t>(product >> 32) + CarryFrom(lowSum);
}

// Swap (SWP) or SwapByte (SWPB)
void SingleDataSwap(uint32_t instruction, GBA_CPU& cpu) 
{
    SingleDataSwap_Decoded values = SingleDataSwap_Decode(instruction);

        // Check for UNPREDICTABLE conditions
    if (values.rnIndex == 15 || values.rmIndex == 15 || values.rdIndex == 15)
    return; // UNPREDICTABLE

    if (values.rdIndex == values.rmIndex || values.rdIndex == values.rnIndex)
    return; // UNPREDICTABLE

    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rm = cpu.GetValueAtRegister(values.rmIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);

    uint32_t readValue;

    if (values.bFlag) // SWPB
    {
        readValue = cpu.Read8_Bus(rn);
        cpu.Write8_Bus(rn, rm & 0xFF);
    }
    else // SWP
    {
        uint32_t alignedAddress = rn & ~3u;
        uint32_t misalignment = rn & 3u;

        readValue = cpu.Read32_Bus(alignedAddress);

        // Rotate if misaligned
        if (misalignment)
        {
            readValue = RotateRight(readValue, misalignment * 8);
        }

        // Aligned write, unrotated
        cpu.Write32_Bus(alignedAddress, rm);
    }

    cpu.SetValueAtRegister(values.rdIndex, readValue); 
    cpu.AddCycles(CPU_Timings::SWAP_BASE_COST);
}

Multiply_Decoded Multiply_Decode(uint32_t instruction)
{
    Multiply_Decoded result;

    result.condition = GetConditionType(instruction);
    result.accumulateFlag = (instruction >> 21) & 1;
    result.setCPSRFlag = (instruction >> 20) & 1;

    result.rdIndex = (instruction >> 16) & 0xF;
    result.rsIndex = (instruction >> 8) & 0xF;
    result.rmIndex = instruction & 0xF;

    return result;
}

MultiplyLong_Decoded MultiplyLong_Decode(uint32_t instruction)
{
    MultiplyLong_Decoded result;

    result.condition = GetConditionType(instruction);
    result.signedFlag = (instruction >> 22) & 1;
    result.accumulateFlag = (instruction >> 21) & 1;
    result.setCPSRFlag = (instruction >> 20) & 1;

    result.rdHiIndex = (instruction >> 16) & 0xF;
    result.rdLoIndex = (instruction >> 12) & 0xF;
    result.rsIndex = (instruction >> 8) & 0xF;
    result.rmIndex = instruction & 0xF;

    return result;
}

SingleDataSwap_Decoded SingleDataSwap_Decode(uint32_t instruction)
{
    SingleDataSwap_Decoded result;

    result.condition = GetConditionType(instruction);
    result.bFlag = (instruction >> 24) & 1;
    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;
    result.rmIndex = instruction & 0xF;

    return result;
}



