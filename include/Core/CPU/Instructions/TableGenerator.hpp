#pragma once
#include "Core/CPU/Instructions/ARM/Handler.hpp"
#include "Core/CPU/Instructions/Thumb/Handler.hpp"

#include "Utils/StaticFor.hpp"

#include <array>

struct TableGenerator
{
    static std::array<Handler_ARM, 4096> GenerateTableARM()
    {
        std::array<Handler_ARM, 4096> table;

        for (int i = 0; i < 4096; i++)
        {
            //table[i] = 
        }

        return table;
    }

    static constexpr std::array<Handler_Thumb, 1024> GenerateTableThumb()
    {
        std::array<Handler_Thumb, 1024> table;

        //Const-time table generation
        static_for<std::size_t, 0, 1024>([&](auto i)
        {
            table[i] = GenerateHandlerThumb<i << 6>();
        });

        return table;
    }
};
