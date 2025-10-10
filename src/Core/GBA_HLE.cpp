#include "Core/GBA_HLE.hpp"
#include "Core/EmulatorCore.hpp"

GBA_HLE::GBA_HLE(EmulatorCore *core) : 
core(core)
{
}

void GBA_HLE::HandleSWI(uint8_t swiNumber, GBA_CPU &cpu)
{
    if (swiNumber >= swiTable.size())
    {
        core->PostStatus("Unknown SWI: " + swiNumber);
        return;
    }

    auto& swiFunc = swiTable[swiNumber];
    if (swiFunc)
    {
        swiFunc(cpu);
    }
    else
    {
        core->PostStatus("Unimplemented SWI: " + swiNumber);
    }
}

void GBA_HLE::HLE_SoftReset(GBA_CPU &cpu)
{
}

void GBA_HLE::HLE_RegisterRamReset(GBA_CPU &cpu)
{
}

void GBA_HLE::HLE_VBlankIntrWait(GBA_CPU &cpu)
{
}

void GBA_HLE::HLE_Div(GBA_CPU &cpu)
{
}

void GBA_HLE::BuildTable()
{
    swiTable = 
    {
        &HLE_SoftReset, // 0x01
        &HLE_RegisterRamReset, // 0x02
        &HLE_VBlankIntrWait, // 0x03
        &HLE_Div // 0x04
        // Add more soon
    };
}
