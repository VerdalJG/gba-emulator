#pragma once
#include <cstdint>

class GBA_CPU;

// These are in order, from 0-14, Condition #15 is undefined behavior
enum class Condition
{
    //Suffix        Description                     Condition
    EQ,             // Equal                        - Z set
    NE,             // Not Equal                    - Z clear
    CS,             // Unsigned higher, or same     - C set
    CC,             // Unsigned lower               - C clear
    MI,             // Negative                     - N set
    PL,             // Positive or Zero             - N clear
    VS,             // Overflow                     - V set
    VC,             // No overflow                  - V clear
    HI,             // Unsigned higher              - C set, Z clear
    LS,             // Unsigned lower, or same      - C clear, Z set
    GE,             // Greater, or equal            - N = V
    LT,             // Less than                    - N <> V (N not equal to V)
    GT,             // Greater than                 - Z clear, N = V
    LE,             // Less than, or equal          - Z set, N <> V
    AL,             // Always                       - No condition
    UD              // Undefined                    - Undefined
};

Condition GetConditionType(uint32_t instruction);
bool ConditionPassed(Condition condition, GBA_CPU &cpu);

