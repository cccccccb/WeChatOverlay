#ifndef WECHATWINDOWMANAGER_H
#define WECHATWINDOWMANAGER_H

#include <QObject>
#include <QWindow>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

struct WindowWechatProcessInfo;

class WeChatWindowManager : public QObject
{
    Q_OBJECT
public:
    explicit WeChatWindowManager(QObject *parent = nullptr);

    QList<QWindow *> windowList() const;
    QWindow *mainWindow() const;
    
    bool isMainWindow(QWindow *window) const;
    QPixmap toPixmap(QWindow *window) const;

    void update();

private:
    friend BOOL EnumWindowsProc(HWND hwnd, LPARAM lParam);
    struct WindowWechatProcessInfo {
    #ifdef Q_OS_WIN
        DWORD processId = 0;
        struct HWNDInfo
        {
            HWND hwnd = nullptr;
            BOOLEAN isMainWindow = FALSE;
        };
        
        QList<HWNDInfo> windowList;
    #endif
    };
    static WindowWechatProcessInfo findWeChatProcessInfo();
    
private:
    QScopedPointer<WindowWechatProcessInfo> mProcessInfo;
};

#endif // WECHATWINDOWMANAGER_H
