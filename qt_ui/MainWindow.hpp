#pragma once

#include <QMainWindow>

class SDLWidget;
class EmulatorCore;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int width, int height, const char* windowTitle, QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    SDLWidget* sdlWidget;
    EmulatorCore* emulatorCore;
    QThread* emulatorThread;

};