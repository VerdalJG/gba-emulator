#include "Core/EmulatorCore.hpp"
#include "Core/GBA_Memory_Helpers.hpp"
#include "Core/GBA_Bus.hpp"

#include "Utils/Integers.hpp"
#include "Utils/Logger.hpp"

EmulatorCore::EmulatorCore(Logger* logger) : 
    logger(logger), 
    io(this), 
    rom(this), 
    memory(this, rom, io),
    bus(this, memory),
    ppu(this, bus, io.GetLCDRegisters()),
    apu(this, bus, io.GetSoundRegisters()), 
    dmaController(this, bus, io.GetDMARegisters()), 
    timerController(this, io.GetTimerRegisters()), 
    interruptController(this, io.GetInterruptRegisters()), 
    keypad(this, io.GetKeypadRegisters()),  
    cpu(this, bus),
    hle(this, memory, io, cpu)
{
    if (logger)
    {
        // Logging file
        logger->Log("Emulator started", LogType::Info);
    }

    io.AttachSubsystems(
        &ppu, 
        &apu, 
        &dmaController, 
        &timerController, 
        &interruptController, 
        &keypad, 
        &bus.GetWaitstateController()
    );

    bus.AttachSubsystems(
        &ppu,
        &apu,
        &dmaController
    );
}

bool EmulatorCore::InitializeCPU()
{
    cpu.Reset();

    // SDL_SetAppMetadata("GBAEmu", "Version 0.1", "GBAEmulator");

    // // Initialize SDL
    // SDL_InitFlags flags = 0;
    // AddInitFlag(flags, SDL_INIT_VIDEO); // Also initializes the events subsystem - must be on main thread
    // AddInitFlag(flags, SDL_INIT_AUDIO); // Also initializes the events subsystem
    // if (!InitSDL(flags))
    // {
    //     return false;
    // }

    // // Window creation
    // SDL_WindowFlags windowFlags = 0;
    // AddWindowFlag(windowFlags, SDL_WINDOW_RESIZABLE); // Allow resizing the window
    // SDL_Window* window;
    // if (!CreateWindow(window, windowFlags))
    // {
    //     return false;
    // }

    // // Renderer creation
    // SDL_Renderer* renderer;
    // if (!CreateRenderer(renderer, window))
    // {
    //     return false;
    // }

    // // Initialize other components like CPU, PPU, APU, etc.

    return true;
}

void EmulatorCore::LoadBIOS(const std::vector<uint8_t> &biosData)
{
    memory.LoadBIOS(biosData);
}

bool EmulatorCore::LoadROM(const std::vector<uint8_t>& romData)
{
    if (romData.empty() || romData.size() > ROM_BANK_SIZE)
    {
        printf("ROM size invalid: %zu bytes\n", romData.size());
        return false;
    }

    rom.LoadROM(romData);
    memory.InitROMBanks();

    return true;
}

void EmulatorCore::Run()
{
    // while (isRunning)
    // {
    //     Update();
    // }
}

void EmulatorCore::Shutdown()
{
    // // Clean up resources
    // if (renderer)
    // {
    //     SDL_DestroyRenderer(renderer);
    //     renderer = nullptr;
    // }
    // if (window)
    // {
    //     SDL_DestroyWindow(window);
    //     window = nullptr;
    // }
    // SDL_Quit(); // Quit SDL subsystems
}

void EmulatorCore::RunFrame()
{
    while (!ppu.FrameReady())
    {
        cpu.Step();
        u32 cycles = cpu.GetCycles();
        dmaController.Step(cycles);
        ppu.Step(cycles);
        apu.Step(cycles);
    }

    // add Render() here?
}

void EmulatorCore::Tick()
{
    //int cyclesToRun = timer.GetCyclesToRunThisFrame();
    //cpu.Tick(cyclesToRun); // Run the CPU for the number of cycles needed for this frame
    //ppu.Tick(cyclesToRun); // Update PPU if implemented
    //apu.Tick(cyclesToRun); // Update APU if implemented
    //Render(); // Render the current frame
}

void EmulatorCore::Render()
{
    // 1. CPU/PPU renders to framebuffer (internal GBA memory)
    //cpu.Render(); // or ppu.Render();

    // 2. Application reads that framebuffer and draws it via SDL
}

void EmulatorCore::HandleSDLEvents()
{
//     // Handle input, update game state, etc.
//     SDL_Event event;
//     while (SDL_PollEvent(&event))
//     {
//         if (event.type == SDL_EVENT_QUIT)
//         {
//             isRunning = false;
//             break;
//         }
//         // Handle other events like keyboard, mouse, etc.
//     }
}

void EmulatorCore::PostStatus(const std::string &message)
{
    if (postStatusCallback) 
    {
        postStatusCallback(message);
    }
}

void EmulatorCore::SetPostStatusCallback(std::function<void(const std::string&)> callback)
{
    postStatusCallback = std::move(callback);
}

void EmulatorCore::Log(const std::string& message, LogType logType, const char *functionName)
{
    if (logger)
    {
        logger->Log(message, logType, functionName);
    }
}

