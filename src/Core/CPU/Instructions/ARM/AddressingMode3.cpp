#include "Core/CPU/Instructions/ARM/AddressingMode3.hpp"
#include "Core/CPU/Instructions/ARM/InstructionHelpers.hpp"
#include <assert.h>

uint32_t CalculateAddress_AddressingMode3(HalfwordDataTransfer_Decoded values, GBA_CPU &cpu)
{
    bool preIndexed = values.pFlag;
    bool writeback = values.wFlag;
    bool isLoad = values.lFlag;
    bool isImmediate = values.iFlag;

    uint32_t rn = cpu.ReadRegister(values.rnIndex);
    uint32_t offset = isImmediate ? GetHDTOffset_Immediate(values.offsetBits) : GetHDTOffset_Register(values.offsetBits, cpu);
    uint32_t resultAddress;

    if (preIndexed) // P = 1
    {
        resultAddress = values.uFlag ? rn + offset : rn - offset;
        if (writeback)
        {
            if (isLoad && (values.rdIndex == values.rnIndex)) // UNPREDICTABLE
            {
                assert(false && "UNPREDICTABLE");
                return 0xFFFFFFFF; // For compiler
            } 
            // Pre-indexed with write-back: Rn = EA
            cpu.WriteRegister(values.rnIndex, resultAddress);
        }
    }
    else // P = 0
    {
        if (writeback) // UNPREDICTABLE
        {
            assert(false && "UNPREDICTABLE");
            return 0xFFFFFFFF; // For compiler
        }

        resultAddress = rn;
        // Post-index update happens AFTER transfer in HDT function, not here
    }
    return resultAddress;
}