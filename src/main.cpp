#include "MainWindow.h"
#include "WeChatWindowManager.h"

#include <QApplication>
#include <QDebug>

HWND targetWindowHandle;

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
    if (hwnd == targetWindowHandle && event == EVENT_OBJECT_LOCATIONCHANGE) {
        RECT rect;
        GetWindowRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        qDebug() << "窗口位置发生变化: " << "X=" << rect.left << ", Y=" << rect.top;
        qDebug() << "窗口大小发生变化: " << "Width=" << width << ", Height=" << height;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WeChatWindowManager windowManager;
    QWindow *mainWindow = windowManager.mainWindow();
    if (!mainWindow)
        return -1;

    targetWindowHandle = reinterpret_cast<HWND>(mainWindow->winId());
    MainWindow w;
    w.createWinId();
    SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    qDebug() << mainWindow->frameGeometry();

    SetParent(reinterpret_cast<HWND>(w.winId()), targetWindowHandle);

    w.move(40, 0);
    w.resize(mainWindow->frameGeometry().size());
    w.show();

    return a.exec();
}

