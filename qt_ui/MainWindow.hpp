#pragma once

#include <QMainWindow>

class SDLWidget;
class EmulatorHandler;
class Logger;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int width, int height, QWidget* parent = nullptr);
    ~MainWindow() override;

signals:
    void RequestLoadROM(const QString& path);

private:
    void SetupMenuBar();
    void SetupFileMenu(QMenuBar* mainMenuBar);
    void SetupEmulationMenu(QMenuBar* mainMenuBar);
    void SetupOptionsMenu(QMenuBar* mainMenuBar);

    void PostStatusMessage(const QString& message, int seconds);
    void PostErrorMessage(const QString& errorMessage);
    void OnROMLoaded(const QString& romName);

    SDLWidget* sdlWidget;
    EmulatorHandler* emulatorHandler;
    QThread* emulatorThread;

    const QString BASE_WINDOW_TITLE = "VerdalGBA";

};