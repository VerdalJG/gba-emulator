#include "Core/CPU/Instructions/ARM/InstructionHelpers.hpp"
#include "Core/CPU/CPU_CPSR.hpp"

template <typename Func>
void ArithmeticOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);
    uint32_t carryIn = ShouldUseCarryIn(values.opcode) ? cpu.GetCPSR_C() : 0;

    uint32_t result = operation(rn, op2.value, carryIn);
    cpu.SetValueAtRegister(values.rdIndex, result);

    // CPSR
    if (!values.setCPSRFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCPSRCase(cpu);
    }

    UpdateCPSR_Arithmetic(values, rn, op2.value, result, cpu);
}

template <typename Func>
void ArithmeticComparisonOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t result = operation(rn, op2.value);

    UpdateCPSR_Arithmetic(values, rn, op2.value, result, cpu);
}

template <typename Func>
void LogicalOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t rd = cpu.GetValueAtRegister(values.rdIndex);

    uint32_t result = operation(rn, op2.value);
    cpu.SetValueAtRegister(values.rdIndex, result);

    if (!values.setCPSRFlag) return;

    if (values.rdIndex == 15)
    {
        HandleProgramCounterCPSRCase(cpu);
    }

    UpdateCPSR_Logical(result, op2, cpu);
}

template <typename Func>
void LogicalTestOperation(DataProcessing_Decoded values, ShifterOperand op2, Func operation, GBA_CPU& cpu)
{
    uint32_t rn = cpu.GetValueAtRegister(values.rnIndex);
    uint32_t result = operation(rn, op2.value);

    UpdateCPSR_Logical(result, op2, cpu);
}