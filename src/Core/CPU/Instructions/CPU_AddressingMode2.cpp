#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/Instructions/CPU_Shifts.hpp"

uint32_t GBA_CPU::CalculateAddressMode2(SingleDataTransfer_Decoded decodedValues)
{
    bool writeback = decodedValues.wFlag;
    bool isRegister = decodedValues.iFlag;
    bool isPreIndexed = decodedValues.pFlag;
    bool usingPC = decodedValues.rnIndex == 15;
    bool addOffset = decodedValues.uFlag;

    uint32_t address;
    uint32_t& rn = registers[decodedValues.rnIndex];

    if (isRegister)
    {   
        // Differentiate between shifted register and normal register offset
        bool isScaledRegister = (decodedValues.offsetBits >> 4); 

        uint32_t rmIndex =  decodedValues.offsetBits & 0xF;
        uint32_t& rm = registers[rmIndex & 0xF];

        bool rnUsingPC = decodedValues.rnIndex == 15;
        bool rmUsingPC = rmIndex == 15;

        if (isScaledRegister)
        {
            ShiftType shiftType = static_cast<ShiftType>((decodedValues.offsetBits >> 5) & 3);
            uint32_t shiftImm = (decodedValues.offsetBits >> 7) & 0x1F;
            uint32_t offset = CalculateScaledRegister(rm, shiftType, shiftImm);
            address = addOffset ? rn + offset : rn - offset;
            if (isPreIndexed && writeback) // Scaled register pre-indexed
            {
                if (rnUsingPC || rmUsingPC) return; // UNPREDICTABLE
                if (decodedValues.rnIndex == rmIndex) return; // UNPREDICTABLE
                // IF CONDITION PASSED (technically if it is here, it already 100% did)
                rn = address;
                return address;
            }
            else if (isPreIndexed) // Scaled register offset
            {
                if (rmUsingPC) return; // UNPREDICTABLE;
                return rnUsingPC ? address + 8 : address;
            }
            else // Scaled register post-indexed
            {
                if (rnUsingPC || rmUsingPC) return; // UNPREDICTABLE
                if (decodedValues.rnIndex == rmIndex) return; // UNPREDICTABLE
                return address;
            }
        }
        else
        {
            address = addOffset ? (rn + rm) : (rn - rm);
            if (isPreIndexed && writeback) // Register offset pre-indexed
            {
                if (rnUsingPC || rmUsingPC) return; // UNPREDICTABLE
                if (decodedValues.rnIndex == rmIndex) return; // UNPREDICTABLE
                // IF CONDITION PASSED (technically if it is here, it already 100% did)
                rn = address;
                return address;
            }
            else if (isPreIndexed) // Register offset
            {
                if (rmUsingPC) return; // UNPREDICTABLE
                return rnUsingPC ? address + 8 : address; 
            }
            else // Register offset post-indexed
            {
                if (rnUsingPC || rmUsingPC) return; // UNPREDICTABLE
                if (decodedValues.rnIndex == rmIndex) return; // UNPREDICTABLE
                return address; // Update happens after operation
            }
        }
    }
    else // Immediate
    {
        // Calculate address
        uint32_t offset12 = decodedValues.offsetBits;
        address = addOffset ? rn + offset12 : rn - offset12;

        // Handle special cases
        if (isPreIndexed && writeback) // Pre-indexed immediate offset
        {
            if (usingPC) return; // UNPREDICTABLE
            // IF CONDITION PASSED (technically if it is here, it already 100% did)
            rn = address;
            return address;
        }
        else if (isPreIndexed) // Immediate offset
        {
            return usingPC ? address + 8 : address; // ARM offset due to using PC
        }
        else // Post-indexed immediate offset
        {
            if (usingPC) return; // UNPREDICTABLE
            return address; // Update happens after operation
        }
    }
}