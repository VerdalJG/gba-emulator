#pragma once
#include <cstdint>
#include "Core/CPU/Instructions/Conditions.hpp"

class GBA_CPU;

struct Multiply_Decoded
{
    Condition condition;
    uint8_t rdIndex, rnIndex, rsIndex, rmIndex;
    bool accumulateFlag, setCPSRFlag;
};

struct MultiplyLong_Decoded
{
    Condition condition;
    uint8_t rdHiIndex, rdLoIndex, rsIndex, rmIndex;
    bool signedFlag, accumulateFlag, setCPSRFlag;
};

struct SingleDataSwap_Decoded
{
    Condition condition;
    uint8_t rnIndex, rdIndex, rmIndex;
    bool bFlag;
};

void Multiply(uint32_t instruction, GBA_CPU& cpu);
void MultiplyLong(uint32_t instruction, GBA_CPU& cpu);

void SMULL(uint32_t& rdLo, uint32_t& rdHi, int64_t& product);
void SMLAL(uint32_t& rdLo, uint32_t& rdHi, int64_t& product);
void UMULL(uint32_t& rdLo, uint32_t& rdHi, uint64_t& product);
void UMLAL(uint32_t& rdLo, uint32_t& rdHi, uint64_t& product);

void SingleDataSwap(uint32_t instruction, GBA_CPU& cpu);

Multiply_Decoded Multiply_Decode(uint32_t instruction);
MultiplyLong_Decoded MultiplyLong_Decode(uint32_t instruction);
SingleDataSwap_Decoded SingleDataSwap_Decode(uint32_t instruction);
