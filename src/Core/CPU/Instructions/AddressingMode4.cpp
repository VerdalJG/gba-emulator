#include "Core/CPU/Instructions/AddressingMode4.hpp"
#include "Core/CPU/Instructions/InstructionHelpers.hpp"

#include "Core/CPU/GBA_CPU.hpp"

AddressingMode4 CalculateAddressingMode4(BlockDataTransfer_Decoded values, GBA_CPU &cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t amountOfRegisters = NumberOfSetBitsIn(values.registerList);

    if (!values.pFlag && values.uFlag)
    {
        return IncrementAfter(rn, amountOfRegisters);
    }
    else if (values.pFlag && values.uFlag)
    {
        return IncrementBefore(rn, amountOfRegisters);
    }
    else if (!values.pFlag && !values.uFlag)
    {
        return DecrementAfter(rn, amountOfRegisters);
    }
    else // p && !u
    {
        return DecrementBefore(rn, amountOfRegisters);
    }
}
AddressingMode4 IncrementAfter(uint32_t rn, uint32_t amountOfRegisters)
{
    uint32_t startAddress = rn;
    uint32_t endAddress = rn + (amountOfRegisters * 4) - 4;
    uint32_t writebackValue = rn + amountOfRegisters * 4;
    return {startAddress, endAddress, writebackValue};
}

AddressingMode4 IncrementBefore(uint32_t rn, uint32_t amountOfRegisters)
{
    uint32_t startAddress = rn + 4;
    uint32_t endAddress = rn + (amountOfRegisters * 4);
    uint32_t writebackValue = endAddress;
    return {startAddress, endAddress, writebackValue};
}

AddressingMode4 DecrementAfter(uint32_t rn, uint32_t amountOfRegisters)
{
    uint32_t startAddress = rn - (amountOfRegisters * 4) + 4;
    uint32_t endAddress = rn;
    uint32_t writebackValue = rn - (amountOfRegisters * 4);
    return {startAddress, endAddress, writebackValue};
}

AddressingMode4 DecrementBefore(uint32_t rn, uint32_t amountOfRegisters)
{
    uint32_t startAddress = rn - (amountOfRegisters * 4);
    uint32_t endAddress = rn - 4;
    uint32_t writebackValue = startAddress;
    return {startAddress, endAddress, writebackValue};
}
