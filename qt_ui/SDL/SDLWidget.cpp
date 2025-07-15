#include "SDLWidget.hpp"
#include <QApplication>
#include <QMainWindow>

SDLWidget::SDLWidget(QWidget *parent)
{
    setParent(parent);
    //setAttribute(QT::WA_NativeWindow);
}

SDLWidget::~SDLWidget()
{

}

void SDLWidget::Render()
{

}
