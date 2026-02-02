#pragma once
#include <cstdint>

class GBA_CPU;

// These are in order, from 0-14, Condition #15 is undefined behavior
enum class Condition
{
    //Suffix            Description                     Condition
    EQ = 0,             // Equal                        - Z set
    NE = 1,             // Not Equal                    - Z clear
    CS = 2,             // Unsigned higher, or same     - C set
    CC = 3,             // Unsigned lower               - C clear
    MI = 4,             // Negative                     - N set
    PL = 5,             // Positive or Zero             - N clear
    VS = 6,             // Overflow                     - V set
    VC = 7,             // No overflow                  - V clear
    HI = 8,             // Unsigned higher              - C set, Z clear
    LS = 9,             // Unsigned lower, or same      - C clear, Z set
    GE = 10,            // Greater, or equal            - N = V
    LT = 11,            // Less than                    - N <> V (N not equal to V)
    GT = 12,            // Greater than                 - Z clear, N = V
    LE = 13,            // Less than, or equal          - Z set, N <> V
    AL = 14,            // Always                       - No condition
    UD = 15             // Undefined                    - Undefined
};

Condition GetConditionType(uint32_t instruction);
bool ConditionPassed(Condition condition, GBA_CPU &cpu);

