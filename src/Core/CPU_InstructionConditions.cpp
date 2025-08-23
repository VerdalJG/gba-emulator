#include "Core/GBA_CPU.hpp"
#include "Core/CPU_InstructionConditions.hpp"

bool GBA_CPU::InstructionConditionCheck(uint8_t conditionByte)
{
    InstructionCondition condition = static_cast<InstructionCondition>(conditionByte);

    switch (condition)
    {
        case InstructionCondition::EQ:
        return GetCpsrZ();

        case InstructionCondition::NE:
        return GetCpsrZ() == 0;

        case InstructionCondition::CS:
        return GetCpsrC();

        case InstructionCondition::CC:
        return GetCpsrC() == 0;

        case InstructionCondition::MI:
        return GetCpsrN();

        case InstructionCondition::PL:
        return GetCpsrN() == 0;

        case InstructionCondition::VS:
        return GetCpsrV();

        case InstructionCondition::VC:
        return GetCpsrV() == 0;

        case InstructionCondition::HI:
        return GetCpsrC() && (GetCpsrZ() == 0);

        case InstructionCondition::LS:
        return (GetCpsrC() == 0) || GetCpsrZ();

        case InstructionCondition::GE:
        return GetCpsrN() == GetCpsrV();

        case InstructionCondition::LT:
        return GetCpsrN() != GetCpsrV();

        case InstructionCondition::GT:
        return GetCpsrZ() == 0 && GetCpsrN() == GetCpsrV();

        case InstructionCondition::LE:
        return GetCpsrZ() || GetCpsrN() != GetCpsrV();

        case InstructionCondition::AL:
        return true;

        default:
        return false;
    }
}