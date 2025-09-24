#pragma once
#include <cstdint>

class GBA_CPU;
    
void UndefinedInstruction(uint32_t instruction, GBA_CPU& cpu);
void SoftwareInterrupt(uint32_t instruction, GBA_CPU& cpu);

// Information on this is in section 2.6 of ARM architecture manual - Exceptions