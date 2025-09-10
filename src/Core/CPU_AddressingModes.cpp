#include "Core/GBA_CPU.hpp"

uint32_t GBA_CPU::CalculateAddressMode2(SingleDataTransfer_Decoded decodedValues)
{
    if (decodedValues.iFlag) // Register
    {        
        if (decodedValues.pFlag)
        {
            
        }
        else
        {

        }
    }
    else // Immediate
    {
        if (decodedValues.pFlag)
        {
            
        }
        else
        {

        }
    }
}