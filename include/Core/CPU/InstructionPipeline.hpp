#pragma once

#include "Utils/Integers.hpp"
#include "Core/GBA_Bus.hpp"
#include <array>

class GBA_CPU;

using InstructionHandler = void (GBA_CPU::*)(u32);

struct PipelineStage
{
    u32 instruction;
    InstructionHandler handler;
    bool valid;
};

struct Pipeline
{
    std::array<PipelineStage, 3> stage{}; // [0] = fetch, [1] = decode, [2] = execute
    Access nextAccess;
};