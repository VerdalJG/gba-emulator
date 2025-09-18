#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/Instructions/CPU_Decoder.hpp"
#include "Core/CPU/CPU_Memory.hpp"
#include <assert.h>



GBA_CPU::GBA_CPU(GBA_Memory& memory) :
    memorySystem(memory)
{

}

GBA_CPU::~GBA_CPU()
{

}

void GBA_CPU::Reset()
{

}

void GBA_CPU::Step()
{
    // Fetch
    uint32_t instruction = memorySystem.Read32(registers[15]);
    
    // Decode
    InstructionFunction operationToExecute;
    
    // Thumb mode does not use condition bits
    if (IsThumbMode()) 
    {
        // Handle Thumb mode here
    }
    else // ARM Mode
    {
        Condition condition = GetConditionType(instruction);
        if (condition == Condition::UD)
        {
            HandleUndefinedBehavior(instruction); // TODO: Change later
            return;
        }

        if (ConditionPassed(condition, *this))
        {
            operationToExecute = DecodeInstruction(instruction);
        }
    }
    
    // Execute
    if (!operationToExecute) return; // Condition failed
    (*operationToExecute)(instruction, *this); // Dereference due to this being a member function pointer
}

void GBA_CPU::RequestInterrupt()
{

}

void GBA_CPU::RestoreCPSRFromSPSR()
{
    switch (opMode)
    {
        case OperatingMode::FIQ: cpsr = spsr_fiq; break;
        case OperatingMode::IRQ: cpsr = spsr_irq; break;
        case OperatingMode::Supervisor: cpsr = spsr_supervisor; break;
        case OperatingMode::Abort: cpsr = spsr_abort; break;
        case OperatingMode::Undefined: cpsr = spsr_undefined; break;
        default:
            assert(false && "No SPSR in User/System mode");
            return;
    }
}

InstructionFunction GBA_CPU::DecodeInstruction(uint32_t instruction)
{
    InstructionPattern pattern = static_cast<InstructionPattern>((instruction >> 26) & 0b11);
    switch (pattern)
    {
        case PATTERN_00: // Data processing and misc
        return DecodePattern00(instruction, *this);

        case PATTERN_01: // Single Data Transfer or Undefined (bit 4 decides)
        return DecodePattern01(instruction, *this); 

        case PATTERN_10: // Block Data Transfer (LDM/STM) or Branch
        return DecodePattern10(instruction, *this);

        case PATTERN_11: // Coprocessor or Software Interrupt
        return DecodePattern11(instruction, *this);

        default: // In theory is unreachable
        //HandleUndefinedBehavior(instruction, *this); - Normally something like this would be called
        return nullptr;
    }
}

void GBA_CPU::UndefinedInstruction(uint32_t instruction)
{
    // Program counter was 2 steps ahead, simulating pipeline offset
    uint32_t faultAddress = registers[15] - 8; 

    // Save address one step ahead of where we were at
    linkRegister_undefined = faultAddress + 4;

    // Save CPSR 
    spsr_undefined = cpsr;
    
    // Update CPSR for undefined mode
    cpsr &= ~0xCF; // 0b10111111 - Only preserving F bit (bit 6), also setting to ARM mode - Bit 5 == 0;
    cpsr |= 1 << 7; // Set I bit == 1;
    cpsr |= static_cast<uint32_t>(OperatingMode::Undefined);

    // Branch to Vector - BIOS region for Undefined instruction
    registers[15] = 0x04;
}

void GBA_CPU::HandleUndefinedBehavior(uint32_t instruction)
{
    // In an emulator, treat it as NOP (No operation), and just step to next instruction
}
