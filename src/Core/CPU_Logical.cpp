#include "Core/GBA_CPU.hpp"
#include "Core/InstructionHelpers.hpp"

void GBA_CPU::LogicalAND(uint32_t instruction)
{
    auto operation = [](uint32_t a, uint32_t b) { return a & b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::LogicalExclusiveOR(uint32_t instruction)
{
    auto operation = [](uint32_t a, uint32_t b) { return a ^ b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::LogicalOR(uint32_t instruction)
{
    auto operation = [](uint32_t a, uint32_t b) { return a | b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::BitClear(uint32_t instruction)
{
    auto operation = [](uint32_t a, uint32_t b) { return a & ~b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::Move(uint32_t instruction)
{
    auto operation = [](uint32_t, uint32_t b) { return b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::MoveNot(uint32_t instruction)
{
    auto operation = [](uint32_t, uint32_t b) { return ~b; };
    LogicalOperation(instruction, operation);
}


void GBA_CPU::Test(uint32_t instruction)
{
    auto operation = [](int a, int b) { return a & b; };
    LogicalOperation(instruction, operation);
}

void GBA_CPU::TestEquivalence(uint32_t instruction)
{
    auto operation = [](int a, int b) { return a ^ b; };
    LogicalOperation(instruction, operation);
}


