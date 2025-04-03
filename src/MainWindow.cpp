#include "MainWindow.h"

#include <QPainter>
#include <QWindow>
#include <Windows.h>


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(true);
}

MainWindow::~MainWindow()
{
}

void MainWindow::paintEvent(QPaintEvent *paintEvent)
{
    QPainter painter(this);
    painter.setOpacity(0.4);
    painter.fillRect(this->rect(), Qt::blue);
}

void MainWindow::showEvent(QShowEvent *event)
{
    auto handle = this->windowHandle();
    if (!handle) {
        this->createWinId();
        handle = this->windowHandle();
    }

    auto hwnd = HWND(handle->winId());
    auto style = GetWindowLongPtr(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) == 0) {
        style &= ~WS_POPUP;
        style |= WS_CHILD;
        SetWindowLongPtr(hwnd, GWL_STYLE, style);
    }

    return QWidget::showEvent(event);
}
