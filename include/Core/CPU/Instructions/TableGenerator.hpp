#pragma once
#include "Core/CPU/Instructions/ARM/Handler.hpp"
#include "Core/CPU/Instructions/Thumb/Handler.hpp"
#include "Core/CPU/Instructions/ARM/Conditions.hpp"

#include "Utils/StaticFor.hpp"

#include <array>

struct TableGenerator
{
    static constexpr std::array<Handler_ARM, 4096> GenerateTable_ARM()
    {
        std::array<Handler_ARM, 4096> table;

        for (int i = 0; i < 4096; i++)
        {
            //table[i] = 
        }

        return table;
    }

    static constexpr std::array<Handler_Thumb, 1024> GenerateTable_Thumb()
    {
        std::array<Handler_Thumb, 1024> table;

        //Const-time table generation
        static_for<std::size_t, 0, 1024>([&](auto i)
        {
            table[i] = GenerateHandlerThumb<i << 6>();
        });

        return table;
    }

    static constexpr std::array<bool, 256> GenerateTable_Condition()
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
            table[(CONDITION_UD << 4 | flags)] = false;
        }

        return table;
    }
};
