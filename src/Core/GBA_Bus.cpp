#include "Core/GBA_Bus.hpp"
#include "Core/Memory/GBA_Memory_Helpers.hpp"
#include "Core/Memory/GBA_Memory.hpp"
#include "Core/GBA_PPU.hpp"
#include "Core/GBA_APU.hpp"
#include "Core/GBA_DMAController.hpp"
#include "Core/GBA_CPU.hpp"
#include "Core/GBA_IO.hpp"
#include "Core/EmulatorCore.hpp"
#include "GBA_Bus.hpp"


GBA_Bus::GBA_Bus(EmulatorCore* core, GBA_Memory& memory, GBA_IO& io) : core(core), memory(memory), io(io)
{
    
}

void GBA_Bus::AttachSubsystems(GBA_PPU* ppu, GBA_APU* apu, GBA_DMAController* dma, GBA_CPU* cpu) 
{
    this->ppu = ppu;
    this->apu = apu;
    this->dma = dma;
    this->cpu = cpu;
}

void GBA_Bus::UpdateLatestAccessValues(u32 value, u32 address, RegionType region, AccessSize accessSize) 
{
    lastAccess.value = value;
    lastAccess.address = address;
    lastAccess.region = region;
    lastAccess.size = accessSize;
}

bool GBA_Bus::IsSequential(u32 address, AccessSize accessSize, RegionType region) 
{ 
    if (GetBusDomain(lastAccess.region) != GetBusDomain(region))
        return false;

    if (lastAccess.size != accessSize)
        return false;

    if (address != lastAccess.address + static_cast<size_t>(accessSize))
        return false;

    // --- Game Pak ROM 128 KB boundary rule ---
    if (GetBusDomain(region) == BusDomain::GamePakROM)
    {
        constexpr u32 BLOCK_MASK = ~0x1FFFFu; // 128 KB
        if ((address & BLOCK_MASK) != (lastAccess.address & BLOCK_MASK))
        {
            return false;
        }
    }

    return true;
}

void GBA_Bus::InvalidateSequentiality() 
{
    accessForcedNonSequential = true;
}

BusDomain GBA_Bus::GetBusDomain(RegionType region) const
{
    switch (region)
    {
        case RegionType::ROM0:
        case RegionType::ROM1:
        case RegionType::ROM2:
            return BusDomain::GamePakROM;

        default:
            return BusDomain::Other;
    }
}

u32 GBA_Bus::OpenBus(u32 address) 
{ 
    RegionType region = GetRegionType(address);

    const std::string message = "Open bus read at address: " + std::to_string(address);
    core->Log(message, LogType::Warning);

    // Calculate shift in case of mis-aligned address (word alignment)
    uint shift = (address & 3) * 8;

    // TODO: DMA Open bus implementation

    u32 result;

    if (cpu->IsThumbMode())
    {
        // LSW and MSW mean less/most significant halfword
        u32 lsw;
        u32 msw;

        u32 pc = cpu->ReadRegister(15);

        switch (region)
        {
            // 16-bit bus
            case RegionType::EWRAM:
            case RegionType::PaletteRAM:
            case RegionType::VRAM:
            case RegionType::ROM0:
            case RegionType::ROM1:
            case RegionType::ROM2:
            {
                lsw = cpu->GetPipelineOpcode(0);
                msw = cpu->GetPipelineOpcode(0);
                break;
            }

            // 32-bit bus
            case RegionType::BIOS:
            case RegionType::OAM:
            {
                if ((pc & 3) == 0) // Aligned
                {
                    // $+6 isn't fetched yet so we duplicate the halfword
                    lsw = cpu->GetPipelineOpcode(0);
                    msw = cpu->GetPipelineOpcode(0);
                }
                else // Misaligned
                {
                    lsw = cpu->GetPipelineOpcode(0);
                    msw = cpu->GetPipelineOpcode(1); 
                }
                break;
            }

            // Special case
            case RegionType::IWRAM:
            {
                if ((pc & 3) == 0) // Aligned
                {
                    lsw = cpu->GetPipelineOpcode(0);
                    msw = cpu->GetPipelineOpcode(1);
                }
                else // Misaligned
                {
                    lsw = cpu->GetPipelineOpcode(1);
                    msw = cpu->GetPipelineOpcode(0);
                }
                break;
            }
        }
        result = (msw << 16) | lsw;
    }
    else // ARM
    {
        result = cpu->GetPipelineOpcode(0); // Get instruction in the Fetch slot
    }

    return result >> shift;
}

void GBA_Bus::HandleAccessCycles(u32 address, MemoryRegion* region, AccessSize size, uint accesses, BusRequester requester) 
{
    bool sequential;

    if (accessForcedNonSequential)
    {
        sequential = false;
        accessForcedNonSequential = false;
    }
    else
    {
        sequential = IsSequential(address, size, region->type);
    }
    
    u32 cycles = waitstateController.GetCycles(region->type, sequential);

    // The CPU must wait 1 cycle if the ppu is currently accessing video memory
    if (requester == BusRequester::CPU && ppu->IsAccessingVideoMemory()) 
    {
        if (region->IsVideoMemory()) cycles += accesses;
    }

    // Sometimes an access can be split up into 4 8-bit reads or 2 16-bit reads 
    for (uint i = 1; i < accesses; i++)
    {
        cycles += waitstateController.GetCycles(region->type, true);
    }

    cpu->AddCycles(cycles);
}

u32 GBA_Bus::GetMirroredAddress(u32 address, MemoryRegion* region) 
{
    u32 offset = 0;
    if (region->mirroring == Mirroring::Mirror) // EWRAM, IWRAM, OAM, Palette RAM, 
    {
        offset = (address - region->start) & (region->physicalSize - 1);
    }
    else if (region->mirroring == Mirroring::NoMirror) // ROM, BIOS
    {
        offset = (address - region->start);
    }
    else // SpecialMirror (VRAM, SRAM, IO) - IO mirroring handled inside IO class
    {
        if (type == RegionType::VRAM)
        {
            uint32_t relativeAddress = (address - VRAM_START);

            // Mirror every 128 KB (64KB + 32KB + 32KB(mirror of first 32KB))
            uint32_t windowOffset = relativeAddress & (VRAM_MIRROR_SIZE - 1);

            // Handle 32KB mirror case inside of the 128KB
            if (windowOffset >= VRAM_TOTAL_SIZE)
            {
                // Mirror OBJ VRAM (-0x8000)
                windowOffset -= VRAM_OBJ_SIZE;
            }

            // Now it is guranteed to be within 0x18000 window
            offset = windowOffset;
        }

        if (type == RegionType::SRAM)
        {
            uint32_t relativeAddress = address - SRAM_START;
            
            // Mirror across 32MB region
            offset = relativeAddress & (SRAM_SIZE - 1);

            // Mirror upper 32KB onto the lower 32KB
            offset &= (SRAM_MIRROR_SIZE - 1);
        }
    }

    return region->start + offset;
}

const RegionType GBA_Bus::GetRegionType(u32 address) const 
{
    switch (address >> 24)
    {
        case 0x00: return RegionType::BIOS;
        case 0x02: return RegionType::EWRAM;
        case 0x03: return RegionType::IWRAM;
        case 0x04: return RegionType::IO;
        case 0x05: return RegionType::PaletteRAM;
        case 0x06: return RegionType::VRAM;
        case 0x07: return RegionType::OAM;
        case 0x08: case 0x09: return RegionType::ROM0;
        case 0x0A: case 0x0B: return RegionType::ROM1;
        case 0x0C: case 0x0D: return RegionType::ROM2;
        case 0x0E: return RegionType::SRAM;

        default: return RegionType::Invalid;
    }
}
