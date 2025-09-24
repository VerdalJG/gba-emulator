#include "Core/CPU/Instructions/AddressingMode2.hpp"
#include "Core/CPU/GBA_CPU.hpp"
#include <assert.h>

uint32_t CalculateAddress_AddressingMode2(SingleDataTransfer_Decoded values, GBA_CPU& cpu)
{
    bool preIndexed = values.pFlag;
    bool writeback = values.wFlag;
    bool isLoad = values.lFlag;

    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t offset = CalculateOffset_AddressingMode2(values.offsetBits, values.iFlag, cpu);
    uint32_t resultAddress;
    uint32_t rmIndex = values.iFlag ? values.offsetBits & 0xF : 0;

    bool rnUsingPC = values.rnIndex == GBA_CPU::PC_INDEX;
    bool rmUsingPC = rmIndex == GBA_CPU::PC_INDEX;

    if (preIndexed)
    {
        resultAddress = values.uFlag ? rn + offset : rn - offset;
        if (rmUsingPC)
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }

        if (writeback)
        {
            if (rnUsingPC || (values.iFlag && values.rnIndex == rmIndex)) 
            {
                assert(false && "UNPREDICTABLE");
                return 0xFFFFFFFF; // For compiler
            }

            cpu.SetValueAtRegister(values.rnIndex, resultAddress);
        }
    }
    else 
    {
        resultAddress = rn;
        if (rnUsingPC || rmUsingPC || (values.iFlag && values.rnIndex == rmIndex)) 
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }
        // Post-indexed update of rn happens in after the transfer in a different function
    }
    return resultAddress;
}

uint32_t CalculateOffset_AddressingMode2(uint32_t offsetBits, bool isRegister, GBA_CPU &cpu)
{
    if (isRegister)
    {   
        // Differentiate between scaled register and normal register offset
        bool isScaledRegister = (offsetBits >> 4); 

        if (isScaledRegister)
        {
            return CalculateOffset_ScaledRegister(offsetBits, cpu);
        }
        else
        {
            return CalculateOffset_Register(offsetBits, cpu);
        }
    }
    else // Immediate
    {
        return offsetBits;
    }
}

uint32_t CalculateOffset_Register(uint32_t offsetBits, GBA_CPU& cpu)
{
    uint32_t rmIndex = offsetBits & 0xF;
    return cpu.GetValueAtRegister(rmIndex);
}

uint32_t CalculateOffset_ScaledRegister(uint32_t offsetBits, GBA_CPU& cpu)
{
    uint32_t rmIndex = offsetBits & 0xF;
    uint32_t rm = cpu.GetValueAtRegister(rmIndex);
    ShiftType shiftType = static_cast<ShiftType>((offsetBits >> 5) & 3);
    uint32_t shiftImm = (offsetBits >> 7) & 0x1F;
    return CalculateScaledRegister(rm, shiftType, shiftImm, cpu);
}

uint32_t CalculateScaledRegister(uint32_t rm, ShiftType shift, uint32_t shiftImm, GBA_CPU& cpu)
{
    switch (shift)
    {
        case ShiftType::LSL:
        return LogicalShiftLeft(rm, shiftImm);

        case ShiftType::LSR:
        return LogicalShiftRight(rm, shiftImm);

        case ShiftType::ASR:
        return ArithmeticShiftRight(rm, shiftImm);

        case ShiftType::ROR:
        if (shiftImm == 0)
        {
            return RotateRightExtendCarry(rm, cpu); // RRX
        }
        return RotateRight(rm, shiftImm);

        default: // Should never reach here, but to satisfy compiler
        return rm;
    }
}
