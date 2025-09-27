#pragma once
#include <cstdint>
#include "Core/CPU/Instructions/InstructionHelpers.hpp"

class GBA_CPU;

void HalfwordDataTransfer(uint32_t instruction, GBA_CPU& cpu);
void SingleDataTransfer(uint32_t instruction, GBA_CPU& cpu);
void BlockDataTransfer(uint32_t instruction, GBA_CPU& cpu);

void LDM(BlockDataTransfer_Decoded values, GBA_CPU& cpu);
void LDMUserRegisters(BlockDataTransfer_Decoded values, GBA_CPU& cpu);
void LDMRestoreCPSR(BlockDataTransfer_Decoded values, GBA_CPU& cpu);
void LDMEmptyRegisterList(BlockDataTransfer_Decoded values, GBA_CPU& cpu);

void STM(BlockDataTransfer_Decoded values, GBA_CPU& cpu);
void STMUserRegisters(BlockDataTransfer_Decoded values, GBA_CPU& cpu);
void STMEmptyRegisterList(BlockDataTransfer_Decoded values, GBA_CPU& cpu);



