#pragma once

#include <QWidget>
#include <SDL3/SDL.h>

class SDLWidget : public QWidget
{
    Q_OBJECT

public:
    SDLWidget(QWidget* parent = nullptr);
    ~SDLWidget();

    void Render();
};