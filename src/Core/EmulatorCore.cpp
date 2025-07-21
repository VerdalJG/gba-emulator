#include "EmulatorCore.hpp"
//#include "SDLUtils.hpp"

EmulatorCore::EmulatorCore() : memory(GBAMemory())
{
}

bool EmulatorCore::Init()
{
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
    memory.LoadROM(romData);
    return true; // Assume it is loaded successfully for now
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

void EmulatorCore::Update()
{
    // HandleSDLEvents(); // Handle SDL events like input, window events, etc.

    // if (!isRunning)
    // {
    //     return; // Exit if we reached a quit event
    // }

    // // Update Timer
    // timer.Update();

    // if (timer.ShouldRunNewFrame())
    // {
    //     Tick(); // Tick all components (CPU, PPU, APU, etc.)
    // }
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
