#include <iostream>

// This is the main entry point for the application
// It initializes SDL and creates the main window and renderer.
#define SDL_MAIN_HANDLED 1
#include <SDL3/SDL.h> // This includes ALL SDL headers, no need for anything else
#include "Application.hpp"

int main(int argc, char* argv[])
{   
    Application app = Application::Application();

    if (!app.Init())
    {
        std::cerr << "Failed to initialize the application." << std::endl;
        return -1;
    }

    app.Run();
    app.Shutdown(); // Clean up resources when the application is done

    return 0;
}
