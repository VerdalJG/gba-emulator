#include <SDL3/SDL.h>

void AddInitFlag(SDL_InitFlags& flags, int flagToAdd);
void RemoveInitFlag(SDL_InitFlags& flags, int flagToRemove);

void AddWindowFlag(SDL_WindowFlags& flags, int flagToAdd);
void RemoveWindowFlag(SDL_WindowFlags& flags, int flagToRemove);

bool InitSDL(SDL_InitFlags flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO);
bool CreateWindow_SDL(SDL_Window*& window, SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE, const char* title = "GBAEmu", int width = 960, int height = 540);
bool CreateRenderer_SDL(SDL_Renderer*& renderer, SDL_Window* window, const char* driverName = NULL);