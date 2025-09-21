#include "Core/CPU/Instructions/CPU_LoadStore.hpp"
#include "Core/CPU/Instructions/CPU_AddressingMode2.hpp"
#include "Core/CPU/Instructions/CPU_AddressingMode3.hpp"
#include "Core/CPU/Instructions/CPU_Shifts.hpp"
#include "Core/CPU/Instructions/InstructionHelpers.hpp"

#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/CPU_Memory.hpp"


// Loads: LDRH LDRSB LDRSH
// Stores: STRH
void HalfwordDataTransfer(uint32_t instruction, GBA_CPU& cpu)
{
    HalfwordDataTransfer_Decoded values = HalfwordDataTransfer_Decode(instruction);

    if (!values.sFlag && !values.hFlag) return; // This is another instruction - Mult or Single Data Swap

    GBA_Memory& memory = cpu.GetMemorySystem();
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);

    uint32_t effectiveAddress = CalculateAddress_AddressingMode3(values, cpu);

    bool isLoad = values.lFlag;
    bool isHalfword = values.hFlag;
    bool isSigned = values.sFlag;
    
    // GBA SPECIFIC LOADING: IF INSTRUCTION IS MISALIGNED FOR HALFWORDS,
    // IT ROTATES THE LOADED HALFWORD 8 BITS TO THE RIGHT

    if (isLoad)
    {
        if (!isHalfword)
        {
            // LDRSB
            uint8_t loadedByte = memory.Read8(effectiveAddress);
            uint32_t valueToLoad = SignExtendTo32(loadedByte);
            cpu.SetValueAtRegister(values.rdIndex, valueToLoad);
        }
        else
        {
            // Account for misaligned halfword
            bool misaligned = effectiveAddress & 1; // First bit must be 0 to be divisible by 2 (halfword)
            uint16_t loadedHalfword = memory.Read16(effectiveAddress);
            if (misaligned)
            {
                loadedHalfword = RotateRight(loadedHalfword, 8);
            }
            
            // LDRSH vs LDRH
            uint32_t valueToLoad = isSigned ? SignExtendTo32(loadedHalfword) : ZeroExtendTo32(loadedHalfword);
            cpu.SetValueAtRegister(values.rdIndex, valueToLoad);
        }
    }
    else // STRH
    {
        if (isSigned) return; // UNPREDICTABLE

        // GBA SPECIFIC: NORMALLY YOU WOULD ABORT THE INSTRUCTION IF IT IS NOT ALIGNED
        // INSTEAD, THE GBA JUST PROCEEDS AND DOES THE STORE INSTRUCTION WITH THE UNALIGNED ADDRESS
        uint16_t valueToStore = static_cast<uint16_t>(rd);
        memory.Write8(effectiveAddress, valueToStore & 0xFF); // Lowest 8 bits
        memory.Write8(effectiveAddress + 1, valueToStore >> 8); // Shift higher bits
    }

    // Only post-index case updates here
    if (!values.pFlag)
    {
        uint32_t offset = values.iFlag ? GetHDTOffset_Immediate(values.offsetBits) : GetHDTOffset_Register(values.offsetBits, cpu);
        uint32_t postIndexedAddress = values.uFlag ? (rn + offset) : (rn - offset);
        cpu.SetValueAtRegister(values.rnIndex, postIndexedAddress);
    } 
}

void SingleDataTransfer(uint32_t instruction, GBA_CPU& cpu)
{
    SingleDataTransfer_Decoded values = SingleDataTransfer_Decode(instruction);

    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);

    bool isImmediate = !values.iFlag; 
    bool isLoad = values.lFlag;
    bool isByte = values.bFlag;
    bool preIndexed = values.pFlag;
    bool shouldWriteback = values.wFlag;
    
    uint32_t effectiveAddress = CalculateAddress_AddressingMode2(values, cpu);
    
}