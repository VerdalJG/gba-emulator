#pragma once
#include <cstdint>
#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Instructions/ARM/LoadStore.hpp"

uint32_t CalculateAddress_AddressingMode3(HalfwordDataTransfer_Decoded values, GBA_CPU& cpu);

// HDT is HalfwordDataTransfer
inline uint32_t GetHDTOffset_Immediate(uint16_t offsetBits)
{
    return ((offsetBits >> 4) & 0xF0) | (offsetBits & 0xF);
}

// HDT is HalfwordDataTransfer
inline uint32_t GetHDTOffset_Register(uint16_t offsetBits, GBA_CPU& cpu)
{
    return cpu.ReadRegister(offsetBits & 0xF);
}