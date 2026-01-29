#include "SDLWidget.hpp"
#include "SDL/SDLUtils.hpp"
#include <QApplication>
#include <QMainWindow>
#include <windows.h>

SDLWidget::SDLWidget(QWidget *parent)
{
    setParent(parent);

    // REQUIRED: force native window
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
}

SDLWidget::~SDLWidget()
{
    if (sdlTexture)  SDL_DestroyTexture(sdlTexture);
    if (sdlRenderer) SDL_DestroyRenderer(sdlRenderer);
    if (sdlWindow)   SDL_DestroyWindow(sdlWindow);

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SDLWidget::Render(const uint32_t* frameBuffer)
{
    if (!sdlRenderer || !sdlTexture)
        return;

    // Upload framebuffer
    SDL_UpdateTexture(
        sdlTexture,
        nullptr,
        frameBuffer,
        240 * sizeof(uint32_t)
    );

    // Clear + render
    SDL_RenderClear(sdlRenderer);
    SDL_RenderTexture(sdlRenderer, sdlTexture, nullptr, nullptr);
    SDL_RenderPresent(sdlRenderer);
}

void SDLWidget::showEvent(QShowEvent* event) 
{
    QWidget::showEvent(event);

    if (sdlWindow) return; // already initialized
        
    InitSDL();

    // Ensure native handle exists
    HWND hwnd = reinterpret_cast<HWND>(winId());

    SDL_PropertiesID props = SDL_CreateProperties();

    SDL_SetPointerProperty(
        props,
        SDL_PROP_WINDOW_CREATE_WIN32_HWND_POINTER,
        hwnd
    );

    SDL_SetNumberProperty(
        props,
        SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER,
        width()
    );

    SDL_SetNumberProperty(
        props,
        SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER,
        height()
    );

    // Create SDL window from Qt window
    sdlWindow = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    assert(sdlWindow);


    // Create renderer
    sdlRenderer = nullptr;
    CreateRenderer_SDL(sdlRenderer, sdlWindow, nullptr);
    assert(sdlRenderer);

    SDL_SetRenderVSync(sdlRenderer, 1);

    // Create streaming texture (GBA resolution)
    sdlTexture = SDL_CreateTexture(
        sdlRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        240,
        160
    );
    assert(sdlTexture);
}

/*
SDL3 automatically handles resizing if you render to the full window,
but it’s still good practice to notify SDL:

(later replace this with viewport logic.)
*/ 
void SDLWidget::resizeEvent(QResizeEvent* event) 
{
    QWidget::resizeEvent(event);

    if (sdlWindow)
    {
        SDL_SetWindowSize(sdlWindow, width(), height());
    }
}
