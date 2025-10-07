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
    // Fill registers with 0
    std::fill(std::begin(visibleRegisters), std::end(visibleRegisters), 0);
    cpsr = 0x000000D3;   // Supervisor mode, IRQ/FIQ disabled
    SetValueAtRegister(PC_INDEX, 0x00000000);
    FlushPipeline();
}

void GBA_CPU::Step()
{
    AdvanceInstructionPipeline();

    Execute();
    Decode();
    Fetch();

    AdvanceProgramCounter();
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

void GBA_CPU::AdvanceInstructionPipeline()
{
    if (instructionPipeline[1].valid)
    {
        instructionPipeline[2] = instructionPipeline[1]; // Move [1] to [2];
    }

    if (instructionPipeline[0].valid)
    {
        instructionPipeline[1] = instructionPipeline[0]; // Move [0] to [1];
    }
}

void GBA_CPU::AdvanceProgramCounter()
{
    visibleRegisters[PC_INDEX] += (IsThumbMode() ? 2u : 4u);
}

void GBA_CPU::FlushPipeline()
{
    instructionPipeline[0].valid = false;
    instructionPipeline[1].valid = false;
    instructionPipeline[2].valid = false;
}

void GBA_CPU::Fetch()
{
    Instruction newInstruction;
    newInstruction.rawInstruction = memorySystem.Read32(visibleRegisters[PC_INDEX]);
    newInstruction.valid = true;
    instructionPipeline[0] = newInstruction;
}

void GBA_CPU::Decode()
{
    if (!instructionPipeline[1].valid) return;

    InstructionFunction functionToExecute;
    
    // Thumb mode does not use condition bits
    if (IsThumbMode()) 
    {
        // Handle Thumb mode here
    }
    else // ARM Mode
    {
        Condition condition = GetConditionType(instructionPipeline[1].rawInstruction);
        if (condition == Condition::UD)
        {
            HandleUndefinedBehavior(instructionPipeline[1].rawInstruction);
            return;
        }

        if (ConditionPassed(condition, *this))
        {
            functionToExecute = DecodeInstruction(instructionPipeline[1].rawInstruction);
        }
    }

    // Store function pointer in pipeline
    instructionPipeline[1].function = functionToExecute;
}

void GBA_CPU::Execute()
{
    if (!instructionPipeline[2].valid) return;
    if (instructionPipeline[2].function == nullptr) return; // No-Op

    // Execute the instruction
    instructionPipeline[2].function(instructionPipeline[2].rawInstruction, *this);
}
