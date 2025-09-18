#include "Core/GBA_CPU.hpp"
#include "Core/CPU_Shifts.hpp"

void GBA_CPU::HalfwordDataTransfer(uint32_t instruction)
{
    HalfwordDataTransfer_Decoded values = HalfwordDataTransfer_Decode(instruction);
    uint32_t& rn = registers[values.rnIndex];
    uint32_t& rd = registers[values.rdIndex];

    bool isImmediate = values.iFlag;
    bool isLoad = values.lFlag;
    bool isSigned = values.sFlag;
    bool isHalfword = values.hFlag;
    bool preIndexed = values.pFlag;
    bool shouldWriteback = values.wFlag;

    if (!isSigned && !isHalfword) return; // This is another instruction - Mult or Single Data Swap

    // POSSIBLE:
    // LOADS: LDRH LDRSB LDRSH
    // STORES: STRH

    uint32_t effectiveAddress;
    uint32_t offset = isImmediate ? GetHDTOffset_Immediate(instruction) : GetHDTOffset_Register(instruction);

    if (preIndexed) // P = 1
    {
        effectiveAddress = values.uFlag ? rn + offset : rn - offset;
        if (shouldWriteback)
        {
            // Pre-indexed with write-back: Rn = EA
            rn = effectiveAddress;

            if (isLoad && (values.rdIndex == values.rnIndex)) return; // UNPREDICTABLE
        }
    }
    else // P = 0
    {
        if (shouldWriteback) return; // UNPREDICTABLE

        effectiveAddress = rn;
        // Post-index update happens AFTER transfer
    }

    // GBA SPECIFIC LOADING: IF INSTRUCTION IS MISALIGNED FOR HALFWORDS,
    // IT ROTATES THE LOADED HALFWORD 8 BITS TO THE RIGHT

    if (isLoad)
    {
        if (!isHalfword)
        {
            // LDRSB
            uint8_t loadedByte = memorySystem.Read8(effectiveAddress);
            rd = SignExtendTo32(loadedByte);
        }
        else
        {
            bool misaligned = effectiveAddress & 1;
            uint16_t loadedHalfword = memorySystem.Read16(effectiveAddress);
            if (misaligned)
            {
                loadedHalfword = RotateRight(loadedHalfword, 8);
            }
            
            // LDRSH vs LDRH
            rd = isSigned ? SignExtendTo32(loadedHalfword) : ZeroExtendTo32(loadedHalfword);
        }
    }
    else
    {
        if (isSigned) return; // UNPREDICTABLE

        // STRH

        // GBA SPECIFIC: NORMALLY YOU WOULD ABORT THE INSTRUCTION IF IT IS NOT ALIGNED
        // INSTEAD, THE GBA JUST PROCEEDS AND DOES THE STORE INSTRUCTION WITH THE UNALIGNED ADDRESS
        uint16_t valueToStore = static_cast<uint16_t>(rd);
        memorySystem.Write8(effectiveAddress, valueToStore & 0xFF); // Lowest 8 bits
        memorySystem.Write8(effectiveAddress + 1, valueToStore >> 8); // Shift higher bits
    }

    // Only post-index case updates here
    if (!preIndexed)
    {
        rn = values.uFlag ? (rn + offset) : (rn - offset);
    } 
}

void GBA_CPU::SingleDataTransfer(uint32_t instruction)
{
    SingleDataTransfer_Decoded values = SingleDataTransfer_Decode(instruction);

    uint32_t& rn = registers[values.rnIndex];
    uint32_t& rd = registers[values.rdIndex];

    bool isImmediate = !values.iFlag; // 0 means immediate offset weirdly
    bool isLoadInstruction = values.lFlag;
    bool isByte = values.bFlag;
    bool isLoad = values.lFlag;
    bool preIndexed = values.pFlag;
    bool shouldWriteback = values.wFlag;
    
    uint32_t effectiveAddress;
    uint32_t offset = isImmediate ? GetHDTOffset_Immediate(instruction) : GetHDTOffset_Register(instruction);


}