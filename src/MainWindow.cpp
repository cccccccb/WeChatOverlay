#include "MainWindow.h"

#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    // setAttribute(Qt::WA_TranslucentBackground);
    // setWindowFlag(Qt::FramelessWindowHint);
}

MainWindow::~MainWindow()
{
}

void MainWindow::paintEvent(QPaintEvent *paintEvent)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), Qt::white);
}
