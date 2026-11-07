#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "EncryptionManager.h"
#include "../meta/MetadataManager.h"
#include <windows.h>
#include <bcrypt.h>
#include <vector>
#include <fstream>
#include <filesystem>
#include <QString>
#include <QThreadPool>
#include <QCoreApplication>
#include <QMetaObject>
#include <QDir>
#include <QFile>

#pragma comment(lib, "bcrypt.lib")

namespace QuarkMeta {

void EncryptionManager::encryptBatchAsync(const QStringList& targets, const std::string& password, std::function<void(bool success)> onFinished) {
    (void)QThreadPool::globalInstance()->start([targets, password, onFinished]() {
        bool allOk = true;
        for (const QString& src : targets) {
            QString dest = src + ".amenc";
            if (EncryptionManager::instance().encryptFile(src.toStdWString(), dest.toStdWString(), password)) {
                QFile::remove(src);
                MetadataManager::instance().setEncrypted(dest.toStdWString(), true);
            } else {
                allOk = false;
            }
        }
        QMetaObject::invokeMethod(QCoreApplication::instance(), [onFinished, allOk]() {
            if (onFinished) onFinished(allOk);
        });
    });
}

void EncryptionManager::decryptBatchAsync(const QStringList& targets, const std::string& password, std::function<void(bool anySuccess)> onFinished) {
    (void)QThreadPool::globalInstance()->start([targets, password, onFinished]() {
        bool anySuccess = false;
        for (const QString& src : targets) {
            QString dest = src;
            if (dest.endsWith(".amenc", Qt::CaseInsensitive)) {
                dest.chop(6);
            } else if (dest.endsWith(".decrypted", Qt::CaseInsensitive)) {
                dest.chop(10);
            }

            if (dest == src) {
                dest += ".dec";
            }

            if (EncryptionManager::instance().decryptFile(src.toStdWString(), dest.toStdWString(), password)) {
                QFile::remove(src);
                MetadataManager::instance().setEncrypted(dest.toStdWString(), false);
                anySuccess = true;
            }
        }
        QMetaObject::invokeMethod(QCoreApplication::instance(), [onFinished, anySuccess]() {
            if (onFinished) onFinished(anySuccess);
        });
    });
}

void EncryptionManager::changePasswordBatchAsync(const QStringList& targets, const std::string& oldPassword, const std::string& newPassword, std::function<void(bool anySuccess)> onFinished) {
    (void)QThreadPool::globalInstance()->start([targets, oldPassword, newPassword, onFinished]() {
        bool anySuccess = false;
        for (const QString& src : targets) {
            QString tempPlain = src + ".tmp_dec";
            if (EncryptionManager::instance().decryptFile(src.toStdWString(), tempPlain.toStdWString(), oldPassword)) {
                if (EncryptionManager::instance().encryptFile(tempPlain.toStdWString(), src.toStdWString(), newPassword)) {
                    QFile::remove(tempPlain);
                    anySuccess = true;
                } else {
                    QFile::remove(tempPlain);
                }
            }
        }
        QMetaObject::invokeMethod(QCoreApplication::instance(), [onFinished, anySuccess]() {
            if (onFinished) onFinished(anySuccess);
        });
    });
}

EncryptionManager& EncryptionManager::instance() {
    static EncryptionManager inst;
    return inst;
}

EncryptionManager::EncryptionManager() {
    BCryptOpenAlgorithmProvider(&m_aesAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
    BCryptSetProperty(m_aesAlg, BCRYPT_CHAINING_MODE, (PBYTE)BCRYPT_CHAIN_MODE_CBC, sizeof(BCRYPT_CHAIN_MODE_CBC), 0);
}

EncryptionManager::~EncryptionManager() {
    if (m_aesAlg) BCryptCloseAlgorithmProvider(m_aesAlg, 0);
}

bool EncryptionManager::deriveKey(const std::string& password, const std::vector<BYTE>& salt, std::vector<BYTE>& key) {
    BCRYPT_ALG_HANDLE hPbkdf2 = NULL;
    if (BCryptOpenAlgorithmProvider(&hPbkdf2, BCRYPT_SHA256_ALGORITHM, NULL, BCRYPT_ALG_HANDLE_HMAC_FLAG) != 0) return false;

    key.resize(32);
    NTSTATUS status = BCryptDeriveKeyPBKDF2(hPbkdf2, (PUCHAR)password.c_str(), (ULONG)password.length(),
                                           (PUCHAR)salt.data(), (ULONG)salt.size(), 10000,
                                           key.data(), (ULONG)key.size(), 0);

    BCryptCloseAlgorithmProvider(hPbkdf2, 0);
    return status == 0;
}

std::vector<BYTE> EncryptionManager::generateRandom(size_t size) {
    std::vector<BYTE> buffer(size);
    BCryptGenRandom(NULL, buffer.data(), (ULONG)size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return buffer;
}

/**
 * @brief 修复：实现基于 64KB 分块的加密逻辑，支持大文件且内存友好
 */
bool EncryptionManager::encryptFile(const std::wstring& srcPath, const std::wstring& destPath, const std::string& password) {
    std::ifstream is(srcPath, std::ios::binary);
    if (!is) return false;
    std::ofstream os(destPath, std::ios::binary);
    if (!is || !os) return false;

    std::vector<BYTE> salt = generateRandom(16);
    std::vector<BYTE> iv = generateRandom(16);
    std::vector<BYTE> key;
    if (!deriveKey(password, salt, key)) return false;

    // 写入 Salt 和原始 IV 到文件头
    os.write((char*)salt.data(), salt.size());
    os.write((char*)iv.data(), iv.size());

    BCRYPT_KEY_HANDLE hKey = NULL;
    BCryptGenerateSymmetricKey(m_aesAlg, &hKey, NULL, 0, key.data(), (ULONG)key.size(), 0);

    const size_t CHUNK_SIZE = 64 * 1024;
    std::vector<BYTE> buffer(CHUNK_SIZE);
    std::vector<BYTE> cipherBuffer(CHUNK_SIZE + 16); // 预留 Padding 空间

    while (is.read((char*)buffer.data(), CHUNK_SIZE) || is.gcount() > 0) {
        DWORD readBytes = (DWORD)is.gcount();
        DWORD cipherLen = 0;
        bool isLast = is.eof();

        // 执行加密
        BCryptEncrypt(hKey, buffer.data(), readBytes, NULL, iv.data(), (ULONG)iv.size(),
                      cipherBuffer.data(), (ULONG)cipherBuffer.size(), &cipherLen,
                      isLast ? BCRYPT_BLOCK_PADDING : 0);

        os.write((char*)cipherBuffer.data(), cipherLen);
    }

    BCryptDestroyKey(hKey);
    is.close();
    os.close();
    return true;
}

/**
 * @brief 实现分块解密还原至指定物理路径
 */
bool EncryptionManager::decryptFile(const std::wstring& amencPath, const std::wstring& destPath, const std::string& password) {
    std::ifstream is(amencPath, std::ios::binary);
    if (!is) return false;

    // 读取 Salt 和 IV
    std::vector<BYTE> salt(16);
    std::vector<BYTE> iv(16);
    is.read((char*)salt.data(), 16);
    if (is.gcount() < 16) return false;
    is.read((char*)iv.data(), 16);
    if (is.gcount() < 16) return false;

    std::vector<BYTE> key;
    if (!deriveKey(password, salt, key)) return false;

    std::ofstream os(destPath, std::ios::binary);
    if (!os) return false;

    BCRYPT_KEY_HANDLE hKey = NULL;
    if (BCryptGenerateSymmetricKey(m_aesAlg, &hKey, NULL, 0, key.data(), (ULONG)key.size(), 0) != 0) {
        return false;
    }

    const size_t CHUNK_SIZE = 64 * 1024;
    std::vector<BYTE> buffer(CHUNK_SIZE + 16);
    std::vector<BYTE> plainBuffer(CHUNK_SIZE + 16);
    bool decryptSuccess = true;

    while (is.read((char*)buffer.data(), CHUNK_SIZE) || is.gcount() > 0) {
        DWORD readBytes = (DWORD)is.gcount();
        DWORD plainLen = 0;
        bool isLast = is.eof();

        NTSTATUS status = BCryptDecrypt(hKey, buffer.data(), readBytes, NULL, iv.data(), (ULONG)iv.size(),
                                        plainBuffer.data(), (ULONG)plainBuffer.size(), &plainLen,
                                        isLast ? BCRYPT_BLOCK_PADDING : 0);
        if (status != 0) {
            decryptSuccess = false;
            break;
        }

        os.write((char*)plainBuffer.data(), plainLen);
    }

    BCryptDestroyKey(hKey);
    is.close();
    os.close();

    if (!decryptSuccess) {
        std::filesystem::remove(destPath);
        return false;
    }

    return true;
}

/**
 * @brief 修复：实现 RAII 句柄持有逻辑，防止临时文件在 CloseHandle 前被删除
 */
/**
 * @brief 实现分块解密逻辑，恢复原始文件到临时路径
 */
std::shared_ptr<DecryptedFileHandle> EncryptionManager::decryptToTemp(const std::wstring& amencPath, const std::string& password) {
    std::ifstream is(QString::fromStdWString(amencPath).toStdString(), std::ios::binary);
    if (!is) return nullptr;

    // 读取 Salt 和 IV
    std::vector<BYTE> salt(16);
    std::vector<BYTE> iv(16);
    is.read((char*)salt.data(), 16);
    is.read((char*)iv.data(), 16);

    std::vector<BYTE> key;
    if (!deriveKey(password, salt, key)) return nullptr;

    wchar_t tempPath[MAX_PATH];
    GetTempPathW(MAX_PATH, tempPath);
    std::wstring amTempDir = std::wstring(tempPath) + L"amtemp\\";
    CreateDirectoryW(amTempDir.c_str(), NULL);

    std::wstring outPath = amTempDir + std::filesystem::path(amencPath).stem().wstring();
    HANDLE hFile = CreateFileW(outPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);

    if (hFile == INVALID_HANDLE_VALUE) return nullptr;

    BCRYPT_KEY_HANDLE hKey = NULL;
    BCryptGenerateSymmetricKey(m_aesAlg, &hKey, NULL, 0, key.data(), (ULONG)key.size(), 0);

    const size_t CHUNK_SIZE = 64 * 1024;
    std::vector<BYTE> buffer(CHUNK_SIZE + 16);
    std::vector<BYTE> plainBuffer(CHUNK_SIZE + 16);

    while (is.read((char*)buffer.data(), CHUNK_SIZE) || is.gcount() > 0) {
        DWORD readBytes = (DWORD)is.gcount();
        DWORD plainLen = 0;
        bool isLast = is.eof();

        BCryptDecrypt(hKey, buffer.data(), readBytes, NULL, iv.data(), (ULONG)iv.size(),
                      plainBuffer.data(), (ULONG)plainBuffer.size(), &plainLen,
                      isLast ? BCRYPT_BLOCK_PADDING : 0);

        DWORD written = 0;
        WriteFile(hFile, plainBuffer.data(), plainLen, &written, NULL);
    }

    BCryptDestroyKey(hKey);
    is.close();

    // 关键：将文件指针移回开头，方便后续进程读取
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

    return std::make_shared<DecryptedFileHandle>(hFile, outPath);
}

} // namespace QuarkMeta
