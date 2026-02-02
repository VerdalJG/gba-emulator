#include "Utils/Integer.hpp"


// TODO (WAY LATER): maybe do compile time look up table

static Handler_Thumb GenerateHandlerThumb(u16 instruction)
{
    // 1. Move shifted register
    if ((instruction & 0xF800) < 0x1800)
    {
        return &GBA_CPU::Thumb_MoveShiftedRegister;
    }
}
