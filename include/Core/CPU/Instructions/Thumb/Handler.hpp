#pragma once
#include "Core/GBA_CPU.hpp"
#include "Utils/Integers.hpp"
#include "Utils/BitOperations.hpp"

template <u16 instruction>
static constexpr Handler_Thumb GenerateHandlerThumb()
{
    // 1. Move shifted register
    if (ExtractBits<15, 10, u16>(instruction) < 0b11000)
    {
        return &GBA_CPU::Thumb_MoveShiftedRegister<shiftOp, offset_5>;
    }

    // 2. Add/Subtract
    if (ExtractBits<15, 10, u16>(instruction) == 0b11000)
    {
        const bool immediate = instruction >> 10;
        const bool subtract  = instruction >> 9;
        const u16 operand = ExtractBits<8, 6, u16>(instruction);

        return &GBA_CPU::Thumb_AddSubtract<subtract, immediate, operand>;
    }

    // 3. Move/Compare/Add/Subtract by Immediate
    if (ExtractBits<15, 13, u16>(instruction) == 0b001)
    {
        const u16 opcode = ExtractBits<12, 11, u16>(instruction);
        const u16 rdIndex = ExtractBits<10, 8, u16>(instruction);

        return &GBA_CPU::Thumb_ImmediateOp<opcode, rdIndex>;
    }

    // 4. ALU Operations
    if (ExtractBits<15, 10, u16>(instruction) == 0b010000)
    {
        const u16 opcode = ExtractBits<9, 6, u16>(instruction);

        return &GBA_CPU::Thumb_ALU<opcode>;
    }

    // 5. High register operations / Branch Exchange (BX)
    if (ExtractBits<15, 10, u16>(instruction) == 0b010001)
    {
        const u16 opcode = ExtractBits<9, 8, u16>(instruction);
        const u16 msbRd = (instruction >> 7) & 1;
        const u16 msbRs = (instruction >> 6) & 1;

        return &GBA_CPU::Thumb_HiRegisterOp<opcode, msbRd, msbRs>;
    }

    // 6. Load PC-Relative (for loading immediates from literal pool)
    if (ExtractBits<15, 11, u16>(instruction) == 0b01001)
    {
        const u16 rdIndex = ExtractBits<10, 8, u16>(instruction);

        return &GBA_CPU::Thumb_LoadPCRelative<rdIndex>;
    }

    // 7. Load/Store with register offset
    if (ExtractBits<15, 12, u16>(instruction) == 0b0101 && !IsBitSet<9>(instruction))
    {
        const u16 opcode = ExtractBits<11, 10, u16>(instruction);
        const u16 roIndex = ExtractBits<8, 6, u16>(instruction);

        return &GBA_CPU::Thumb_LoadStoreRegisterOffset<opcode, roIndex>;
    }

    // 8. Load/Store sign-extended byte/halfword
    if (ExtractBits<15, 12, u16>(instruction) == 0b0101 && IsBitSet<9>(instruction))
    {
        const u16 opcode = ExtractBits<11, 10, u16>(instruction);
        const u16 roIndex = ExtractBits<8, 6, u16>(instruction);

        return &GBA_CPU::Thumb_LoadStoreSignExtended<opcode, roIndex>;
    }

    // 9. Load/Store with immediate offset
    if (ExtractBits<15, 13, u16>(instruction) == 0b011)
    {
        const u16 opcode = ExtractBits<12, 11, u16>(instruction);
        const u16 offset_5 = ExtractBits<10, 6, u16>(instruction);

        return &GBA_CPU::Thumb_LoadStoreImmediateOffset<opcode, offset_5>;
    }

    // 10. Load/Store halfword
    if (ExtractBits<15, 12, u16>(instruction) == 0b1000)
    {
        const bool load = IsBitSet<11>(instruction);
        const u16 offset_5 = ExtractBits<10, 6, u16>(instruction);

        return &GBA_CPU::Thumb_LoadStoreHalfword<load, offset_5>;
    }

    // 11. Load/Store SP-relative
    if (ExtractBits<15, 12, u16>(instruction) == 0b1001)
    {
        const bool load = IsBitSet<11>(instruction);
        const u16 rdIndex = ExtractBits<10, 8, u16>(instruction);

        return &GBA_CPU::Thumb_LoadStoreSPRelative<load, rdIndex>;
    }

    // 12. Get relative address
    if (ExtractBits<15, 12, u16>(instruction) == 0b1010)
    {
        const bool getSP = IsBitSet<11>(instruction);
        const u16 rdindex = ExtractBits<10, 8, u16>(instruction);

        return &GBA_CPU::Thumb_GetRelativeAddress<getSP, rdindex>;
    }

    // 13. Add offset to stack pointer (SP) - R13
    if (ExtractBits<15, 8, u16>(instruction) == 0b10110000)
    {
        const bool sub = IsBitSet<7>(instruction);

        return &GBA_CPU::Thumb_AddOffsetToStackPointer<sub>;
    }

    // 14. Push/pop registers
    if (ExtractBits<15, 12, u16>(instruction) == 0b1011 && ExtractBits<10, 9, u16>(instruction) == 0b10)
    {
        bool pop = IsBitSet<11>(instruction);
        bool bit_pc_lr = IsBitSet<8>(instruction);

        return &GBA_CPU::Thumb_PushPopRegisters<pop, bit_pc_lr>;
    }

    // 15. Load/store multiple
    if (ExtractBits<15, 12, u16>(instruction) == 0b1100)
    {
        bool load = IsBitSet<11>(instruction);
        u16 rbIndex = ExtractBits<10, 8, u16>(instruction);

        return &GBA_CPU::Thumb_LoadStoreMultiple<load, rbIndex>;
    }

    // 16. Conditional branch
    if (ExtractBits<15, 12, u16>(instruction) < 0b11011111)
    {
        u16 condition = ExtractBits<11, 8, u16>(instruction);

        return &GBA_CPU::Thumb_ConditionalBranch<condition>;
    }

    // 17. Software Interrupt
    if (ExtractBits<15, 8, u16>(instruction) == 0b11011111)
    {
        return &GBA_CPU::Thumb_SoftwareInterrupt;
    }

    // 18. Unconditional branch
    if (ExtractBits<15, 11, u16>(instruction) == 0b11100)
    {
        return &GBA_CPU::Thumb_UnconditionalBranch;
    }

    // 19. Long branch with link
    if (ExtractBits<15, 12, u16>(instruction) == 0b1111)
    {
        const bool secondInstruction = IsBitSet<11>(instruction);

        return &GBA_CPU::Thumb_LongBranchWithLink<secondInstruction>;
    }
}
