#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/Instructions/Decoder.hpp"
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
    uint32_t instruction = memorySystem.Read32(visibleRegisters[15]);
    
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

void GBA_CPU::RestoreCPSRFromSPSR(int oldExceptionModeIndex)
{
    cpsr = spsr[oldExceptionModeIndex];
}

void GBA_CPU::SaveCPSRIntoSPSR(int exceptionModeIndex)
{
    spsr[exceptionModeIndex] = cpsr;
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



void GBA_CPU::HandleUndefinedBehavior(uint32_t instruction)
{
    // In an emulator, treat it as NOP (No operation), and just step to next instruction
}
