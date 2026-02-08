#pragma once
#include "Core/GBA_CPU.hpp"
#include "Utils/Integer.hpp"
#include "Utils/BitOperations.hpp"

// TODO (WAY LATER): maybe do compile time look up table

using Handler_Thumb = GBA_CPU::Handler_Thumb;

template <u16 instruction>
static constexpr Handler_Thumb GenerateHandlerThumb()
{
    // 1. Move shifted register
    if (ExtractBits<15, 10, u16>(instruction) < 0b11000)
    {
        const u16 shiftOp = ExtractBits<12, 11, u16>;
        const u16 offset_5 = (instruction >> 6) & 0x1F;

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
}
