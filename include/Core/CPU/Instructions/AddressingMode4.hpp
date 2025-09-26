#pragma once
#include <cstdint>
#include "Core/CPU/Instructions/InstructionHelpers.hpp"

class GBA_CPU;

struct AddressingMode4
{
    uint32_t startAddress;
    uint32_t endAddress;
    uint32_t writebackValue;
};

AddressingMode4 CalculateAddressingMode4(BlockDataTransfer_Decoded values, GBA_CPU& cpu);

AddressingMode4 IncrementAfter(uint32_t rn, uint32_t amountOfRegisters);
AddressingMode4 IncrementBefore(uint32_t rn, uint32_t amountOfRegisters);
AddressingMode4 DecrementAfter(uint32_t rn, uint32_t amountOfRegisters);
AddressingMode4 DecrementBefore(uint32_t rn, uint32_t amountOfRegisters);

