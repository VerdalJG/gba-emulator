#pragma once
#include <cstdint>

class GBA_CPU;

void Branch(uint32_t instruction, GBA_CPU& cpu);
void BranchAndExchange(uint32_t instruction, GBA_CPU& cpu);