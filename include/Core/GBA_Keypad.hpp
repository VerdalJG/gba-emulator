#pragma once    
#include "Core/IO/GBA_IO_Helpers.hpp"

#include <cstdint>

class EmulatorCore;

class GBA_Keypad
{
public:
    GBA_Keypad() = delete;
    explicit GBA_Keypad(EmulatorCore* core);
    ~GBA_Keypad() = default;

private:
    EmulatorCore* core;
    //IO_KeypadRegisters keypadRegisters;
};