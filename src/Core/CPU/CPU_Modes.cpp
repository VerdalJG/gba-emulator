#include "Core/CPU/GBA_CPU.hpp"
#include "Core/CPU/CPU_CPSR.hpp"

    void GBA_CPU::UpdateVisibleRegistersForMode(OperatingMode newMode)
    {
        OperatingMode oldMode = GetCurrentOperatingMode();

        if (oldMode == newMode) return;

        // Save what the old mode owns

        // Save R8-12_fiq if leaving FIQ
        if (oldMode == OperatingMode::FIQ)
        {
            for (int i = 0; i < 5; ++i)
            {
                fiqR8_R12[i] = visibleRegisters[8 + i];
            }
        }
        else
        {
            if (IsUserOrSystemMode(oldMode))
            {
                for (int i = 0; i < 5; ++i)
                {
                    sharedR8_R12[i] = visibleRegisters[8 + i];
                }
            }
        }

        // Save R13's and R14's when exiting exception modes
        if (IsExceptionMode(oldMode))
        {
            int oldIndex = BankIndex(oldMode);
            bankedR13s[oldIndex] = visibleRegisters[13];
            bankedR14s[oldIndex] = visibleRegisters[14];
        }

        // Restore what the new mode owns

        if (newMode == OperatingMode::FIQ)
        {
            for (int i = 0; i < 5; ++i)
            {
                visibleRegisters[8 + i] = fiqR8_R12[i];
            }
        }
        else
        {
            if (IsUserOrSystemMode(newMode))
            {
                for (int i = 0; i < 5; ++i)
                {
                    visibleRegisters[8 + i] = sharedR8_R12[i];
                }
            }
        }

        // Restore R13's and R14's when exiting exception modes
        if (IsExceptionMode(newMode))
        {
            int newIndex = BankIndex(newMode);
            visibleRegisters[13] = bankedR13s[newIndex];
            visibleRegisters[14] = bankedR14s[newIndex];
        }

        // CPSR updated in other function
    }