#pragma once

#include <QMainWindow>

class SDLWidget;
class EmulatorHandler;
class Logger;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int width, int height, const char* windowTitle, QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void SetupMenuBar();
    void SetupFileMenu(QMenuBar* mainMenuBar);
    void SetupEmulationMenu(QMenuBar* mainMenuBar);
    void SetupOptionsMenu(QMenuBar* mainMenuBar);

    SDLWidget* sdlWidget;
    EmulatorHandler* emulatorHandler;
    QThread* emulatorThread;

};