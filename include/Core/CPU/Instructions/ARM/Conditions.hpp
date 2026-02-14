#pragma once
#include <cstdint>

class GBA_CPU;

// These are in order, from 0-14, Condition #15 is undefined behavior
enum Condition
{
    //Suffix                    Description                     Condition
    CONDITION_EQ = 0,           // Equal                        - Z set
    CONDITION_NE = 1,           // Not Equal                    - Z clear
    CONDITION_CS = 2,           // Unsigned higher, or same     - C set
    CONDITION_CC = 3,           // Unsigned lower               - C clear
    CONDITION_MI = 4,           // Negative                     - N set
    CONDITION_PL = 5,           // Positive or Zero             - N clear
    CONDITION_VS = 6,           // Overflow                     - V set
    CONDITION_VC = 7,           // No overflow                  - V clear
    CONDITION_HI = 8,           // Unsigned higher              - C set, Z clear
    CONDITION_LS = 9,           // Unsigned lower, or same      - C clear, Z set
    CONDITION_GE = 10,          // Greater, or equal            - N = V
    CONDITION_LT = 11,          // Less than                    - N <> V (N not equal to V)
    CONDITION_GT = 12,          // Greater than                 - Z clear, N = V
    CONDITION_LE = 13,          // Less than, or equal          - Z set, N <> V
    CONDITION_AL = 14,          // Always                       - No condition
    CONDITION_UD = 15           // Undefined                    - Undefined
};

Condition GetConditionType(uint32_t instruction);

