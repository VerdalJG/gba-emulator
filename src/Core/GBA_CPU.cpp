#include "Core/GBA_CPU.hpp"


GBA_CPU::GBA_CPU(GBA_Memory& memory) :
    memorySystem(memory)
{

}

GBA_CPU::~GBA_CPU()
{

}

GBA_CPU::InstructionFunction GBA_CPU::dataProcessingFuncTable[GBA_CPU::DATA_PROCESSING_OPCODE_COUNT] = 
{
    &GBA_CPU::LogicalAND,         
    &GBA_CPU::LogicalExclusiveOR,         
    &GBA_CPU::Subtract,        
    &GBA_CPU::ReverseSubtract,         
    &GBA_CPU::Add,       
    &GBA_CPU::AddWithCarry,       
    &GBA_CPU::SubtractWithCarry,         
    &GBA_CPU::ReverseSubtractWithCarry,   
    &GBA_CPU::Test,     
    &GBA_CPU::TestEquivalence,   
    &GBA_CPU::Compare,     
    &GBA_CPU::CompareNegative,    
    &GBA_CPU::LogicalOR,        
    &GBA_CPU::Move,        
    &GBA_CPU::BitClear,      
    &GBA_CPU::MoveNot         
};

void GBA_CPU::Reset()
{

}

void GBA_CPU::Step()
{
    // Fetch
    uint32_t instruction = memorySystem.Read32(ReadProgramCounter(false));
    
    // Decode
    InstructionFunction operationToExecute;
    
    // Thumb mode does not use condition bits
    if (IsThumbMode()) 
    {
        // Handle Thumb mode here
    }
    else // ARM Mode
    {
        uint8_t condition = instruction >> 28;
        if (condition == 0xF)
        {
            HandleUndefinedBehavior(instruction); // TODO: Change later
            return;
        }

        if (InstructionConditionCheck(condition))
        {
            operationToExecute = DecodeARMInstruction(instruction);
        }
    }
    
    // Execute
    if (!operationToExecute) return; // Condition failed
    (this->*operationToExecute)(instruction); // Dereference due to this being a member function pointer
}

void GBA_CPU::RequestInterrupt()
{
}

uint32_t GBA_CPU::GetValueAtRegister(int registerIndex)
{
    if (registerIndex == 15)
    {
        return ReadProgramCounter(true);
    }
    else
    {
        return registers[registerIndex];
    }
}

uint32_t GBA_CPU:: ReadProgramCounter(bool isPartOfInstruction)
{
    // Pipeline offset: if we are reading the PC as part of the behavior of an instruction, 
    // we must simulate the offset that occurs because the actual PC is 2 instructions ahead.
    uint32_t programCounter = registers[15];
    if (isPartOfInstruction)
    {
        // Add to get the actual program counter (PC) - Usually 2 instructions ahead during execution
        programCounter += (IsThumbMode()) ? 4 : 8;
    }

    return programCounter;
}


GBA_CPU::InstructionFunction GBA_CPU::DecodeARMInstruction(uint32_t instruction)
{
    InstructionPattern pattern = static_cast<InstructionPattern>((instruction >> INSTRUCTION_TYPE_SHIFT) & INSTRUCTION_TYPE_MASK);
    switch (pattern)
    {
        case PATTERN_00: // Data processing and misc
        return DecodePattern00(instruction);

        case PATTERN_01: // Single Data Transfer or Undefined (bit 4 decides)
        return DecodePattern01(instruction); 

        case PATTERN_10: // Block Data Transfer (LDM/STM) or Branch
        return DecodePattern10(instruction);

        case PATTERN_11: // Coprocessor or Software Interrupt
        return DecodePattern11(instruction);

        default:
        return &GBA_CPU::HandleUndefinedBehavior;
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
