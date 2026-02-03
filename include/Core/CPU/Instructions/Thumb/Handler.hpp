#include "Utils/Integer.hpp"


// TODO (WAY LATER): maybe do compile time look up table

template <u16 instruction>
static constexpr Handler_Thumb GenerateHandlerThumb()
{
    // 1. Move shifted register
    if ((instruction & 0xF800) < 0x1800)
    {
        const auto opcode = (instruction >> 11) & 3;
        const auto offset5 = (instruction >> 6) & 0x1F;
        return &GBA_CPU::Thumb_MoveShiftedRegister<opcode, offset_5>;
    }
}
