#pragma once


enum class OperatingMode
{
    User        = 0b10000,
    FIQ         = 0b10001, // Fast Interrupt
    IRQ         = 0b10010, // Interrupt
    Supervisor  = 0b10011,
    Abort       = 0b10111,
    Undefined   = 0b11011,
    System      = 0b11111
};

constexpr int BankIndex(OperatingMode mode) 
{
    switch (mode) 
    {
        case OperatingMode::FIQ:        return 0;
        case OperatingMode::IRQ:        return 1;
        case OperatingMode::Supervisor: return 2;
        case OperatingMode::Abort:      return 3;
        case OperatingMode::Undefined:  return 4;
        case OperatingMode::User:        
        case OperatingMode::System:     return 5;
        default:                        return -1; // User/System -> no bank
    }
}

inline bool IsUserOrSystemMode(OperatingMode mode)
{
    return mode == OperatingMode::User || mode == OperatingMode::System;
}

inline bool IsExceptionMode(OperatingMode mode)
{
    return  mode == OperatingMode::FIQ ||
            mode == OperatingMode::IRQ ||
            mode == OperatingMode::Supervisor ||
            mode == OperatingMode::Abort ||
            mode == OperatingMode::Undefined;
}