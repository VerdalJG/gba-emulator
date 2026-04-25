#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Shifts.hpp"

#include "Utils/BitOperations.hpp"

void GBA_CPU::ARM_SingleDataSwap(u32 instruction)
{
    bool byte = IsBitSet<22>(instruction);
    u32 rnIndex = ExtractBits<19, 16>(instruction);
    u32 rdIndex = ExtractBits<15, 12>(instruction);
    u32 rmIndex = ExtractBits<3, 0>(instruction);

    // Check for UNPREDICTABLE conditions
    bool usingPC = rnIndex == 15 || rmIndex == 15 || rdIndex == 15;
    if (usingPC || rdIndex == rmIndex || rdIndex == rnIndex)
    {
        // UNPREDICTABLE
    }

    u32 rn = ReadRegister(rnIndex);
    u32 rm = ReadRegister(rmIndex);

    u32 readValue;

    if (byte) // SWPB
    {   
        readValue = Read8(rn, Access::Data | Access::Nonsequential);
        Write8(rn, rm & 0xFF, Access::Data | Access::Nonsequential); // TODO: Lock bus?
    }
    else // SWP
    {
        readValue = Read32_Rotated(rn, Access::Data | Access::Nonsequential);
        Write32(rn, rm, Access::Data | Access::Nonsequential); // TODO: Lock bus?
    }

    pipeline.access = Access::Code | Access::Nonsequential;
    AdvanceProgramCounter();

    AddInternalCycles(1);

    cpuState.registers[rdIndex] = readValue;

    if (rdIndex == 15)
    {
        FlushPipeline();
    }
}

void GBA_CPU::ARM_SingleDataTransfer(u32 instruction)
{
    const bool immediate = !IsBitSet<25>(instruction); // (0=Immediate, 1=Shifted Register)
    const bool pre_indexed = IsBitSet<24>(instruction);
    const bool add = IsBitSet<23>(instruction);
    const bool byte = IsBitSet<22>(instruction);
    const bool writeback = IsBitSet<21>(instruction);
    const bool load = IsBitSet<20>(instruction);

    const u32 rnIndex = ExtractBits<19, 16>(instruction); // Base register (base address)
    const u32 rdIndex = ExtractBits<15, 12>(instruction); // Source/Destination register
    const u32 rmIndex = ExtractBits<3, 0>(instruction); // Offset register

    u32 baseAddress = ReadRegister(rnIndex);
    u32 effectiveAddress = baseAddress;
    u32 writebackValue;

    u32 offset;

    // Address calculation
    if (immediate)
    {
        offset = ExtractBits<11, 0>(instruction);
    }
    else // Register shifted by immediate as offset
    {
        uint carry = GetCPSR_C();
        u32 shiftAmount = ExtractBits<11, 7>(instruction); // Shift amount is an immediate value
        uint shiftOp = ExtractBits<6, 5>(instruction);
        
        offset = ReadRegister(rmIndex);
        ApplyShift(shiftOp, offset, shiftAmount, carry, true); 
    }

    if (!add)
    {
        offset = -offset;
    }

    bool rnUsingPC = rnIndex == 15;
    bool rmUsingPC = rmIndex == 15;

    if (pre_indexed)
    {
        effectiveAddress = baseAddress + offset;

        if (rmUsingPC); // UNPREDICTABLE
        if (writeback && (rnUsingPC || (!immediate && rnIndex == rmIndex))); // UNPREDICTABLE
    }
    else
    {
        effectiveAddress = baseAddress;
        
        if (rnUsingPC || rmUsingPC || (!immediate && rnIndex == rmIndex)); // UNPREDICTABLE
    }

    writebackValue = baseAddress + offset;
    AdvanceProgramCounter();

    if (load)
    {
        u32 readValue;

        if (byte) // LDRB
        {
            readValue = Read8(effectiveAddress, Access::Data | Access::Nonsequential);
        }
        else // LDR
        {
            readValue = Read32_Rotated(effectiveAddress, Access::Data | Access::Nonsequential);
        }

        // Post-Indexed update
        if ((writeback || !pre_indexed))
        {
            cpuState.registers[rnIndex] = writebackValue;
            if (rnIndex == 15) FlushPipeline();
        }

        // Align if rdIndex == PC
        if (rdIndex == 15) readValue &= (IsThumbMode() ? ~1u : ~3u);
        cpuState.registers[rdIndex] = readValue;

        AddInternalCycles(1);
        pipeline.access = Access::Code | Access::Sequential;
    }
    else // Store
    {
        u32 rd = ReadRegister(rdIndex);

        if (byte) // STRB
        {
            uint8_t byte = rd & 0xFF;
            Write8(effectiveAddress, byte, Access::Data | Access::Nonsequential);
        }
        else // STR
        {
            // (ARM7TDMI / GBA): misaligned address is forcibly aligned
            uint32_t alignedAddress = effectiveAddress & ~3;
            Write32(alignedAddress, rd, Access::Data | Access::Nonsequential);
        }

        // Post-Indexed update
        if ((writeback || !pre_indexed))
        {
            cpuState.registers[rnIndex] = writebackValue;
            if (rnIndex == 15) FlushPipeline();
        }

        pipeline.access = Access::Code | Access::Nonsequential;
    }

    // Program counter increment/flush
    if (load && rdIndex == 15)
    {
        FlushPipeline();
    }
}

void GBA_CPU::ARM_HalfwordDataTransfer(u32 instruction)
{
    const bool pre_indexed = IsBitSet<24>(instruction);
    const bool add = IsBitSet<23>(instruction);
    const bool immediate = IsBitSet<22>(instruction);
    const bool writeback = IsBitSet<21>(instruction);
    const bool load = IsBitSet<20>(instruction);

    const u32 rnIndex = ExtractBits<19, 16>(instruction); // Base register (base address)
    const u32 rdIndex = ExtractBits<15, 12>(instruction); // Source/Destination register

    const bool signed_flag = IsBitSet<6>(instruction);
    const bool halfword = IsBitSet<5>(instruction);

    u32 baseAddress = ReadRegister(rnIndex);
    u32 effectiveAddress;
    u32 writebackValue;

    u32 offset;

    if (immediate)
    {
        offset = (ExtractBits<11, 8>(instruction) << 4) | ExtractBits<3, 0>(instruction);
    }
    else
    {
        u32 rmIndex = ExtractBits<3, 0>(instruction);
        offset = ReadRegister(rmIndex);
    }

    if (!add) 
    {
        offset = -offset;
    }

    if (pre_indexed)
    {
        effectiveAddress = baseAddress + offset;
        writebackValue = effectiveAddress;

        if (writeback && load && rdIndex == rnIndex)
        {
            // UNPREDICTABLE
            Log("UNPREDICTABLE - Pre-Indexed load, writeback rd == rn", LogType::Warning, __func__);
        } 
    }
    else
    {
        effectiveAddress = baseAddress;
        writebackValue = baseAddress + offset;

        if (writeback) // UNPREDICTABLE
        {
            Log("UNPREDICTABLE - Post-Indexed with Writeback!", LogType::Warning, __func__);
        }
    }

    AdvanceProgramCounter();

    if (load)
    {
        u32 readValue;

        if (signed_flag && halfword) // LDRSH
        {
            readValue = Read16_Signed(effectiveAddress, Access::Data | Access::Nonsequential);
        }
        else if (halfword) // LDRH
        {
            readValue = Read16_Rotated(effectiveAddress, Access::Data | Access::Nonsequential);
        }
        else if (signed_flag) // LDRSB
        {
            readValue = Read8_Signed(effectiveAddress, Access::Data | Access::Nonsequential);
        }

        if (writeback || !pre_indexed)
        {
            cpuState.registers[rnIndex] = writebackValue;
        }

        cpuState.registers[rdIndex] = readValue;

        AddInternalCycles(1);
        pipeline.access = Access::Code | Access::Sequential;
    }
    else // STRH
    {
        if (signed_flag); // UNPREDICTABLE

        uint32_t rd = ReadRegister(rdIndex);

        // GBA SPECIFIC: ARM7TDMI (GBA) misaligned STRH does NOT fault.
        // The low address bit is ignored and the store is forced to an aligned address.
        uint16_t storeValue = static_cast<uint16_t>(rd);

        // ARM7TDMI (GBA): misaligned STRH is forcibly aligned
        uint32_t alignedAddress = effectiveAddress & ~1;
        Write16(alignedAddress, storeValue, Access::Data | Access::Nonsequential);

        if (writeback || !pre_indexed)
        {
            cpuState.registers[rnIndex] = writebackValue;
        }

        pipeline.access = Access::Code | Access::Nonsequential;
    }



    // Program counter increment/flush
    if (load && rdIndex == 15)
    {
        FlushPipeline();
    }
}

void GBA_CPU::ARM_BlockDataTransfer(u32 instruction)
{
    bool pre_indexed = IsBitSet<24>(instruction);
    const bool incrementing = IsBitSet<23>(instruction);
    const bool forceUserMode = IsBitSet<22>(instruction);
    const bool writeback = IsBitSet<21>(instruction);
    const bool load = IsBitSet<20>(instruction);

    const u32 rnIndex = ExtractBits<19, 16>(instruction);
    u32 registerList = ExtractBits<15, 0>(instruction);

    bool pcInRegisterList = IsBitSet<15>(registerList);

    const Mode currentMode = cpuState.cpsr.fields.mode;
    uint bytes = 0;
    uint first = 0;

    if (registerList == 0) // Empty register list
    {
        registerList = 1 << 15; // Only the PC is loaded/stored
        first = 15;
        pcInRegisterList = true;

        // The base register increments/decrements as if all registers were stored
        // numRegisters * sizeof(u32) (16 * 4 = 64)
        bytes = 64; 
    }
    else // Find first register in list and amount of bytes to transfer
    {
        for (int i = 15; i >= 0; i--)
        {
            if (registerList & (1 << i))
            {
                first = i;
                bytes += 4;
            }
        }
    }

    u32 rn = ReadRegister(rnIndex); // Base register

    /** 
     * In LDM^ (bit 22 set), we would need to switch to user mode if we are not in user/system mode 
     * already. This attempts to solve the LDM user mode register bus conflict, where for 2 internal
     * cycles, both register banks can be accessed and attempt to drive the same bus.
     * 
     * This does not happen for stores, in STM^ the transfer just occurs with the user mode registers,
     * but no mode-switch occurs
     * 
     * Switch if:
     * STM (load == false) → true
     * LDM without PC → true
     * LDM with PC → false
     */

    const bool shouldSwitchMode = forceUserMode && (!load || !pcInRegisterList) && CurrentModeHasSPSR();

    if (shouldSwitchMode)
    {
        SwitchMode(Mode::USR);
    }

    u32 address = rn;
    u32 newBaseAddress = address;

    /**
     * For DecrementBefore (DB) and DecrementAfter (DA), final address is determined first, 
     * then transfers occur using an incrementing address.
     * 
     * Because of this, pre-indexed bool is flipped.
     */

    if (incrementing)
    {
        newBaseAddress += bytes; 
    }
    else // Decrementing
    {
        pre_indexed = !pre_indexed;
        address -= bytes;
        newBaseAddress -= bytes;
    }

    pipeline.access = Access::Code | Access::Nonsequential;
    
    int accessType = Access::Data | Access::Nonsequential;

    AdvanceProgramCounter();

    for (int i = first; i <= 15; i++)
    {
        if (~registerList & (1 << i)) continue;

        if (pre_indexed)
        {
            address += 4;
        }

        if (load)
        {
           u32 readValue = Read32(address, accessType);
            if (writeback && i == first)
            {
                WriteRegister(rnIndex, newBaseAddress);
            }
            WriteRegister(i, readValue);
        }
        else // Store
        {
            Write32(address, ReadRegister(i), accessType);
            if (writeback && i == first)
            {
                WriteRegister(rnIndex, newBaseAddress);
            }
        }

        if (!pre_indexed)
        {
            address += 4;
        }

        accessType = Access::Sequential;
    }

    if (load)
    {
        AddInternalCycles(1);

        if (shouldSwitchMode)
        {
            // TODO: Emulate LDM usermode conflict
        }

        if (pcInRegisterList)
        {
            if (forceUserMode)
            {
                StatusRegister spsr = GetCurrentSPSR();
                SwitchMode(spsr.fields.mode);
                cpuState.cpsr.value = spsr.value;
            }

            FlushPipeline();
        }
    }

    // Switch back to current mode
    if (forceUserMode)
    {
        SwitchMode(currentMode);
    }
} 