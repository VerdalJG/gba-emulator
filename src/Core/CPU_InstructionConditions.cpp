#include "Core/GBA_CPU.hpp"
#include "Core/CPU_InstructionConditions.hpp"

bool GBA_CPU::InstructionConditionCheck(uint8_t conditionByte)
{
    InstructionCondition condition = static_cast<InstructionCondition>(conditionByte);

    switch (condition)
    {
        case InstructionCondition::EQ:
        return GetCPSR_Z();

        case InstructionCondition::NE:
        return GetCPSR_Z() == 0;

        case InstructionCondition::CS:
        return GetCPSR_C();

        case InstructionCondition::CC:
        return GetCPSR_C() == 0;

        case InstructionCondition::MI:
        return GetCPSR_N();

        case InstructionCondition::PL:
        return GetCPSR_N() == 0;

        case InstructionCondition::VS:
        return GetCPSR_V();

        case InstructionCondition::VC:
        return GetCPSR_V() == 0;

        case InstructionCondition::HI:
        return GetCPSR_C() && (GetCPSR_Z() == 0);

        case InstructionCondition::LS:
        return (GetCPSR_C() == 0) || GetCPSR_Z();

        case InstructionCondition::GE:
        return GetCPSR_N() == GetCPSR_V();

        case InstructionCondition::LT:
        return GetCPSR_N() != GetCPSR_V();

        case InstructionCondition::GT:
        return GetCPSR_Z() == 0 && GetCPSR_N() == GetCPSR_V();

        case InstructionCondition::LE:
        return GetCPSR_Z() || GetCPSR_N() != GetCPSR_V();

        case InstructionCondition::AL:
        return true;

        default:
        return false;
    }
}