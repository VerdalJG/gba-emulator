#pragma once    
#include "Core/GBA_IO_Helpers.hpp"

#include <cstdint>

class EmulatorCore;

class GBA_Keypad
{
public:
    GBA_Keypad() = delete;
    explicit GBA_Keypad(EmulatorCore* core, IO_KeypadRegisters& keypadRegisters);
    ~GBA_Keypad() = default;

private:
    EmulatorCore* core;
    IO_KeypadRegisters& keypadRegisters;
};