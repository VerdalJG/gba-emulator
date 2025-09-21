#pragma once
#include <cstdint>
#include "Core/CPU/Instructions/CPU_Shifts.hpp"
#include "Core/CPU/Instructions/InstructionHelpers.hpp"

class GBA_CPU;

uint32_t CalculateAddress_AddressingMode2(SingleDataTransfer_Decoded values, GBA_CPU& cpu);

uint32_t AddressingMode2_ImmediateOffset(SingleDataTransfer_Decoded values, GBA_CPU& cpu);
uint32_t AddressingMode2_RegisterOffset(SingleDataTransfer_Decoded values, GBA_CPU& cpu);
uint32_t AddressingMode2_ScaledRegisterOffset(SingleDataTransfer_Decoded values, GBA_CPU& cpu);

uint32_t CalculateScaledRegister(uint32_t rm, ShiftType shift, uint32_t shiftImm, GBA_CPU& cpu);