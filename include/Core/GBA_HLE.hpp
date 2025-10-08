#pragma once
#include <cstdint>
#include <functional>
#include <array>
#include <string>


class EmulatorCore;
class GBA_CPU;

class GBA_HLE
{
public:
    explicit GBA_HLE(EmulatorCore* core);

    void HandleSWI(uint8_t swiNumber, GBA_CPU& cpu);

private:
    EmulatorCore* core;

    using SWIFunction = std::function<void(GBA_CPU& cpu)>;
    std::array<SWIFunction, 32> swiTable; // 0x00 to 0x1F

    // HLE implementations:
    static void HLE_SoftReset(GBA_CPU& cpu);
    static void HLE_RegisterRamReset(GBA_CPU& cpu);
    static void HLE_VBlankIntrWait(GBA_CPU& cpu);
    static void HLE_Div(GBA_CPU& cpu);

    void BuildTable();
};