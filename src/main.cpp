#include "MainWindow.h"
#include "WeChatWindowManager.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    WeChatWindowManager windowManager;
    QWindow *mainWindow = windowManager.mainWindow();
    if (!mainWindow)
        return -1;

    MainWindow w;
    w.createWinId();
    qDebug() << mainWindow->frameGeometry();
    w.windowHandle()->setParent(mainWindow);

    w.move(40, 0);
    w.resize(mainWindow->frameGeometry().size());
    w.show();

    return a.exec();
}

