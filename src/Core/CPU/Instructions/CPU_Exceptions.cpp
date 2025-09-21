#include "Core/CPU/Instructions/CPU_Exceptions.hpp"
#include "Core/CPU/GBA_CPU.hpp"

void UndefinedInstruction(uint32_t instruction, GBA_CPU& cpu)
{
    // Program counter was 2 steps ahead, simulating pipeline offset
    uint32_t faultAddress = cpu.GetValueAtRegister(GBA_CPU::PC_INDEX) - 8; 

    // Save address one step ahead of where we were at
    //cpu.linkRegister_undefined = faultAddress + 4;

    // Store CPSR into SPSR_undefined
    cpu.SaveCPSRIntoSPSR(OperatingMode::Undefined);
    
    // Update CPSR for undefined mode
    uint32_t iBit = 1 << 7;
    uint32_t undefinedMode = static_cast<uint32_t>(OperatingMode::Undefined);
    uint32_t updatedCPSR = iBit | undefinedMode;
    uint32_t bitsToUpdate = 0xCF; // 0b10111111 - Only preserving F bit (bit 6), also setting to ARM mode - Bit 5 == 0;
    cpu.UpdateCPSR(updatedCPSR ,0xCF);


    // Branch to Vector - BIOS region (0x04) for Undefined instruction
    cpu.SetValueAtRegister(GBA_CPU::PC_INDEX, 0x04);
}