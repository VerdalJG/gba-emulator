#include "Core/GBA_CPU.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_HLE.hpp"

#include "Utils/BitOperations.hpp"

void GBA_CPU::Thumb_ConditionalBranch(u16 instruction)
{
    u16 condition = ExtractBits<11, 8>(instruction);

    if (condition == 0xE) // Undefined in THUMB, AL in ARM
    {
        return; // TODO: For now, should go into undefined mode I think.
    }

    if (ConditionPassed(static_cast<Condition>(condition)))
    {
        u8 offset_8 = ExtractBits<7, 0>(instruction);
        s32 offset_32 = static_cast<s32>(SignExtend_8(offset_8) * 2); // Offset is in steps of two

        cpuState.r15 += offset_32;
        FlushPipeline();
    }
    else
    {
        pipeline.access = Access::Code | Access::Sequential;
        AdvanceProgramCounter();
    }
}

void GBA_CPU::Thumb_SoftwareInterrupt(u16 instruction)
{
    // HLE BIOS path
    if (GetCore()->UsingHLE())
    {
        u32 swiNumber = ExtractBits<8, 0>(instruction);
        GetCore()->GetHLE().HandleSWI(swiNumber);

        // TODO: Add cycles - 2S + 1N
        return;
    }

    // Save CPSR
    cpuState.spsr[BANK_SVC].value = cpuState.cpsr.value;

    // Switch to Supervisor Mode, switch to ARM mode and disable IRQs
    SwitchMode(Mode::SVC);
    cpuState.cpsr.fields.thumb = 0;
    cpuState.cpsr.fields.irq_disable = 1;

    // Save one instruction ahead of the fault address and jump to SVC exception vector
    u32 returnAddress = cpuState.r15 - 2; // Only minus 2 because we want the instruction after the SWI
    cpuState.r14 = returnAddress;
    cpuState.r15 = 0x08;
    FlushPipeline();
}

void GBA_CPU::Thumb_UnconditionalBranch(u16 instruction)
{
    u16 offset_11 = ExtractBits<10, 0>(instruction);
    u32 offset_32 = static_cast<u32>(offset_11);
    s32 finalOffset;

    if (offset_11 & 0x400)
    {
        offset_32 |= 0xFFFFF800;
    }

    finalOffset = static_cast<s32>(offset_32) * 2; // In steps of 2

    cpuState.r15 += finalOffset;
    FlushPipeline();
}

void GBA_CPU::Thumb_LongBranchWithLink(u16 instruction)
{
    const bool secondInstruction = IsBitSet<11>(instruction);
    u32 offset_11 = static_cast<u32>(ExtractBits<10, 0>(instruction));

    if (secondInstruction)
    {
        // Get the PC of the currently executing instruction
        u32 currentPC = cpuState.r15 - 4;

        // Save the next instruction address (+2 in thumb)
        cpuState.r14 = (currentPC + 2) | 1;

        // Modify R15 and flush pipeline
        cpuState.r15 = cpuState.r14 + (offset_11 << 1);
        FlushPipeline();
    }
    else
    {
        // Sign extend
        if (offset_11 & 0x400)
        {
            offset_11 |= 0xFFFFF800;
        }

        cpuState.r14 = cpuState.r15 + (offset_11 << 12);

        pipeline.access = Access::Code | Sequential;
        AdvanceProgramCounter();
    }
}