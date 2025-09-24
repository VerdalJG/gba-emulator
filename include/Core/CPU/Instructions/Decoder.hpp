#pragma once
#include "Core/CPU/GBA_CPU.hpp"

#include <cstdint>


inline uint8_t GetConditionBits(uint32_t instruction) // Only ARM mode uses condition bits
{
    return (instruction >> 28) & 0xF;
}

InstructionFunction DecodePattern00(uint32_t instruction, GBA_CPU& cpu);
InstructionFunction DecodePattern01(uint32_t instruction, GBA_CPU& cpu);
InstructionFunction DecodePattern10(uint32_t instruction, GBA_CPU& cpu);
InstructionFunction DecodePattern11(uint32_t instruction, GBA_CPU& cpu);
