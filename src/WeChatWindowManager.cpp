#include "WeChatWindowManager.h"

#include <QDebug>
#include <QScreen>

#ifdef Q_OS_WIN
#include <TlHelp32.h>

static DWORD GetWeChatProcessId()
{
    DWORD processId = 0;

    // 创建进程快照
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32W);

        // 遍历进程快照
        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                std::wstring processName(processEntry.szExeFile);
                qDebug() << QString::fromStdWString(processName) << "Process: ";

                // 检查进程名是否为微信
                if (processName == L"WeChat.exe") {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }

        CloseHandle(snapshot);
    }

    return processId;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    WeChatWindowManager::WindowWechatProcessInfo* processInfo = reinterpret_cast<WeChatWindowManager::WindowWechatProcessInfo*>(lParam);

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId != processInfo->processId)
        return TRUE;

    wchar_t className[256];
    GetClassNameW(hwnd, className, sizeof(className) / sizeof(wchar_t));

    qDebug() << QString::fromStdWString(className);

    WeChatWindowManager::WindowWechatProcessInfo::HWNDInfo windowInfo;
    if (std::wstring(className) == L"WeChatMainWndForPC") {
        // 主窗口
        windowInfo.isMainWindow = true;
        windowInfo.hwnd = hwnd;
    } else if (std::wstring(className) == L"ChatWnd") {
        // 其他聊天窗口
        windowInfo.hwnd = hwnd;
    }

    processInfo->windowList << windowInfo;
    return TRUE;
}
#endif

WeChatWindowManager::WeChatWindowManager(QObject *parent)
    : QObject(parent)
{
}

QList<QWindow *> WeChatWindowManager::windowList() const
{
    if (!mProcessInfo) {
        const_cast<WeChatWindowManager *>(this)->update();
    }

    QList<QWindow *> windowList;
    std::transform(mProcessInfo->windowList.cbegin(), mProcessInfo->windowList.cend(),
        std::back_inserter(windowList), [](const WindowWechatProcessInfo::HWNDInfo &info) {
            return QWindow::fromWinId(reinterpret_cast<WId>(info.hwnd));
    });

    return QList<QWindow *>();
}

QWindow *WeChatWindowManager::mainWindow() const
{
    if (!mProcessInfo) {
        const_cast<WeChatWindowManager *>(this)->update();
    }

    auto it = std::find_if(mProcessInfo->windowList.cbegin(), mProcessInfo->windowList.cend(),
        [](const WindowWechatProcessInfo::HWNDInfo &info) {
            return info.isMainWindow;
    });

    if (it == mProcessInfo->windowList.cend())
        return nullptr;

    return QWindow::fromWinId(reinterpret_cast<WId>((*it).hwnd));
}

bool WeChatWindowManager::isMainWindow(QWindow *window) const
{
    if (!mProcessInfo) {
        const_cast<WeChatWindowManager *>(this)->update();
    }

    return std::any_of(mProcessInfo->windowList.cbegin(), mProcessInfo->windowList.cend(),
        [window](const WindowWechatProcessInfo::HWNDInfo &info) {
            return window->winId() == reinterpret_cast<WId>(info.hwnd) && info.isMainWindow;
    });
}

QPixmap WeChatWindowManager::toPixmap(QWindow *window) const
{
    if (!window)
        return {};

    QScreen *screen = window->screen();
    if (!screen)
        return {};

    return screen->grabWindow(window->winId(), 0, 0, window->frameGeometry().width(), window->frameGeometry().height());
}

void WeChatWindowManager::update()
{
    WindowWechatProcessInfo info = findWeChatProcessInfo();
    mProcessInfo.reset(new WindowWechatProcessInfo(info));
}

WeChatWindowManager::WindowWechatProcessInfo WeChatWindowManager::findWeChatProcessInfo()
{
    WindowWechatProcessInfo processInfo;
#ifdef Q_OS_WIN
    DWORD wechatProcessId = GetWeChatProcessId();
    qDebug() << "Process id: " << wechatProcessId;
    if (wechatProcessId == 0)
        return processInfo;
    
    processInfo.processId = wechatProcessId;

    // 遍历所有顶级窗口并查找微信窗口
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&processInfo));

    if (processInfo.windowList.isEmpty()) {
        qDebug() << "Can't find the wechat window!";
        return processInfo;
    }

#endif
    return processInfo;
}
