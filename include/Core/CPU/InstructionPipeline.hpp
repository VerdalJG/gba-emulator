#pragma once
#include "Utils/Integers.hpp"

#include <array>

class GBA_CPU;

using ARM_Handler = void (GBA_CPU::*)(u32);
using Thumb_Handler = void (GBA_CPU::*)(u16);

struct PipelineStage
{
    u32 rawBits;
    u8 opcode; // Corresponds to ARM_Instruction and Thumb_Instruction types
    bool valid;
};

struct Pipeline
{
    std::array<PipelineStage, 3> stage{}; // [0] = fetch, [1] = decode, [2] = execute
    int access;
};