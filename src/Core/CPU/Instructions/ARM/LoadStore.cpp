#include "Core/CPU/Instructions/LoadStore.hpp"
#include "Core/CPU/Instructions/AddressingMode2.hpp"
#include "Core/CPU/Instructions/AddressingMode3.hpp"
#include "Core/CPU/Instructions/AddressingMode4.hpp"
#include "Core/CPU/Instructions/Shifts.hpp"
#include "Core/CPU/Instructions/InstructionHelpers.hpp"

#include "Core/CPU/CPU_Timings.hpp"
#include "Core/GBA_CPU.hpp"

#include <assert.h>
#include <bit>


// Loads: LDRH LDRSB LDRSH
// Stores: STRH
void HalfwordDataTransfer(uint32_t instruction, GBA_CPU& cpu)
{
    HalfwordDataTransfer_Decoded values = HalfwordDataTransfer_Decode(instruction);

    if (!values.sFlag && !values.hFlag) return; // This is another instruction - Mult or Single Data Swap

    uint32_t effectiveAddress = CalculateAddress_AddressingMode3(values, cpu);

    bool isLoad = values.lFlag;
    bool isHalfword = values.hFlag;
    bool isSigned = values.sFlag;
    
    // GBA SPECIFIC LOADING: IF INSTRUCTION IS MISALIGNED FOR HALFWORDS,
    // IT ROTATES THE LOADED HALFWORD 8 BITS TO THE RIGHT

    if (isLoad)
    {
        if (!isHalfword)
        {
            // LDRSB
            uint8_t loadedByte = cpu.Read8_Bus(effectiveAddress);
            uint32_t valueToLoad = SignExtendTo32(loadedByte);
            cpu.SetValueAtRegister(values.rdIndex, valueToLoad);
        }
        else
        {
            // Account for misaligned halfword
            bool misaligned = effectiveAddress & 1; // First bit must be 0 to be divisible by 2 (halfword)
            uint32_t alignedAddress = effectiveAddress & ~1;
            uint16_t loadedHalfword = cpu.Read16_Bus(alignedAddress);
            if (misaligned)
            {
                loadedHalfword = RotateRight(loadedHalfword, 8);
            }
            
            // LDRSH vs LDRH
            uint32_t valueToLoad = isSigned ? SignExtendTo32(loadedHalfword) : ZeroExtendTo32(loadedHalfword);
            cpu.SetValueAtRegister(values.rdIndex, valueToLoad);
        }

        cpu.AddCycles(CPU_Timings::LOAD_BASE_COST);
    }
    else // STRH
    {
        if (isSigned) return; // UNPREDICTABLE

        uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);
        // GBA SPECIFIC: ARM7TDMI (GBA) misaligned STRH does NOT fault.
        // The low address bit is ignored and the store is forced to an aligned address.
        uint16_t valueToStore = static_cast<uint16_t>(rd);

        // ARM7TDMI (GBA): misaligned STRH is forcibly aligned
        uint32_t alignedAddress = effectiveAddress & ~1;
        cpu.Write16_Bus(alignedAddress, valueToStore);
        cpu.InvalidateSequentiality();
    }

    // Only post-index case updates here
    if (!values.pFlag)
    {
        uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
        uint32_t offset = values.iFlag ? GetHDTOffset_Immediate(values.offsetBits) : GetHDTOffset_Register(values.offsetBits, cpu);
        uint32_t postIndexedAddress = values.uFlag ? (rn + offset) : (rn - offset);
        cpu.SetValueAtRegister(values.rnIndex, postIndexedAddress);
    } 
}

void SingleDataTransfer(uint32_t instruction, GBA_CPU& cpu)
{
    SingleDataTransfer_Decoded values = SingleDataTransfer_Decode(instruction);

    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);

    bool isImmediate = !values.iFlag; 
    bool isLoad = values.lFlag;
    bool isByte = values.bFlag;
    bool preIndexed = values.pFlag;
    bool shouldWriteback = values.wFlag;
    
    uint32_t effectiveAddress = CalculateAddress_AddressingMode2(values, cpu);
    
    if (isLoad)
    {
        uint32_t valueToLoad;
        if (isByte)
        {
            // LDRB
            uint8_t loadedByte = cpu.Read8_Bus(effectiveAddress);
            valueToLoad = ZeroExtendTo32(loadedByte);
        }
        else
        {
            // LDR
            // First and second bit must be 0 to be divisible by 4 (word)
            uint32_t align = effectiveAddress & 3;
            uint32_t alignedAddress = effectiveAddress & ~3; 

            valueToLoad = cpu.Read32_Bus(alignedAddress);

            if (align)
            {
                // Rotate by how many bytes it is misaligned by
                valueToLoad = RotateRight(valueToLoad, 8 * (align)); 
            }
        }
        
        // Special case if rdIndex == PC
        if (values.rdIndex == GBA_CPU::PC_INDEX)
        {
            valueToLoad &= cpu.IsThumbMode() ? ~1u : ~3u; // Align if PC 
        }

        cpu.SetValueAtRegister(values.rdIndex, valueToLoad);
        cpu.AddCycles(CPU_Timings::LOAD_BASE_COST);
    }
    else
    {
        uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);
        if (values.rdIndex == GBA_CPU::PC_INDEX)
        {
            rd += 4; // +8 is already factored in when doing GetValueAtRegister
        }

        if (isByte)
        {
            // STRB
            uint8_t byte = rd & 0xFF;
            cpu.Write8_Bus(effectiveAddress, byte);
        }
        else
        {
            // STR 
            // (ARM7TDMI / GBA): misaligned address is forcibly aligned
            uint32_t alignedAddress = effectiveAddress & ~3;
            cpu.Write32_Bus(alignedAddress, rd);
        }
        cpu.InvalidateSequentiality();
    }

    // Post-indexed update
    if (!values.pFlag)
    {
        uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
        uint32_t offset = CalculateOffset_AddressingMode2(values.offsetBits, values.iFlag, cpu);
        uint32_t postIndexedAddress = values.uFlag ? (rn + offset) : (rn - offset);
        cpu.SetValueAtRegister(values.rnIndex, postIndexedAddress);
    }
}

void BlockDataTransfer(uint32_t instruction, GBA_CPU& cpu)
{
    BlockDataTransfer_Decoded values = BlockDataTransfer_Decode(instruction);

    bool isLoad = values.lFlag;

    if (values.registerList == 0)
    {
        return isLoad ? LDMEmptyRegisterList(values, cpu) : STMEmptyRegisterList(values, cpu);
    }
    
    if (isLoad)
    {
        if (values.sFlag)
        {
            bool pcInRegisterList = values.registerList & 0x8000;
            if (pcInRegisterList)
            {
                if (!cpu.CurrentModeHasSPSR()) return; // UNPREDICTABLE due to not having SPSR
                LDMRestoreCPSR(values, cpu);
            }
            else
            {
                LDMUserRegisters(values, cpu);
            }
        }
        else
        {   
            LDM(values, cpu);
        }
        cpu.AddCycles(CPU_Timings::LOAD_BASE_COST);
    }
    else
    {
        if (values.sFlag)
        {
            STMUserRegisters(values, cpu);
        }
        else 
        {
            STM(values, cpu);
        }
        cpu.InvalidateSequentiality();
    }
}

void LDM(BlockDataTransfer_Decoded values, GBA_CPU &cpu)
{
    bool baseRegisterInList = (values.registerList >> values.rnIndex) & 1;
    AddressingMode4 addressing4 = CalculateAddressingMode4(values, cpu);
    uint32_t address = addressing4.startAddress & ~3u; // Word align address
    
    for (int i = 0; i < 16; ++i)
    {
        if (!((values.registerList >> i) & 1)) continue;
        if (i == values.rnIndex && values.wFlag) continue;

        uint32_t loaded = cpu.Read32_Bus(address);

        if (i == GBA_CPU::PC_INDEX)
        {
            cpu.SetValueAtRegister(GBA_CPU::PC_INDEX, loaded & 0xFFFFFFFC); // Ignore bits [1:0]
        }
        else
        {
            cpu.SetValueAtRegister(i, loaded);
        }

        address += 4;
    }

    assert(address - 4 == addressing4.endAddress && "INCORRECT ADDRESSING4 CALCULATION");

    if (baseRegisterInList && values.wFlag)
    {
        // Normally UNPREDICTABLE, but ARM7TDMI has this special case
        cpu.SetValueAtRegister(values.rnIndex, addressing4.writebackValue);
    }
}

void LDMUserRegisters(BlockDataTransfer_Decoded values, GBA_CPU &cpu)
{
    bool baseRegisterInList = (values.registerList >> values.rnIndex) & 1;
    AddressingMode4 addressing4 = CalculateAddressingMode4(values, cpu);
    uint32_t address = addressing4.startAddress & ~3u;
    
    for (int i = 0; i < 8; ++i)
    {
        if (!((values.registerList >> i) & 1)) continue;
        if (i == values.rnIndex && values.wFlag) continue;

        uint32_t loaded = cpu.Read32_Bus(address);

        cpu.SetValueAtRegister(i, loaded);

        address += 4;
    }

    for (int i = 8; i < 13; ++i)
    {
        if (!((values.registerList >> i) & 1)) continue;
        if (i == values.rnIndex) continue;

        uint32_t loaded = cpu.Read32_Bus(address);

        if (cpu.GetCurrentOperatingMode() == OperatingMode::FIQ)
        {
            cpu.SetValueAtUserRegister(i, loaded);
        }
        else
        {
            cpu.SetValueAtRegister(i, loaded);
        }

        address += 4;
    }

    assert(address - 4 == addressing4.endAddress && "INCORRECT ADDRESSING4 CALCULATION");

    if (baseRegisterInList && values.wFlag)
    {
        // Normally UNPREDICTABLE, but ARM7TDMI has this special case
        cpu.SetValueAtRegister(values.rnIndex, addressing4.writebackValue);
    }
}

void LDMRestoreCPSR(BlockDataTransfer_Decoded values, GBA_CPU &cpu)
{
    bool baseRegisterInList = (values.registerList >> values.rnIndex) & 1;
    AddressingMode4 addressing4 = CalculateAddressingMode4(values, cpu);
    uint32_t address = addressing4.startAddress & ~3u;

    for (int i = 0; i < 15; ++i)
    {
        if (!((values.registerList >> i) & 1)) continue;
        if (i == values.rnIndex && values.wFlag) continue;

        uint32_t loaded = cpu.Read32_Bus(address);
        cpu.SetValueAtRegister(i, loaded);

        address += 4;
    }

    int bank = BankIndex(cpu.GetCurrentOperatingMode());
    cpu.RestoreCPSRFromSPSR(bank);

    uint32_t loaded = cpu.Read32_Bus(address);
    uint32_t pcValue = cpu.IsThumbMode() ? loaded & 0xFFFFFFFE : loaded & 0xFFFFFFFC;

    cpu.SetValueAtRegister(GBA_CPU::PC_INDEX, pcValue); // Ignore bits [1:0]

    assert(address - 4 == addressing4.endAddress && "INCORRECT ADDRESSING4 CALCULATION");

    if (baseRegisterInList && values.wFlag)
    {
        // Normally UNPREDICTABLE, but ARM7TDMI has this special case
        cpu.SetValueAtRegister(values.rnIndex, addressing4.writebackValue);
    }
}

void LDMEmptyRegisterList(BlockDataTransfer_Decoded values, GBA_CPU &cpu)
{
    AddressingMode4 addressing4 = CalculateAddressingMode4(values, cpu);
    uint32_t address = addressing4.startAddress & ~3u;

    uint32_t loaded = cpu.Read32_Bus(address);
    uint32_t pcValue = cpu.IsThumbMode() ? (loaded & ~1u) : (loaded & ~3u);

    cpu.SetValueAtRegister(GBA_CPU::PC_INDEX, pcValue);

    assert(address - 4 == addressing4.endAddress && "INCORRECT ADDRESSING4 CALCULATION");
}

void STM(BlockDataTransfer_Decoded values, GBA_CPU &cpu)
{
    // TODO: check countr_zero
    bool lowestSetBitIsRn = std::countr_zero(values.registerList) == values.rnIndex;
    AddressingMode4 addressing4 = CalculateAddressingMode4(values, cpu); 
    uint32_t address = addressing4.startAddress & ~3u;

    for (int i = 0; i < 15; ++i)
    {
        if (!((values.registerList >> i) & 1)) continue;

        uint32_t toStore = cpu.GetValueAtRegister(i);

        bool sequential = i != 0;
        cpu.Write32_Bus(address, toStore);

        address += 4;
    }

    if (values.registerList & (1u << GBA_CPU::PC_INDEX))
    {
        uint32_t pc = cpu.GetValueAtRegister(GBA_CPU::PC_INDEX) + 4;
        cpu.Write32_Bus(address, pc);
        address += 4;
    }

    assert(address - 4 == addressing4.endAddress && "INCORRECT ADDRESSING4 CALCULATION");

    if (!lowestSetBitIsRn && values.wFlag)
    {
        // Normally UNPREDICTABLE, but ARM7TDMI has this special case
        cpu.SetValueAtRegister(values.rnIndex, addressing4.writebackValue);
    }
}

void STMUserRegisters(BlockDataTransfer_Decoded values, GBA_CPU &cpu)
{
    AddressingMode4 addressing4 = CalculateAddressingMode4(values, cpu);
    uint32_t address = addressing4.startAddress & ~3u;
    
    for (int i = 0; i < 8; ++i)
    {
        if (!((values.registerList >> i) & 1)) continue;

        uint32_t toStore = cpu.GetValueAtRegister(i);

        bool sequential = i != 0;
        cpu.Write32_Bus(address, toStore);

        address += 4;
    }

    for (int i = 8; i < 13; ++i)
    {
        if (!((values.registerList >> i) & 1)) continue;

        uint32_t toStore;
        
        if (cpu.GetCurrentOperatingMode() == OperatingMode::FIQ)
        {
            toStore = cpu.GetValueAtUserRegister(i);
        }
        else
        {
            toStore = cpu.GetValueAtRegister(i);
        }

        bool sequential = i != 0;
        cpu.Write32_Bus(address, toStore);

        address += 4;
    }

    if (values.registerList & (1u << GBA_CPU::PC_INDEX))
    {
        uint32_t pc = cpu.GetValueAtRegister(GBA_CPU::PC_INDEX) + 4;
        cpu.Write32_Bus(address, pc);
        address += 4;
    }

    assert(address - 4 == addressing4.endAddress && "INCORRECT ADDRESSING4 CALCULATION");

    if (values.wFlag)
    {
        // Normally UNPREDICTABLE, but ARM7TDMI has this special case
        cpu.SetValueAtRegister(values.rnIndex, addressing4.writebackValue);
    }

}

void STMEmptyRegisterList(BlockDataTransfer_Decoded values, GBA_CPU &cpu)
{
    AddressingMode4 addressing4 = CalculateAddressingMode4(values, cpu);
    uint32_t address = addressing4.startAddress & ~3u;

    // GetValueAtRegister(PC) returns PC+8; STM stores PC+12
    uint32_t toStore = cpu.GetValueAtRegister(GBA_CPU::PC_INDEX) + 4;
    cpu.Write32_Bus(address, toStore);

    assert(address - 4 == addressing4.endAddress && "INCORRECT ADDRESSING4 CALCULATION");
}

HalfwordDataTransfer_Decoded HalfwordDataTransfer_Decode(uint32_t instruction)
{
    HalfwordDataTransfer_Decoded result;

    result.condition = GetConditionType(instruction);
    result.pFlag = (instruction >> 24) & 1;
    result.uFlag = (instruction >> 23) & 1;
    result.wFlag = (instruction >> 21) & 1;
    result.lFlag = (instruction >> 20) & 1;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;

    result.sFlag = (instruction >> 6) & 1;
    result.hFlag = (instruction >> 5) & 1;

    result.offsetBits = instruction & 0xF0F;
    return result;
}

SingleDataTransfer_Decoded SingleDataTransfer_Decode(uint32_t instruction)
{
    SingleDataTransfer_Decoded result;

    result.condition = GetConditionType(instruction);
    result.iFlag = (instruction >> 25) & 1;
    result.pFlag = (instruction >> 24) & 1;
    result.uFlag = (instruction >> 23) & 1;
    result.bFlag = (instruction >> 22) & 1;
    result.wFlag = (instruction >> 21) & 1;
    result.lFlag = (instruction >> 20) & 1;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.rdIndex = (instruction >> 12) & 0xF;
    result.offsetBits = instruction & 0xFFF;
    return result;
}

BlockDataTransfer_Decoded BlockDataTransfer_Decode(uint32_t instruction)
{
    BlockDataTransfer_Decoded result;

    result.condition = GetConditionType(instruction);

    result.pFlag = (instruction >> 24) & 1;
    result.uFlag = (instruction >> 23) & 1;
    result.sFlag = (instruction >> 22) & 1;
    result.wFlag = (instruction >> 21) & 1;
    result.lFlag = (instruction >> 20) & 1;

    result.rnIndex = (instruction >> 16) & 0xF;
    result.registerList = instruction & 0xFFFF;
    return result;
}
