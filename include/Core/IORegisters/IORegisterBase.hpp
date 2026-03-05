#pragma once

#include "Core/GBA_Bus.hpp"

#include "Utils/Integers.hpp"

struct IORegisterBase
{
    u32 address;
    BusAccessSize width;
    bool readable;
    bool writeable;
};