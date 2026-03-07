#include "Core/GBA_CPU.hpp"
#include "Core/CPU/ARMOpcodes.hpp"

#include "Utils/BitOperations.hpp"

ARM_Opcode GBA_CPU::Decode_ARM(u32 instruction)
{
    switch (ExtractBits<27, 26>(instruction))
    {
        case 0b00: return Decode_ARM_Pattern00(instruction);
        case 0b01: return Decode_ARM_Pattern01(instruction);
        case 0b10: return Decode_ARM_Pattern10(instruction);
        case 0b11: return Decode_ARM_Pattern11(instruction);
        default: return ARM_Opcode::ARM_Invalid;
    }
}

ARM_Opcode GBA_CPU::Decode_ARM_Pattern00(u32 instruction)
{
    if (IsBitSet<25>(instruction))
    {
        // ARM 1. PSR transfer immediate
        if (ExtractBits<24, 23>(instruction) == 0b10 && !IsBitSet<20>(instruction))
        {
            return ARM_Opcode::ARM_PSRTransfer; // Immediate
        }

        // ARM 2. Data processing immediate op2
        return ARM_Opcode::ARM_DataProcessing;
    } 

    if ((ExtractBits<7, 4>(instruction) == 0b1001))
    {
        // ARM 3. Multiply (MUL)
        if (ExtractBits<24, 22>(instruction) == 0b000)
        {
            return ARM_Opcode::ARM_Multiply;
        }

        // ARM 4. Multiply long (MULL/MLA/UMLAL/UMULL)
        if (ExtractBits<24, 23>(instruction) == 0b01)
        {
            return ARM_Opcode::ARM_MultiplyLong;
        }

        // ARM 5. Single data swap (SWP/SWPB)
        if ((instruction >> 20) & 0b11011 == 0b10000 && ExtractBits<11, 8>(instruction) == 0)
        {
            return ARM_Opcode::ARM_SingleDataSwap;
        }
    }

    // ARM 6. Halfword data transfer
    if (IsBitSet<7>(instruction) && IsBitSet<4>(instruction))
    {
        if (IsBitSet<22>(instruction))
        {
            return ARM_Opcode::ARM_HalfwordDataTransfer; // Immediate offset
        }
        else
        {
            if (ExtractBits<11, 8>(instruction) == 0)
            {
                return ARM_Opcode::ARM_HalfwordDataTransfer; // Register offset
            }
        }
    }

    // ARM 7. Branch and exchange (BX) and Branch (B)
    if (ExtractBits<25, 4>(instruction) == 0x12FFF1)
    {
        return ARM_Opcode::ARM_BranchAndExchange;
    }

    // ARM 8. PSR transfer register
    if (ExtractBits<24, 23>(instruction) == 0b10 && ExtractBits<11, 4>(instruction) == 0 && !IsBitSet<20>(instruction))
    {
        return ARM_Opcode::ARM_PSRTransfer; // Register
    }

    // ARM 9. Data processing register shift by immediate
    if (!IsBitSet<4>(instruction))
    {
        return ARM_Opcode::ARM_DataProcessing; // Shift by immediate
    }
    else
    {
        // ARM 10. Data processing register shift by register
        if (!IsBitSet<7>(instruction))
        {
            return ARM_Opcode::ARM_DataProcessing; // Shift by register
        }
    }

    // Fallback to undefined instruction
    return ARM_Invalid;
}

ARM_Opcode GBA_CPU::Decode_ARM_Pattern01(u32 instruction)
{
    // ARM 11. Undefined instruction
    if (IsBitSet<25>(instruction) && IsBitSet<4>(instruction))
    {
        return ARM_Opcode::ARM_UndefinedInstruction;
    }
    else // ARM 12. Single data transfer (LDR/STR)
    {
        return ARM_Opcode::ARM_SingleDataTransfer;
    }
    
    // Fallback to undefined instruction
    return ARM_Invalid;
}

ARM_Opcode GBA_CPU::Decode_ARM_Pattern10(u32 instruction)
{
    // ARM 13. Branch and link
    if (IsBitSet<25>(instruction))
    {
        return ARM_Opcode::ARM_Branch;
    }
    else // ARM 14. Block data transfer (LDM/STM)
    {
        return ARM_Opcode::ARM_BlockDataTransfer;
    }

    // Fallback to undefined instruction
    return ARM_Invalid;
}

ARM_Opcode GBA_CPU::Decode_ARM_Pattern11(u32 instruction)
{
    if (IsBitSet<25>(instruction))
    {
        // ARM 15. Software interrupt
        if (IsBitSet<24>(instruction))
        {
            return ARM_Opcode::ARM_SoftwareInterrupt;
        }
        else
        {
            // ARM 16. Coprocessor data operation (maps to undefined instruction in GBA)
            // ARM 17. Coprocessor register transfer (maps to undefined instruction in GBA)
            return ARM_Opcode::ARM_Coprocessor;
        }
    }
    else
    {
        // ARM 18. Coprocessor data transfer (maps to undefined instruction in GBA)
        return ARM_Opcode::ARM_Coprocessor;
    }
   
    // Fallback to undefined instruction
    return ARM_Opcode::ARM_Invalid;
}
