#include "Core/GBA_CPU.hpp"
#include "Core/CPU/Instructions/ARM/Decoder.hpp"
#include "Core/GBA_Memory.hpp"
#include "Core/EmulatorCore.hpp"
#include "Core/GBA_WaitstateController.hpp"
#include "Core/CPU/CPU_Timings.hpp"
#include "Core/GBA_Bus.hpp"
#include "Core/CPU/Instructions/ARM/Conditions.hpp"
#include "Core/CPU/Instructions/ARM/Shifts.hpp"

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
    WriteRegister(CPU_Registers::PC_INDEX, 0x00000000);
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
    Execute(); // Instructions advance the program counter if appropriate
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
    pipeline[2] = pipeline[1]; // Move [1] to [2];
    pipeline[1] = pipeline[0]; // Move [0] to [1];
}



void GBA_CPU::FlushPipeline()
{
    pipeline[0].valid = false;
    pipeline[1].valid = false;
    pipeline[2].valid = false;
    nextInstructionFetchIsSequential = false;   
}

void GBA_CPU::Fetch()
{
    Instruction newInstruction;

    u32 address = cpuState.r15;

    if (IsThumbMode())
    {
        newInstruction.rawInstruction = Read16(address);
    }
    else
    {
        newInstruction.rawInstruction = Read32(address);
    }
    newInstruction.valid = true;
    pipeline[0] = newInstruction;
    nextInstructionFetchIsSequential = true;
}

void GBA_CPU::Decode()
{
    if (!pipeline[1].valid) return;

    InstructionFunction functionToExecute = nullptr;
    
    // Thumb mode does not use condition bits
    if (IsThumbMode()) 
    {
        // Handle Thumb mode here
        return;
    }
    else // ARM Mode
    {
        Condition condition = GetConditionType(pipeline[1].rawInstruction);
        if (condition == CONDITION_UD)
        {
            HandleUndefinedBehavior(pipeline[1].rawInstruction);
            return;
        }

        if (ConditionPassed(condition))
        {
            functionToExecute = DecodeInstruction(pipeline[1].rawInstruction, *this);
        }
    }

    if (functionToExecute == nullptr) return;

    // Store function pointer in pipeline
    pipeline[1].function = functionToExecute;
}

void GBA_CPU::Execute()
{
    if (!pipeline[2].valid) return;
    if (pipeline[2].function == nullptr) return; // No-Op

    // Execute the instruction
    pipeline[2].function(pipeline[2].rawInstruction, *this);
}


bool GBA_CPU::ConditionPassed(Condition condition)
{
    if (condition == CONDITION_AL)
    {
        return true;
    }
    return conditionTable[(static_cast<int>(condition) << 4) | (cpuState.cpsr.value >> 28)];
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

u32 GBA_CPU::Read8(u32 address)
{
    u32 cycles = 0;
    u32 readValue = bus.Read8(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return readValue;
}

u32 GBA_CPU::Read16(u32 address)
{
    u32 cycles = 0;
    u32 readValue = bus.Read16(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return readValue;
}

u32 GBA_CPU::Read32(u32 address)
{
    u32 cycles = 0;
    u32 readValue = bus.Read32(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return readValue;
}

void GBA_CPU::Write8(u32 address, u8 value)
{
    u32 cycles = 0;
    bus.Write8(address, value, BusRequester::CPU, &cycles);
    AddCycles(cycles);
}

void GBA_CPU::Write16(u32 address, u16 value)
{
    u32 cycles = 0;
    bus.Write16(address, value, BusRequester::CPU, &cycles);
    AddCycles(cycles);
}

void GBA_CPU::Write32(u32 address, u32 value)
{
    u32 cycles = 0;
    bus.Write32(address, value, BusRequester::CPU, &cycles);
    AddCycles(cycles);
}

u32 GBA_CPU::Read16_Rotated(u32 address) 
{
    u32 cycles = 0;
    u32 value = bus.Read16(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);

    if (address & 1)
    {
        value = (value >> 8) | (value << 24);
    }

    return value;
}

u32 GBA_CPU::Read32_Rotated(u32 address) 
{
    u32 cycles = 0;
    u32 value = bus.Read32(address, BusRequester::CPU, &cycles);
    u32 shift = (address & 3) * 8;
    
    AddCycles(cycles);
    return value >> shift | (value << (32 - shift));
}

u32 GBA_CPU::Read8_SignExtended(u32 address) 
{ 
    u32 cycles = 0;
    u8 value = bus.Read8(address, BusRequester::CPU, &cycles);
    AddCycles(cycles);
    return SignExtend_8(value); 
}

u32 GBA_CPU::Read16_SignExtended(u32 address) 
{ 
    u32 cycles = 0;

    if (address & 1) // Misaligned
    {
        u8 value = bus.Read8(address, BusRequester::CPU, &cycles);
        AddCycles(cycles);
        return SignExtend_8(value);
    }
    else
    {
        u16 value = bus.Read16(address, BusRequester::CPU, &cycles);
        AddCycles(cycles);
        return SignExtend_16(value);
    }
}

void GBA_CPU::InvalidateSequentiality() 
{
    bus.InvalidateSequentiality();
}
