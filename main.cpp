#include <iostream>

// This is the main entry point for the application
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
    MainWindow mainWindow = MainWindow(800, 600);
    mainWindow.show();

    // Run the application event loop - this will block the thread until the application is closed
    return app.exec();
}
