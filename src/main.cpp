#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include "gbacpu.h"
#pragma message("✅ Included gbacpu.h successfully")

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("WindowTest", 960, 540, SDL_WINDOW_RESIZABLE);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Delay(3000);

    GBA_CPU* cpu = new GBA_CPU();
    delete cpu;
    
    SDL_DestroyWindow(window);
    SDL_Quit();
}
