#pragma once
#include <cstdint>

class GBA_CPU;

void HalfwordDataTransfer(uint32_t instruction, GBA_CPU& cpu);
void SingleDataTransfer(uint32_t instruction, GBA_CPU& cpu);
void BlockDataTransfer(uint32_t instruction, GBA_CPU& cpu);


