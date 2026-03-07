#pragma once

#include "Core/GBA_Bus.hpp"

#include "Utils/Integers.hpp"

struct IORegisterBase
{
    u32 address;
    size_t width;
    bool readable;
    bool writeable;
};