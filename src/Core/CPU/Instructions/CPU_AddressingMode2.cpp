#include "Core/CPU/Instructions/CPU_AddressingMode2.hpp"
#include "Core/CPU/GBA_CPU.hpp"
#include <assert.h>

uint32_t CalculateAddress_AddressingMode2(SingleDataTransfer_Decoded values, GBA_CPU& cpu)
{
    bool isRegister = values.iFlag;

    if (isRegister)
    {   
        // Differentiate between shifted register and normal register offset
        bool isScaledRegister = (values.offsetBits >> 4); 

        if (isScaledRegister)
        {
            return AddressingMode2_ScaledRegisterOffset(values, cpu);
        }
        else
        {
            return AddressingMode2_RegisterOffset(values, cpu);
        }
    }
    else // Immediate
    {
        return AddressingMode2_ImmediateOffset(values, cpu);
    }
}

uint32_t AddressingMode2_ImmediateOffset(SingleDataTransfer_Decoded values, GBA_CPU &cpu)
{
    // Calculate address
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    bool usingPC = values.rnIndex == 15;

    uint32_t offset12 = values.offsetBits;
    uint32_t resultAddress = values.uFlag ? rn + offset12 : rn - offset12;

    if (values.pFlag && values.wFlag) // Pre-indexed immediate offset
    {
        if (usingPC) 
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }

        cpu.SetValueAtRegister(values.rnIndex, resultAddress);
        return resultAddress;
    }
    else if (values.pFlag) // Immediate offset
    {
        return resultAddress;
    }
    else // Post-indexed immediate offset
    {
        if (usingPC) 
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }

        return resultAddress; // Update happens after operation
    }
}

uint32_t AddressingMode2_RegisterOffset(SingleDataTransfer_Decoded values, GBA_CPU &cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rmIndex =  values.offsetBits & 0xF;
    uint32_t rm = cpu.GetValueAtRegister(rmIndex);

    bool rnUsingPC = values.rnIndex == 15;
    bool rmUsingPC = rmIndex == 15;

    uint32_t resultAddress = values.uFlag ? (rn + rm) : (rn - rm);
    if (values.pFlag && values.wFlag) // Register offset pre-indexed
    {
        if (rnUsingPC || rmUsingPC || values.rnIndex == rmIndex) 
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }
        
        cpu.SetValueAtRegister(values.rnIndex, resultAddress);
        return resultAddress;
    }
    else if (values.pFlag) // Register offset
    {
        if (rmUsingPC) 
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }
        return resultAddress; 
    }
    else // Register offset post-indexed
    {
        if (rnUsingPC || rmUsingPC || values.rnIndex == rmIndex) 
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }

        return resultAddress; // Update happens after operation
    }
}

uint32_t AddressingMode2_ScaledRegisterOffset(SingleDataTransfer_Decoded values, GBA_CPU &cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rmIndex =  values.offsetBits & 0xF;
    uint32_t rm = cpu.GetValueAtRegister(rmIndex);

    ShiftType shiftType = static_cast<ShiftType>((values.offsetBits >> 5) & 3);
    uint32_t shiftImm = (values.offsetBits >> 7) & 0x1F;
    uint32_t offset = CalculateScaledRegister(rm, shiftType, shiftImm, cpu);
    uint32_t resultAddress = values.uFlag ? rn + offset : rn - offset;

    bool rnUsingPC = values.rnIndex == 15;
    bool rmUsingPC = rmIndex == 15;
    
    if (values.pFlag && values.wFlag) // Scaled register pre-indexed
    {
        if (rnUsingPC || rmUsingPC || values.rnIndex == rmIndex) 
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }

        cpu.SetValueAtRegister(values.rnIndex, resultAddress);
        return resultAddress;
    }
    else if (values.pFlag) // Scaled register offset
    {
        if (rmUsingPC)
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }
        
        return resultAddress;
    }
    else // Scaled register post-indexed
    {
        if (rnUsingPC || rmUsingPC || values.rnIndex == rmIndex) 
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }

        return resultAddress;
    }
}

uint32_t CalculateScaledRegister(uint32_t rm, ShiftType shift, uint32_t shiftImm, GBA_CPU& cpu)
{
    switch (shift)
    {
        case ShiftType::LSL:
        return LogicalLeft(rm, shiftImm);

        case ShiftType::LSR:
        return LogicalRight(rm, shiftImm);

        case ShiftType::ASR:
        return ArithmeticRight(rm, shiftImm);

        case ShiftType::ROR:
        if (shiftImm == 0)
        {
            return RotateRightExtendCarry(rm, cpu);
        }
        return RotateRight(rm, shiftImm);

        default: // Should never reach here, but to satisfy compiler
        return rm;
    }
}
