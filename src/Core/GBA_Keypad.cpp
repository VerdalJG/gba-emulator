#include "Core/GBA_Keypad.hpp"

GBA_Keypad::GBA_Keypad(EmulatorCore* core, IO_KeypadRegisters& keypadRegisters) : 
    core(core), keypadRegisters(keypadRegisters)
{

}