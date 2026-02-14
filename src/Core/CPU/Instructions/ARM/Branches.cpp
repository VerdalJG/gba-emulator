#include "Core/CPU/Instructions/ARM/Branches.hpp"
#include "Core/CPU/Instructions/ARM/Shifts.hpp"
#include "Core/GBA_CPU.hpp"


void Branch(uint32_t instruction, GBA_CPU& cpu)
{
    bool saveToLinkRegister = (instruction >> 24) & 1;
    uint32_t currentPC = cpu.ReadRegister(GBA_CPU::PC_INDEX);

    if (saveToLinkRegister)
    {
        // Current instruction address is PC - 8, so next instruction is PC - 4
        // (due to pipelining)
        cpu.WriteRegister(GBA_CPU::LR_INDEX, currentPC - 4);
    }

    // Sign extend to 30 bits, then shift left 2 times, then add to current PC
    uint32_t signedImmediate_24 = instruction & 0xFFFFFF;
    uint32_t shifted_24 = LogicalShiftLeft(signedImmediate_24, 6);
    int32_t signExtended_30 = static_cast<int32_t>(ArithmeticShiftRight(shifted_24, 6));

    int32_t targetAddress = (signExtended_30 << 2) + currentPC;
    
    targetAddress &= ~3u; // Word-alignment
    cpu.WriteRegister(GBA_CPU::PC_INDEX, targetAddress);
}

// NOTE: Should only change thumb mode flag via this function, changing it directly is UNPREDICTABLE
void BranchAndExchange(uint32_t instruction, GBA_CPU& cpu)
{
    uint32_t rmIndex = instruction & 0xF;
    uint32_t rm = cpu.ReadRegister(rmIndex);
    
    // Check for UNPREDICTABLE: branch to half-word misaligned in ARM state
    if ((rm & 3) == 0b10) return;

    // Switch to arm/thumb if needed
    cpu.UpdateCPSR((rm & 1) << 5, 0x10); // Update bit 5, ARM = 0, Thumb = 1
    rm &= ~3u; // Word-alignment
    cpu.WriteRegister(GBA_CPU::PC_INDEX, rm & 0xFFFFFFFE); // Branch to the address held
}