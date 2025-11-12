#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Instructions/Decoder.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_WaitstateController.hpp"

#include <assert.h>


GBA_CPU::GBA_CPU(EmulatorCore* core) :
    core(core)
{
    assert(core != nullptr && "CPU must have valid EmulatorCore object");
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
    totalCycles = 0;
}

void GBA_CPU::Step()
{
    if (halted)
    {
        HandleHalt();
        return; // No instructions/cycles during cpu halt
    }

    currentInstructionCycles = 0;

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

void GBA_CPU::HandleHalt()
{
    if (/*interrupts.AnyPendingEnabled(*this)*/ true)
    {
        halted = false; 
    }
    else
    {
        // Advance timers
        // Handle DMA
    }
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
    newInstruction.rawInstruction = GetMemorySystem().Read32(visibleRegisters[PC_INDEX]);
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

void GBA_CPU::AddCycles(uint32_t cycles)
{
    if (cycles == 0) return;

    currentInstructionCycles += cycles;
    currentFrameCycles += cycles;
    totalCycles += cycles;
}

GBA_Memory &GBA_CPU::GetMemorySystem() 
{
    return core->GetMemory();
}

EmulatorCore* GBA_CPU::GetEmulatorCore()
{
    return core;
}

void GBA_CPU::Log(const std::string& message, LogType logType, const char *functionName)
{
    if (core)
    {
       core->Log(message, logType, functionName);
    }
}

uint8_t GBA_CPU::Read8FromMemory(uint32_t address, bool isSequential)
{
    const GBA_Memory& memory = GetMemorySystem();
    uint32_t cycles = memory.GetWaitstateController().GetCycles(address, AccessSize::Byte, isSequential);
    AddCycles(cycles);
    return GetMemorySystem().Read8(address);
}

uint16_t GBA_CPU::Read16FromMemory(uint32_t address, bool isSequential)
{
    const GBA_Memory& memory = GetMemorySystem();
    uint32_t cycles = memory.GetWaitstateController().GetCycles(address, AccessSize::Halfword, isSequential);
    AddCycles(cycles);
    return GetMemorySystem().Read16(address);
}

uint32_t GBA_CPU::Read32FromMemory(uint32_t address, bool isSequential)
{
    const GBA_Memory& memory = GetMemorySystem();
    uint32_t cycles = memory.GetWaitstateController().GetCycles(address, AccessSize::Word, isSequential);
    AddCycles(cycles);
    return GetMemorySystem().Read32(address);
}
