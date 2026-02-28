#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Thumb/Opcodes.hpp"

#include "Utils/BitOperations.hpp"

Thumb_Opcode GBA_CPU::Decode_Thumb(u16 instruction)
{
    // 1. Move shifted register
    if (ExtractBits<15, 11>(instruction) < 0b00011)
    {
        return Thumb_Opcode::Thumb_MoveShiftedRegister;
    }

    // 2. Add/Subtract
    if (ExtractBits<15, 11>(instruction) == 0b00011)
    {
        return Thumb_Opcode::Thumb_AddSubtract;
    }

    // 3. Move/Compare/Add/Subtract by Immediate
    if (ExtractBits<15, 13>(instruction) == 0b001)
    {
        return Thumb_Opcode::Thumb_ImmediateOp;
    }

    // 4. ALU Operations
    if (ExtractBits<15, 10>(instruction) == 0b010000)
    {
        return Thumb_Opcode::Thumb_ALU;
    }

    // 5. High register operations / Branch Exchange (BX)
    if (ExtractBits<15, 10>(instruction) == 0b010001)
    {
        return Thumb_Opcode::Thumb_HiRegisterOp;
    }

    // 6. Load PC-Relative (for loading immediates from literal pool)
    if (ExtractBits<15, 11>(instruction) == 0b01001)
    {
        return Thumb_Opcode::Thumb_LoadPCRelative;
    }

    // 7. Load/Store with register offset
    if (ExtractBits<15, 12>(instruction) == 0b0101 && !IsBitSet<9>(instruction))
    {
        return Thumb_Opcode::Thumb_LoadStoreRegisterOffset;
    }

    // 8. Load/Store sign-extended byte/halfword
    if (ExtractBits<15, 12>(instruction) == 0b0101 && IsBitSet<9>(instruction))
    {
        return Thumb_Opcode::Thumb_LoadStoreSignExtended;
    }

    // 9. Load/Store with immediate offset
    if (ExtractBits<15, 13>(instruction) == 0b011)
    {
        return Thumb_Opcode::Thumb_LoadStoreImmediateOffset;
    }

    // 10. Load/Store halfword
    if (ExtractBits<15, 12>(instruction) == 0b1000)
    {
        return Thumb_Opcode::Thumb_LoadStoreHalfword;
    }

    // 11. Load/Store SP-relative
    if (ExtractBits<15, 12>(instruction) == 0b1001)
    {
        return Thumb_Opcode::Thumb_LoadStoreSPRelative;
    }

    // 12. Get relative address
    if (ExtractBits<15, 12>(instruction) == 0b1010)
    {
        return Thumb_Opcode::Thumb_GetRelativeAddress;
    }

    // 13. Add offset to stack pointer (SP) - R13
    if (ExtractBits<15, 8>(instruction) == 0b10110000)
    {
        return Thumb_Opcode::Thumb_AddOffsetToStackPointer;
    }

    // 14. Push/pop registers
    if (ExtractBits<15, 12>(instruction) == 0b1011 && ExtractBits<10, 9>(instruction) == 0b10)
    {
        return Thumb_Opcode::Thumb_PushPopRegisters;
    }

    // 15. Load/store multiple
    if (ExtractBits<15, 12>(instruction) == 0b1100)
    {
        return Thumb_Opcode::Thumb_LoadStoreMultiple;
    }

    // 16. Conditional branch
    if (ExtractBits<15, 12>(instruction) < 0b11011111)
    {
        return Thumb_Opcode::Thumb_ConditionalBranch;
    }

    // 17. Software Interrupt
    if (ExtractBits<15, 8>(instruction) == 0b11011111)
    {
        return Thumb_Opcode::Thumb_UnconditionalBranch;
    }

    // 18. Unconditional branch
    if (ExtractBits<15, 11>(instruction) == 0b11100)
    {
        return Thumb_Opcode::Thumb_LongBranchWithLink;
    }

    // 19. Long branch with link
    if (ExtractBits<15, 12>(instruction) == 0b11110)
    {
        return Thumb_Opcode::Thumb_SoftwareInterrupt;
    }

    return Thumb_Opcode::Thumb_Invalid;
}