#include <iostream>


#define SDL_MAIN_HANDLED 1
#include <SDL3/SDL.h> // This includes ALL SDL headers, no need for anything else

int main(int argc, char* argv[])
{
    SDL_SetAppMetadata("GBAEmu", "Version 0.1", "GBAEmulator");

    // Initialize SDL
    SDL_InitFlags flags = 0;
    flags |= SDL_INIT_VIDEO; // Also initializes the events subsystem - must be on main thread
    flags |= SDL_INIT_AUDIO; // Also initializes the events subsystem
    if (!SDL_InitSubSystem(flags)) // SDL_Init calls SDL_InitSubSystem, returns 0 on success
    {
        SDL_Log("SDL_InitSubsystem error: %s", SDL_GetError());
        return 1;
    }

    
    
    // Window creation
    SDL_Window* window = SDL_CreateWindow("GBAEmu", 960, 540, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        SDL_Log("SDL_CreateWindow error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Renderer creation
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main loop
    bool running = true;
    SDL_Event event;

    const double GBA_FPS = 59.73; // GBA runs at 59.73 FPS
    const double GBA_TICKS_PER_SECOND = 16777216; // GBA CPU runs at 16.78 MHz
    const double frame_duration = 1.0 / GBA_FPS;  // ~16.74 ms - GBA is 59.73FPS

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            // Handle events here
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
                break;
            }

            // Handle other events like keyboard, mouse, etc.
        }

        // Frame timing
        Uint64 currentTick = SDL_GetPerformanceCounter();
        //double deltaTime = (currentTick - )


    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
