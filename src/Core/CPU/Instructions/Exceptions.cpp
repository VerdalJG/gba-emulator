#include "Core/CPU/Instructions/Exceptions.hpp"
#include "Core/CPU/GBA_CPU.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_HLE.hpp"

void UndefinedInstruction(uint32_t instruction, GBA_CPU& cpu)
{
    // Program counter was 2 steps ahead, simulating pipeline offset
    uint32_t faultAddress = cpu.GetValueAtRegister(GBA_CPU::PC_INDEX) - (cpu.IsThumbMode() ? 4u : 8u); 

    // Store CPSR into SPSR_undefined
    int exceptionModeIndex = BankIndex(OperatingMode::Undefined);
    cpu.SaveCPSRIntoSPSR(exceptionModeIndex);

    // Make mode-specific registers visible
    cpu.UpdateVisibleRegistersForMode(OperatingMode::Undefined);

    // Save address one step ahead of where we were at
    cpu.SetValueAtRegister(GBA_CPU::LR_INDEX, + (cpu.IsThumbMode() ? 2u : 4u));
    
    // Update CPSR for undefined mode
    uint32_t iBit = 1 << 7; // Disable IRQ interrupts
    uint32_t undefinedMode = static_cast<uint32_t>(OperatingMode::Undefined);
    
    uint32_t updatedCPSR = iBit | undefinedMode;

    // Preserve F bit (bit 6)
    // Normally bit 5 is preserved, but GBA sets to ARM Mode- Bit 5 == 0
    uint32_t bitsToUpdate = 0xCF; // 0b10111111 
    cpu.UpdateCPSR(updatedCPSR, bitsToUpdate);

    // Branch to Vector - BIOS region (0x04) for Undefined instruction
    cpu.SetValueAtRegister(GBA_CPU::PC_INDEX, 0x04);
}

void SoftwareInterrupt(uint32_t instruction, GBA_CPU &cpu)
{
    // HLE BIOS
    if (cpu.GetEmulatorCore()->GetUsingHLE())
    {
        uint32_t swiNumber = instruction & 0x00FFFFFF;
        cpu.GetEmulatorCore()->GetHLE().HandleSWI(swiNumber, cpu);
        return;
    }

    // Real BIOS path

    // Program counter was 2 steps ahead, simulating pipeline offset
    uint32_t interruptAddress = cpu.GetValueAtRegister(GBA_CPU::PC_INDEX) - (cpu.IsThumbMode() ? 4u : 8u); 

    // Store CPSR into SPSR_supervisor
    int exceptionModeIndex = BankIndex(OperatingMode::Supervisor);
    cpu.SaveCPSRIntoSPSR(exceptionModeIndex);

    // Make mode-specific registers visible
    cpu.UpdateVisibleRegistersForMode(OperatingMode::Supervisor);

    // Save address one step ahead of where we were at
    cpu.SetValueAtRegister(GBA_CPU::LR_INDEX, interruptAddress + (cpu.IsThumbMode() ? 2u : 4u));

    // Update CPSR for supervisor mode
    uint32_t iBit = 1 << 7; // Disable IRQ interrupts
    uint32_t supervisorMode = static_cast<uint32_t>(OperatingMode::Supervisor);
    
    uint32_t updatedCPSR = iBit | supervisorMode;

    // Preserve F bit (bit 6)
    // Normally bit 5 is preserved, but GBA sets to ARM Mode- Bit 5 == 0
    uint32_t bitsToUpdate = 0xCF; // 0b10111111 
    cpu.UpdateCPSR(updatedCPSR, bitsToUpdate);

    // Branch to Vector - BIOS region (0x08) for Software interrupt instruction
    cpu.SetValueAtRegister(GBA_CPU::PC_INDEX, 0x08);
}