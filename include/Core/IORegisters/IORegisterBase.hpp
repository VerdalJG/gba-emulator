#pragma once

#include "Core/GBA_Bus.hpp"

#include "Utils/Integers.hpp"

#include <string>


struct IORegisterBase
{
    std::string name;
    u32 address;
    AccessSize width;
    bool readable; // Refers to the cpu's ability to read the IO register
    bool writeable; // Refers to the cpu's ability to write the IO register
};