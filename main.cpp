#include <iostream>

// This is the main entry point for the application
// It initializes SDL and creates the main window and renderer.
#include "qt_ui/MainWindow.hpp" // Include the Qt UI header if needed
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>

int main(int argc, char* argv[])
{   
    printf("Welcome to the GBA Emulator From Main!\n");

    QApplication app(argc, argv);

    // Main Window Setup
    MainWindow mainWindow = MainWindow(800, 600, "Diego is Stupid");
    mainWindow.show();

    // Run the application event loop - this will block the thread until the application is closed
    return app.exec();

    // QApplication app(argc, argv);

    // QMainWindow mainWindow;
    // mainWindow.setWindowTitle("Test Menu and Status");
    // mainWindow.resize(800, 600);

    // QMenuBar *bar = mainWindow.menuBar();
    // QMenu *file = bar->addMenu("&File");
    // file->addAction("Exit", &mainWindow, &QMainWindow::close);

    // mainWindow.statusBar()->showMessage("Status bar visible");

    // mainWindow.show();
    // return app.exec();
}
