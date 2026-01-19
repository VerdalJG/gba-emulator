#pragma once
#include <cstdint>
#include "Core/CPU/Instructions/Shifts.hpp"
#include "Core/CPU/Instructions/LoadStore.hpp"

class GBA_CPU;

uint32_t CalculateAddress_AddressingMode2(SingleDataTransfer_Decoded values, GBA_CPU& cpu);

uint32_t CalculateOffset_AddressingMode2(uint32_t offsetBits, bool isRegister, GBA_CPU& cpu);
uint32_t CalculateOffset_Register(uint32_t offsetBits, GBA_CPU& cpu);
uint32_t CalculateOffset_ScaledRegister(uint32_t offsetBits, GBA_CPU& cpu);

uint32_t CalculateScaledRegister(uint32_t rm, ShiftType shift, uint32_t shiftImm, GBA_CPU& cpu);