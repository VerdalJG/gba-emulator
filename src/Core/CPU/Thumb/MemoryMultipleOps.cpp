#include "Core/GBA_CPU.hpp"
#include "Utils/BitOperations.hpp"

#include <assert.h>

void GBA_CPU::Thumb_PushPopRegisters(u16 instruction)
{
    const bool pop = IsBitSet<11>(instruction);
    const bool bit_pc_lr = IsBitSet<8>(instruction);
    u16 registerList = ExtractBits<7, 0>(instruction);

    // TODO: Affect pipeline
    AdvanceProgramCounter();

    // Handle special case for empty register lists.
    if (registerList == 0 && !bit_pc_lr)
    {
        if (pop)
        {
            cpuState.r15 = Read32(cpuState.r13, Access::Data | Access::Nonsequential);
            FlushPipeline();
            cpuState.r13 += 0x40;
        }
        else
        {
            cpuState.r13 -= 0x40;
            Write32(cpuState.r13, cpuState.r15, Access::Data | Access::Nonsequential);
            pipeline.access = Access::Code | Access::Nonsequential;
        }
        return;
    }

    if (pop)
    {
        u32 startAddress = cpuState.r13;
        u32 endAddress = startAddress + 4 * (bit_pc_lr + NumberOfSetBitsIn(registerList));
        u32 address = startAddress;

        uint accessType = Access::Data | Access::Nonsequential;

        for (int i = 0; i < 8; i++) // R0 - R7
        {
            if (registerList & (1 << i))
            {
                cpuState.registers[i] = Read32(address, accessType);
                accessType = Access::Data | Access::Sequential;
                address += 4;
            }
        }

        if (bit_pc_lr)
        {
            cpuState.r15 = Read32(address, Access::Data | Access::Nonsequential) & ~1; // Halfword alignment
            address += 4;
            FlushPipeline();
        }

        assert(endAddress == address);
        cpuState.r13 = endAddress;
        // TODO: Add one I cycle
        pipeline.access = Access::Code | Access::Sequential;
    }
    else
    {
        u32 startAddress = cpuState.r13 - 4 * (bit_pc_lr + NumberOfSetBitsIn(registerList));
        u32 endAddress = cpuState.r13 - 4;
        u32 address = startAddress;

        uint accessType = Access::Data | Access::Nonsequential;

        for (int i = 0; i < 8; i++) // R0 - R7
        {
            if (registerList & (1 << i))
            {
                Write32(address, cpuState.registers[i], accessType);
                accessType = Access::Data | Access::Sequential;
                address += 4;
            }
        }

        if (bit_pc_lr)
        {
            Write32(address, cpuState.r14, accessType);
            address += 4;
        }

        assert(endAddress == address - 4);
        cpuState.r13 = startAddress;

        pipeline.access = Access::Code | Access::Nonsequential;
    }
}

void GBA_CPU::Thumb_LoadStoreMultiple(u16 instruction)
{
    const bool load = IsBitSet<11>(instruction);
    const u16 rbIndex = ExtractBits<10, 8>(instruction);

    u16 registerList = ExtractBits<7, 0>(instruction);
    u32 rb = ReadRegister(rbIndex);
    u32 count = NumberOfSetBitsIn(registerList);

    

    if (count == 0) // Empty register list
    {
        if (load)
        {
            cpuState.r15 = Read32(rb, Access::Data | Access::Nonsequential);
            FlushPipeline();
        }
        else
        {
            Write32(rb, cpuState.r15, Access::Data | Access::Nonsequential);
            pipeline.access = Access::Code | Access::Nonsequential;
        }
        cpuState.registers[rbIndex] += 0x40;
        return;
    }

    if (load)
    {
        u32 startAddress = rb;
        u32 endAddress = rb + (count * 4);
        u32 address = startAddress;

        uint accessType = Access::Data | Access::Nonsequential;

        for (int i = 0; i < 8; i++)
        {
            if (registerList & (1 << i))
            {
                WriteRegister(i, Read32(address, accessType));
                accessType = Access::Data | Access::Sequential;
                address += 4;
            }
        }

        assert(endAddress == address);

        // [ARMv4T specific] Store the address in the register only 
        // if the base register was not part of the list
        if (!(registerList & (1 << rbIndex)))
        {
            WriteRegister(rbIndex, address);
        }
        
        pipeline.access = Access::Code | Access::Sequential;
        // TODO: Add one I cycle
    }
    else
    {
        u32 first = 0;

        u32 startAddress = rb;
        u32 endAddress = rb + (count * 4);
        u32 address = startAddress;

        // Get the index of the first register in the list
        for (int i = 0; i < 8; i++)
        {
            if (registerList & (1 << i))
            {
                first = i;
                break;
            }
        }

        // Calculate new base register value (not updated yet)
        u32 rb_new = endAddress;

        // Store first register, then update the base register's value (writeback)
        Write32(address, ReadRegister(first), Access::Data | Access::Nonsequential);
        WriteRegister(rbIndex, rb_new);
        address += 4;

        // Store the rest of the registers
        for (int i = first + 1; i < 8; i++)
        {
            if (registerList & (1 << i))
            {
                Write32(address, ReadRegister(i), Access::Data | Access::Sequential);
                address += 4;
            }
        }

        assert(endAddress == address);
        pipeline.access = Access::Code | Access::Nonsequential;
    }

    AdvanceProgramCounter();
}