#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Instructions/Decoder.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_WaitstateController.hpp"
#include "Core/CPU/CPU_Timings.hpp"
#include "Core/GBA_Bus.hpp"
#include "Core/CPU/Instructions/Conditions.hpp"

#include <assert.h>

GBA_CPU::GBA_CPU(EmulatorCore* core, GBA_Bus& bus) : core(core), bus(bus)
{
    assert(core && "CPU must have valid EmulatorCore object");
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

    Fetch();
    Decode();
    Execute();

    // Advance program counter
    visibleRegisters[PC_INDEX] += (IsThumbMode() ? 2u : 4u);
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
    instructionPipeline[2] = instructionPipeline[1]; // Move [1] to [2];
    instructionPipeline[1] = instructionPipeline[0]; // Move [0] to [1];
}



void GBA_CPU::FlushPipeline()
{
    instructionPipeline[0].valid = false;
    instructionPipeline[1].valid = false;
    instructionPipeline[2].valid = false;
    nextInstructionFetchIsSequential = false;   
}

void GBA_CPU::Fetch()
{
    Instruction newInstruction;
    newInstruction.rawInstruction = Read32_Bus(visibleRegisters[PC_INDEX]);
    newInstruction.valid = true;
    instructionPipeline[0] = newInstruction;
    nextInstructionFetchIsSequential = true;
}

void GBA_CPU::Decode()
{
    if (!instructionPipeline[1].valid) return;

    InstructionFunction functionToExecute = nullptr;
    
    // Thumb mode does not use condition bits
    if (IsThumbMode()) 
    {
        // Handle Thumb mode here
        return;
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
            functionToExecute = DecodeInstruction(instructionPipeline[1].rawInstruction, *this);
        }
    }

    if (functionToExecute == nullptr) return;

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

uint8_t GBA_CPU::Read8_Bus(uint32_t address)
{
    uint32_t cycles = 0;
    uint8_t readValue = bus.Read8(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return readValue;
}

uint16_t GBA_CPU::Read16_Bus(uint32_t address)
{
    uint32_t cycles = 0;
    uint16_t readValue = bus.Read16(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return readValue;
}

uint32_t GBA_CPU::Read32_Bus(uint32_t address)
{
    uint32_t cycles = 0;
    uint32_t readValue = bus.Read32(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return readValue;
}

void GBA_CPU::Write8_Bus(uint32_t address, uint8_t value)
{
    uint32_t cycles = 0;
    bus.Write8(address, value, BusRequester::CPU, &cycles);
    AddCycles(cycles);
}

void GBA_CPU::Write16_Bus(uint32_t address, uint16_t value)
{
    uint32_t cycles = 0;
    bus.Write16(address, value, BusRequester::CPU, &cycles);
    AddCycles(cycles);
}

void GBA_CPU::Write32_Bus(uint32_t address, uint32_t value)
{
    uint32_t cycles = 0;
    bus.Write32(address, value, BusRequester::CPU, &cycles);
    AddCycles(cycles);
}

void GBA_CPU::InvalidateSequentiality() 
{
    bus.InvalidateSequentiality();
}
