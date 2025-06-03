#include "SDLUtils.hpp"

void AddInitFlag(SDL_InitFlags &flags, int flagToAdd)
{
    flags |= flagToAdd;
}

void RemoveInitFlag(SDL_InitFlags &flags, int flagToRemove)
{
    flags &= ~flagToRemove;
}

void AddWindowFlag(SDL_WindowFlags &flags, int flagToAdd)
{
    flags |= flagToAdd;
}

void RemoveWindowFlag(SDL_WindowFlags &flags, int flagToRemove)
{
    flags &= ~flagToRemove;
}

bool InitSDL(SDL_InitFlags flags)
{
    if (!SDL_InitSubSystem(flags)) // SDL_Init calls SDL_InitSubSystem, returns 0 on success
    {
        SDL_Log("SDL_InitSubsystem error: %s", SDL_GetError());
        return false;
    }
    return true;
}

bool CreateWindow(SDL_Window *&window, SDL_WindowFlags flags, const char *title, int width, int height)
{
    window = SDL_CreateWindow(title, width, height, flags);
    if (!window)
    {
        SDL_Log("SDL_CreateWindow error: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }
    return true;
}

bool CreateRenderer(SDL_Renderer *&renderer, SDL_Window *window, const char *driverName)
{
   renderer = SDL_CreateRenderer(window, NULL); // NULL makes SDL choose the best available renderer
    if (!renderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    } 

    return true;
}
