#pragma once
#include <array>

#include "Utils/Meta.hpp"
#include "Core/GBA_CPU.hpp"

using Handler_ARM = GBA_CPU::Handler_ARM;
using Handler_Thumb = GBA_CPU::Handler_Thumb;

struct TableGenerator
{
    //#include "Core/CPU/Instructions/ARM/Handler.hpp"
    #include "Core/CPU/Instructions/Thumb/Handler.hpp"

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

        static_for<std::size_t, 0, 1024>([&](auto i)
        {
            table[i] = GenerateHandlerThumb<i << 6>();
        });

        for (int i = 0; i < 1024; i++)
        {
            //table[i] = GenerateHandlerThumb(static_cast<u16>(i << 6));
        }

        return table;
    }
};
