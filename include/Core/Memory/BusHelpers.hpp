#pragma once

#include "Utils/Integers.hpp"

enum class BusRequester
{
    CPU,
    PPU,
    APU,
    DMA
};

enum Access : uint
{
    Nonsequential = 0,
    Sequential = 1,
    Code = 2, // Instruction fetch/prefetch
    Data = 4, // CPU Load/Store
    DMA = 8,  // DMA bus request
    Lock = 16
};