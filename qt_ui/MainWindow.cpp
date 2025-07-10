#include "MainWindow.hpp"
#include "EmulatorCore.hpp"

// Qt headers
#include <QApplication>
#include <QMainWindow>
#include <QThread>

// SDL headers
#include "SDL/SDLWidget.hpp"

MainWindow::MainWindow(int width, int height, const char* windowTitle, QWidget *parent) :
    QMainWindow(parent),
    sdlWidget(new SDLWidget(this)),
    emulatorCore(new EmulatorCore()),
    emulatorThread(new QThread())
{
    setCentralWidget(sdlWidget);
    setWindowTitle(windowTitle);
    resize(width, height);
    setAttribute(Qt::WA_NativeWindow); // Ensure the window is a native window
}

MainWindow::~MainWindow()
{
    // TODO: Cleanup emulator and emulatorThread
    delete emulatorCore;
    delete emulatorThread;
}