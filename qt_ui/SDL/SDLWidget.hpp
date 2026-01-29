#pragma once

#include <QWidget>
#include <SDL3/SDL.h>

class SDLWidget : public QWidget
{
    Q_OBJECT

public:
    SDLWidget(QWidget* parent = nullptr);
    ~SDLWidget();

    void Render(const uint32_t* frameBuffer);

    
protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    SDL_Window* sdlWindow = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
    SDL_Texture* sdlTexture = nullptr;
};