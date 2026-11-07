#pragma once
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRandomGenerator>
#ifdef Q_OS_WIN
#include <windows.h>
#include <io.h>
#endif

namespace QuarkMeta {
class SecureFileEraser {
public:
    static bool shredFile(const QString& path) {
        // 安全擦除逻辑：递归覆写与完整清理
        QFileInfo info(path);
        if (info.isDir()) {
            QDir dir(path);
            // 关键修正：必须加上 QDir::Hidden | QDir::System，防止隐藏/系统文件遗留导致 rmdir 失败
            auto entries = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
            for (const QString& entry : entries) {
                shredFile(dir.filePath(entry));
            }
#ifdef Q_OS_WIN
            // 剥离 Windows 目录可能的只读/系统属性
            SetFileAttributesW(path.toStdWString().c_str(), FILE_ATTRIBUTE_NORMAL);
#endif
            return dir.rmdir(dir.absolutePath());
        } else {
#ifdef Q_OS_WIN
            // 剥离 Windows 文件可能的只读属性
            SetFileAttributesW(path.toStdWString().c_str(), FILE_ATTRIBUTE_NORMAL);
#endif
            QFile file(path);
            if (file.open(QIODevice::ReadWrite)) {
                qint64 size = file.size();
                if (size > 0) {
                    QByteArray buffer(65536, 0);
                    for (int pass = 0; pass < 3; ++pass) { // 3 遍随机覆写
                        file.seek(0);
                        qint64 written = 0;
                        while (written < size) {
                            for (int i = 0; i < buffer.size(); ++i) {
                                buffer[i] = (char)QRandomGenerator::global()->bounded(256);
                            }
                            qint64 toWrite = qMin((qint64)buffer.size(), size - written);
                            file.write(buffer.data(), toWrite);
                            written += toWrite;
                        }
                        file.flush();
#ifdef Q_OS_WIN
                        HANDLE hFile = (HANDLE)_get_osfhandle(file.handle());
                        if (hFile != INVALID_HANDLE_VALUE) {
                            FlushFileBuffers(hFile); // 强制刷新扇区落盘
                        }
#endif
                    }
                }
                file.close();
            }
            return QFile::remove(path);
        }
    }
};
}
