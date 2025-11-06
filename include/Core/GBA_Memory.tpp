

template <typename T>
T GBA_Memory::Read(uint32_t address, AccessSize size)
{
    // Align if needed  
    if (sizeof(T) > 1) address &= ~(sizeof(T) - 1u);

    MemoryRegion* region = GetRegionFromAddress(address);

    if (!region || !region->data)
    {
        const std::string message = "Read in invalid region at: " + std::to_string(address);
        Log(message, LogType::Warning);
        
        return FillFromLastBusAccess<T>();
    }

    const std::vector<uint8_t>& regionData = *region->data;
    size_t offset = address - region->startAddress;

    if (offset + sizeof(T) - 1 >= regionData.size())
    {
        const std::string message = "Read of " + std::to_string(sizeof(T)) + " bytes in multiple regions at: " + std::to_string(address);
        Log(message, LogType::Warning);

        return FillFromLastBusAccess<T>();
    }

    // Create default value (max possible number) for open-bus emulation
    T value = 0;
    for (int i = 0; i < sizeof(T); ++i) 
    {
        value |= static_cast<T>(regionData[offset + i]) << (i * 8);
    }
    lastAccess.UpdateValues(address, sizeof(T), region);
    return value;
}

template <typename T>
void GBA_Memory::Write(uint32_t address, T value)
{
    // Align if needed
    if (sizeof(T) > 1) address &= ~(sizeof(T) - 1u);

    MemoryRegion* region = GetRegionFromAddress(address);

    if (!region || !region->data)
    {
        const std::string message = "Attempting to write to invalid region at: " + std::to_string(address);
        Log(message, LogType::Warning);
        return;
    }

    if (region->permissions == Permissions::ReadOnly)
    {
        const std::string message = "Attempting to write to read-only region at: " + std::to_string(address);
        Log(message, LogType::Warning);
        return;
    }
    
    size_t offset = address - region->startAddress;
    if (offset + sizeof(T) - 1 >= region->data->size())
    {
        const std::string message = "Attempting to write to read-only region at: " + std::to_string(address);
        Log(message, LogType::Warning);
        return;
    }

    for (int i = 0; i < sizeof(T); ++i)
    {
        (*region->data)[offset + i] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
    }
}

template <typename T>
T GBA_Memory::FillFromLastBusAccess() const
{
    if (lastAccess.region == RegionType::Invalid)
    {
        const std::string message = "Attempting to use an invalid last bus access";
        Log(message, LogType::Error);
        return static_cast<T>(0xFFFFFFFFu);
    }

    MemoryRegion* lastRegion = GetRegionFromType(lastAccess.region);
    size_t offset = lastAccess.address - lastRegion->startAddress;
    const std::vector<uint8_t>& regionData = *lastRegion->data;

    T busFill = 0;
    for (int i = 0; i < sizeof(T); ++i)
    {
        busFill |= static_cast<T>(regionData[offset]) << (i * 8);
    }
    return busFill;
}

// THE ERROR IS WITH CONST FUNCTIONS 