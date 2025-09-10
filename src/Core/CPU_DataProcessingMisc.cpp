#include "Core/GBA_CPU.hpp"
#include "Core/CPU_Shifts.hpp"

void GBA_CPU::Multiply(uint32_t instruction)
{
    Multiply_Decoded values = Multiply_Decode(instruction);

    uint32_t& rm = registers[values.rmIndex];
    uint32_t& rs = registers[values.rsIndex];
    uint32_t& rn = registers[values.rnIndex];
    uint32_t& rd = registers[values.rdIndex];
    
    // Check for Unpredictable conditions
    bool usingPC = values.rmIndex == 15 || values.rsIndex == 15 || values.rdIndex == 15 || values.rnIndex == 15;
    bool allDistinct = values.rdIndex != values.rmIndex;

    if (usingPC || !allDistinct) return; // UNPREDICTABLE

    // MUL and MLA
    rd = values.accumulateFlag ? (rm * rs + rn) : (rm * rs);

    uint32_t cpsrFlags = 0;

    // N flag
    cpsrFlags |= (rd & 0x80000000);

    // Z flag
    cpsrFlags |= ((rd == 0) << 30);

    // Update cpsr
    cpsr = (cpsr & 0x3FFFFFFF) | cpsrFlags; // Preserve C and V flag by masking
    
    // C flag is technically unpredictable after a MULS in versions below ARM5
    // MUL truncates to lower 32 bits, so the result is the same for signed and unsigned numbers

}

void GBA_CPU::MultiplyLong(uint32_t instruction)
{
    MultiplyLong_Decoded values = MultiplyLong_Decode(instruction);

    uint32_t& rdHi = registers[values.rdHiIndex];
    uint32_t& rdLo = registers[values.rdLoIndex];
    uint32_t& rm = registers[values.rmIndex];
    uint32_t& rs = registers[values.rsIndex];

    // Check for Unpredictable conditions
    bool usingPC = values.rmIndex == 15 || values.rdHiIndex == 15 || values.rdLoIndex == 15 || values.rsIndex == 15;
    bool allDistinct =  values.rmIndex != values.rdHiIndex &&
                        values.rmIndex != values.rdLoIndex &&
                        values.rdHiIndex != values.rdLoIndex;

    if (usingPC || !allDistinct) return; // UNPREDICTABLE - Cannot use R15 or registers must be distinct

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

    if (!values.setCpsrFlag) return;
    
    uint32_t cpsrFlags = 0;

    // N flag
    cpsrFlags |= (rdHi & 0x80000000);

    // Z flag
    cpsrFlags |= ((rdHi == 0 && rdLo == 0) << 30);

    // Update cpsr
    cpsr = (cpsr & 0x3FFFFFFF) | cpsrFlags; // Preserve C and V flag by masking
}

void GBA_CPU::SMULL(uint32_t &rdLo, uint32_t &rdHi, int64_t& product)
{
    rdLo = static_cast<uint32_t>(product);
    rdHi = static_cast<uint32_t>(product >> 32);
}

void GBA_CPU::SMLAL(uint32_t &rdLo, uint32_t &rdHi, int64_t& product)
{
    uint64_t lowSum = (product & 0xFFFFFFFF) + static_cast<uint64_t>(rdLo);
    rdLo = static_cast<uint32_t>(lowSum);
    rdHi += static_cast<uint32_t>(product >> 32) + CarryFrom(lowSum);
}

void GBA_CPU::UMULL(uint32_t &rdLo, uint32_t &rdHi, uint64_t& product)
{
    rdLo = static_cast<uint32_t>(product);
    rdHi = static_cast<uint32_t>(product >> 32);
}

void GBA_CPU::UMLAL(uint32_t &rdLo, uint32_t &rdHi, uint64_t& product)
{
    uint64_t lowSum = (product & 0xFFFFFFFF) + static_cast<uint64_t>(rdLo);
    rdLo = static_cast<uint32_t>(lowSum);
    rdHi += static_cast<uint32_t>(product >> 32) + CarryFrom(lowSum);
}

// Swap (SWP) or SwapByte (SWPB)
void GBA_CPU::SingleDataSwap(uint32_t instruction) 
{
    SingleDataSwap_Decoded values = SingleDataSwap_Decode(instruction);
    
    uint32_t& rn = registers[values.rnIndex];
    uint32_t& rm = registers[values.rmIndex];
    uint32_t& rd = registers[values.rdIndex];

    // Check for UNPREDICTABLE conditions
    bool usingPC = values.rnIndex == 15 || values.rmIndex == 15 || values.rdIndex == 15;
    bool allDistinct = values.rnIndex != values.rmIndex &&
                       values.rnIndex != values.rdIndex &&
                       values.rmIndex != values.rdIndex;

    if (usingPC || !allDistinct) return; // UNPREDICTABLE


    if (values.bFlag) // SWPB
    {
        uint32_t temp = memorySystem.Read8(rn);
 
        uint8_t byteToWrite = rm & 0xFF;
        memorySystem.Write8(rn, byteToWrite);
        rd = temp;
    }
    else // SWP
    {
        uint32_t rotatedBytes = (rn & 3);
        uint32_t rotatedBits = rotatedBytes * 8;

        uint32_t temp = RotateRight(memorySystem.Read32(rn), rotatedBits); // Normally rotate right is only used for unaligned addresses
        memorySystem.Write32(rn, rm);
        rd = temp; 
    }
}

// NOTE: Should only change thumb mode flag via this function, changing it directly is UNPREDICTABLE
void GBA_CPU::BranchAndExchange(uint32_t instruction)
{
    uint32_t rmIndex = instruction & 0xF;
    uint32_t rm = registers[rmIndex];
    
    // Check for UNPREDICTABLE: branch to half-word misaligned in ARM state
    if ((rm & 3) == 0b10) return;

    // Switch to arm/thumb if needed
    cpsr &= ~(1 << 5);// Flush bit 5 on cpsr (thumb mode flag)
    cpsr |= ((rm & 1) << 5); // ARM = 0, Thumb = 1

    registers[15] = rm & (0xFFFFFFFE); // Branch to the address held 

}

