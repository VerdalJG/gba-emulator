#include "Core/GBA_CPU.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_HLE.hpp"

#include "Utils/BitOperations.hpp"

void GBA_CPU::ARM_SoftwareInterrupt(u32 instruction)
{
    // HLE BIOS path
    if (GetCore()->UsingHLE())
    {
        u32 swiNumber = ExtractBits<8, 0>(instruction);
        GetCore()->GetHLE().HandleSWI(swiNumber);

        // TODO: Add cycles - 2S + 1N
        return;
    }

    // Real BIOS path:
    // Save current CPSR into the SPSR_SVC
    cpuState.spsr[BANK_SVC].value = cpuState.cpsr.value;

    SwitchMode(Mode::SVC);
    cpuState.cpsr.fields.thumb = 0;
    cpuState.cpsr.fields.irq_disable = 1;

    u32 returnAddress = cpuState.r15 - 4;
    cpuState.r14 = returnAddress;
    cpuState.r15 = 0x08;
    FlushPipeline();
}

void GBA_CPU::ARM_UndefinedInstruction(u32 instruction)
{
    // Save current CPSR into the SPSR_UND
    cpuState.spsr[BANK_UND] = cpuState.cpsr;

    // Switch mode and disable IRQ and Thumb
    SwitchMode(Mode::UND);
    cpuState.cpsr.fields.irq_disable = 1;

    u32 returnAddress = cpuState.r15 - 4;
    cpuState.r14 = returnAddress;
    cpuState.r15 = 0x04;
    FlushPipeline();
}