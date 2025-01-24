#include <iostream>

#include <SDL3/SDL.h>

int main(int, char**){
    std::cout << "Hello, from GBA-Emulator!\n";

    std::cout << "ABS: " << SDL_abs(-1) << std::endl;
}
