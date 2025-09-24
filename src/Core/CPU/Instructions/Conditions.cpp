#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/Instructions/Conditions.hpp"

Condition GetConditionType(uint32_t instruction)
{
    return static_cast<Condition>(instruction >> 28);
}

bool ConditionPassed(Condition condition, GBA_CPU &cpu)
{
    switch (condition)
    {
        case Condition::EQ: return cpu.GetCPSR_Z();
        case Condition::NE: return cpu.GetCPSR_Z() == 0;
        case Condition::CS: return cpu.GetCPSR_C();
        case Condition::CC: return cpu.GetCPSR_C() == 0;
        case Condition::MI: return cpu.GetCPSR_N();
        case Condition::PL: return cpu.GetCPSR_N() == 0;
        case Condition::VS: return cpu.GetCPSR_V();
        case Condition::VC: return cpu.GetCPSR_V() == 0;
        case Condition::HI: return cpu.GetCPSR_C() && (cpu.GetCPSR_Z() == 0);
        case Condition::LS: return (cpu.GetCPSR_C() == 0) || cpu.GetCPSR_Z();
        case Condition::GE: return cpu.GetCPSR_N() == cpu.GetCPSR_V();
        case Condition::LT: return cpu.GetCPSR_N() != cpu.GetCPSR_V();
        case Condition::GT: return cpu.GetCPSR_Z() == 0 && cpu.GetCPSR_N() == cpu.GetCPSR_V();
        case Condition::LE: return cpu.GetCPSR_Z() || cpu.GetCPSR_N() != cpu.GetCPSR_V();
        case Condition::AL: return true;
        default: return false;
    }
}