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
    uint32_t totalCycles = CPU_Timings::ALU_BASE_COST + multiplierCycles + values.accumulateFlag;
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
    uint32_t totalCycles = CPU_Timings::ALU_BASE_COST + multiplierCycles + accumulateCycles;
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

    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rm = cpu.GetValueAtRegister(values.rmIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);

    // Check for UNPREDICTABLE conditions
    bool usingPC = values.rnIndex == 15 || values.rmIndex == 15 || values.rdIndex == 15;
    bool allDistinct = values.rnIndex != values.rmIndex &&
                       values.rnIndex != values.rdIndex &&
                       values.rmIndex != values.rdIndex;

    if (usingPC || !allDistinct) return; // UNPREDICTABLE

    uint32_t temp;

    if (values.bFlag) // SWPB
    {
        temp = cpu.Read8FromMemory(rn, false);
        uint8_t byteToWrite = rm & 0xFF;
        cpu.Write8ToMemory(rn, byteToWrite, false);
    }
    else // SWP
    {
        uint32_t rotatedBytes = (rn & 0b11);
        uint32_t rotatedBits = rotatedBytes * 8; // Rotate by byte size
        temp = RotateRight(cpu.Read32FromMemory(rn, false), rotatedBits); // Normally rotate right is only used for unaligned addresses
        cpu.Write32ToMemory(rn, rm, false); 
    }

    cpu.SetValueAtRegister(values.rdIndex, temp); 
}



