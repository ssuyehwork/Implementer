#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "WindowsShellThumbnailProvider.h"
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QFileIconProvider>
#include <QMutexLocker>
#include "SvgIconRenderer.h"
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#include <objbase.h>
#include <shlobj.h>
#ifdef __MINGW32__
#include <shlwapi.h>
#else
#include <shobjidl_core.h>
#include <thumbcache.h>
#endif
#endif

namespace QuarkMeta {

WindowsShellThumbnailProvider& WindowsShellThumbnailProvider::instance() {
    static WindowsShellThumbnailProvider inst;
    return inst;
}

WindowsShellThumbnailProvider::WindowsShellThumbnailProvider() {
    connect(this, &WindowsShellThumbnailProvider::requestIconLoad,
            this, &WindowsShellThumbnailProvider::handleIconLoad,
            Qt::QueuedConnection);
}

QMutex& WindowsShellThumbnailProvider::fileIconMutex() {
    static QMutex mutex;
    return mutex;
}

QMap<QString, QIcon>& WindowsShellThumbnailProvider::fileIconCache() {
    static QMap<QString, QIcon> cache;
    return cache;
}

QMutex& WindowsShellThumbnailProvider::loadingMutex() {
    static QMutex mutex;
    return mutex;
}

QSet<QString>& WindowsShellThumbnailProvider::loadingKeys() {
    static QSet<QString> keys;
    return keys;
}

bool WindowsShellThumbnailProvider::isDriveRootPath(const QString& filePath) {
    return filePath.endsWith(":\\") || filePath.endsWith(":/") || filePath.length() <= 3;
}

// 缓存键唯一计算入口
QString WindowsShellThumbnailProvider::iconKey(const QString& filePath, bool isDir, const QString& suffix) {
    QString key;
    if (isDir) {
        key = isDriveRootPath(filePath) ? filePath : QStringLiteral("folder");
    } else {
        key = suffix.toLower();
    }
    if (key.length() > 128) key = QStringLiteral("unknown");
    return key;
}

// getFileIcon 只是 getFileIconFast 的薄封装，禁止再复制一份实现
QIcon WindowsShellThumbnailProvider::getFileIcon(const QString& filePath, int size) {
    Q_UNUSED(size);
    QFileInfo info(filePath);
    return getFileIconFast(filePath, info.isDir(), info.suffix());
}

bool WindowsShellThumbnailProvider::isIconCached(const QString& filePath, bool isDir, const QString& suffix) {
    const QString key = iconKey(filePath, isDir, suffix);
    QMutexLocker locker(&fileIconMutex());
    return fileIconCache().contains(key);
}

QIcon WindowsShellThumbnailProvider::getFileIconFast(const QString& filePath, bool isDir, const QString& suffix) {
    const QString key = iconKey(filePath, isDir, suffix);
    const bool isRoot = isDir && isDriveRootPath(filePath);

    {
        QMutexLocker locker(&fileIconMutex());
        if (fileIconCache().contains(key)) {
            return fileIconCache()[key];
        }
    }

    static QIcon s_defaultFileIcon;
    static QIcon s_defaultFolderIcon;
    if (s_defaultFileIcon.isNull() || s_defaultFolderIcon.isNull()) {
        QFileIconProvider provider;
        s_defaultFolderIcon = SvgIconRenderer::getIcon("folder_filled", QColor("#888888"), 128);
        s_defaultFileIcon = provider.icon(QFileIconProvider::File);
    }
    QIcon placeholderIcon = isDir ? s_defaultFolderIcon : s_defaultFileIcon;

    {
        QMutexLocker lock(&loadingMutex());
        if (loadingKeys().contains(key)) {
            return placeholderIcon;
        }
        loadingKeys().insert(key);
    }

    emit instance().requestIconLoad(filePath, key, isDir, isRoot);

    return placeholderIcon;
}

namespace {
    std::atomic<int> g_consecutiveFailures{0};
    std::atomic<int64_t> g_cooldownUntilMs{0};
}

bool WindowsShellThumbnailProvider::checkCircuitBreaker() {
    int64_t nowMs = QDateTime::currentMSecsSinceEpoch();
    if (nowMs < g_cooldownUntilMs.load()) {
        return false; // 处于熔断冷却期，阻断 Shell 调用
    }
    return true;
}

void WindowsShellThumbnailProvider::recordShellFailure() {
    int fails = ++g_consecutiveFailures;
    if (fails >= 5) {
        int64_t nowMs = QDateTime::currentMSecsSinceEpoch();
        g_cooldownUntilMs.store(nowMs + 1000); // 触发 1000ms 熔断冷却，允许 Shell 缓存池 GC
        g_consecutiveFailures.store(0);
    }
}

void WindowsShellThumbnailProvider::recordShellSuccess() {
    g_consecutiveFailures.store(0);
}

void WindowsShellThumbnailProvider::handleIconLoad(const QString& filePath, const QString& key, bool isDir, bool isRoot) {
    // 在主线程（本槽所在线程）取得通知器指针，保证通知器归属主线程，
    // 避免首次在子线程创建导致 queued 调用永远不执行、通知发不出。
    IconLoadNotifier* notifier = &IconLoadNotifier::instance();

    (void)QtConcurrent::run([filePath, key, isDir, isRoot, notifier]() {
        // 在子线程中执行 COM 线程环境初始化
#ifdef Q_OS_WIN
        HRESULT hres = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
#endif
        QFileIconProvider provider;
        QIcon icon;
        QFileInfo info(filePath);

        try {
            if (isDir) {
                if (isRoot) {
                    icon = provider.icon(info);
                } else {
                    icon = SvgIconRenderer::getIcon("folder_filled", QColor("#888888"), 128);
                }
            } else {
                if (!checkCircuitBreaker()) {
                    icon = provider.icon(QFileIconProvider::File);
                } else {
                    icon = provider.icon(QFileInfo("dummy." + key));
                    if (icon.isNull()) {
                        icon = provider.icon(QFileIconProvider::File);
                    }
                    recordShellSuccess();
                }
            }
        } catch (...) {
            recordShellFailure();
            icon = SvgIconRenderer::getIcon(isDir ? "folder_filled" : "file", QColor("#888888"), 128);
        }

        {
            QMutexLocker locker(&fileIconMutex());
            fileIconCache()[key] = icon;
        }

        {
            QMutexLocker lock(&loadingMutex());
            loadingKeys().remove(key);
        }

#ifdef Q_OS_WIN
        if (SUCCEEDED(hres)) {
            CoUninitialize();
        }
#endif

        // 通知主线程：该 key 对应的系统图标已就绪，视图/模型据此重绘
        QMetaObject::invokeMethod(notifier, [notifier, key]() {
            emit notifier->iconLoadedForKey(key);
            emit notifier->iconLoaded();
        }, Qt::QueuedConnection);
    });
}

struct ComInitializer {
    HRESULT hr;
    ComInitializer() {
#ifdef Q_OS_WIN
        hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
#else
        hr = S_OK;
#endif
    }
    ~ComInitializer() {
#ifdef Q_OS_WIN
        if (SUCCEEDED(hr)) {
            CoUninitialize();
        }
#endif
    }
};

QImage WindowsShellThumbnailProvider::getShellThumbnail(const QString& path, int size) {
#ifdef Q_OS_WIN
    if (!checkCircuitBreaker()) {
        return QImage();
    }

    try {
        ComInitializer comInit;
        PIDLIST_ABSOLUTE pidl = nullptr;
        HRESULT hr = SHParseDisplayName(path.toStdWString().c_str(), nullptr, &pidl, 0, nullptr);
        if (FAILED(hr)) return QImage();

        IShellItem* pItem = nullptr;
        hr = SHCreateItemFromIDList(pidl, IID_IShellItem, (void**)&pItem);
        ILFree(pidl);

        if (SUCCEEDED(hr) && pItem) {
            IShellItemImageFactory* pFactory = nullptr;
            hr = pItem->QueryInterface(IID_IShellItemImageFactory, (void**)&pFactory);
            if (SUCCEEDED(hr) && pFactory) {
                SIZE nativeSize = { size, size };
                HBITMAP hBitmap = nullptr;

                // 🚨 关键防线：包裹 try-catch (...) 拦截 Windows Shell 底层 CacheOverflowException 异常
                try {
                    hr = pFactory->GetImage(nativeSize, SIIGBF_THUMBNAILONLY, &hBitmap);
                } catch (...) {
                    hr = E_FAIL;
                    recordShellFailure();
                }

                if (SUCCEEDED(hr) && hBitmap) {
                    BITMAP bmpInfo;
                    GetObject(hBitmap, sizeof(bmpInfo), &bmpInfo);
                    int w = bmpInfo.bmWidth;
                    int h = std::abs(bmpInfo.bmHeight);

                    if (w > 0 && h > 0) {
                        BITMAPINFOHEADER bi = {};
                        bi.biSize        = sizeof(BITMAPINFOHEADER);
                        bi.biWidth       = w;
                        bi.biHeight      = -h;   // 负值 = top-down，方向永远正确
                        bi.biPlanes      = 1;
                        bi.biBitCount    = 32;
                        bi.biCompression = BI_RGB;

                        QByteArray pixels(w * h * 4, 0);
                        HDC hdc = GetDC(nullptr);
                        if (hdc) {
                            GetDIBits(hdc, hBitmap, 0, h, pixels.data(),
                                      reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);
                            ReleaseDC(nullptr, hdc);
                        }

                        // Windows 返回 BGRA，Qt 需要 RGBA，交换 R/B 通道
                        uint8_t* p = reinterpret_cast<uint8_t*>(pixels.data());
                        for (int i = 0; i < w * h; ++i) {
                            std::swap(p[i * 4 + 0], p[i * 4 + 2]);
                        }

                        QImage img(p, w, h, w * 4, QImage::Format_RGBA8888);
                        img = img.copy(); // 确保数据所有权

                        DeleteObject(hBitmap);
                        pFactory->Release();
                        pItem->Release();
                        recordShellSuccess();
                        return img;
                    }
                    DeleteObject(hBitmap);
                }
                pFactory->Release();
            }
            pItem->Release();
        }
    } catch (...) {
        recordShellFailure();
    }
#else
    Q_UNUSED(path); Q_UNUSED(size);
#endif
    return QImage();
}

} // namespace QuarkMeta