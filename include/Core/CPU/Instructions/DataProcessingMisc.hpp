#pragma once
#include <cstdint>

class GBA_CPU;

void Multiply(uint32_t instruction, GBA_CPU& cpu);
void MultiplyLong(uint32_t instruction, GBA_CPU& cpu);

void SMULL(uint32_t& rdLo, uint32_t& rdHi, int64_t& product);
void SMLAL(uint32_t& rdLo, uint32_t& rdHi, int64_t& product);
void UMULL(uint32_t& rdLo, uint32_t& rdHi, uint64_t& product);
void UMLAL(uint32_t& rdLo, uint32_t& rdHi, uint64_t& product);

void SingleDataSwap(uint32_t instruction, GBA_CPU& cpu);
