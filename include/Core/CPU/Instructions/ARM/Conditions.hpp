#pragma once
#include "Utils/Integers.hpp"

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
    CONDITION_NV = 15           // Unpredictable (no-op)        - Unpredictable
};

inline Condition GetConditionType(u32 instruction)
{
    return static_cast<Condition>(instruction >> 28);
}

static constexpr std::array<bool, 256> GenerateConditionTable()
{
    std::array<bool, 256> table = {};

    for (int flags = 0; flags < 16; flags++)
    {
        bool n = flags & 8;
        bool z = flags & 4;
        bool c = flags & 2;
        bool v = flags & 1;

        table[(CONDITION_EQ << 4 | flags)] = z;
        table[(CONDITION_NE << 4 | flags)] = !z;
        table[(CONDITION_CS << 4 | flags)] = c;
        table[(CONDITION_CC << 4 | flags)] = !c;
        table[(CONDITION_MI << 4 | flags)] = n;
        table[(CONDITION_PL << 4 | flags)] = !n;
        table[(CONDITION_VS << 4 | flags)] = v;
        table[(CONDITION_VC << 4 | flags)] = !v;
        table[(CONDITION_HI << 4 | flags)] = c && !z;
        table[(CONDITION_LS << 4 | flags)] = !c || z;
        table[(CONDITION_GE << 4 | flags)] = n == v;
        table[(CONDITION_LT << 4 | flags)] = n != v;
        table[(CONDITION_GT << 4 | flags)] = !z && (n == v);
        table[(CONDITION_LE << 4 | flags)] = z || (n != v);
        table[(CONDITION_AL << 4 | flags)] = true;
        table[(CONDITION_NV << 4 | flags)] = false;
    }

    return table;
}

