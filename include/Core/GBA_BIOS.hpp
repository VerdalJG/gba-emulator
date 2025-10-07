#pragma once
#include <cstdint>

class GBA_CPU;

class GBA_BIOS
{
public:
    void HandleSoftwareInterrupt(uint8_t interruptNumber, GBA_CPU& cpu);

};