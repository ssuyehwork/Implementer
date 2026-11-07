#ifndef NOMINMAX
#define NOMINMAX
#endif
#pragma once

#include <windows.h>
#include <bcrypt.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <QStringList>

namespace QuarkMeta {

/**
 * @brief 临时解密文件句柄持有者 (RAII)
 * 利用 FILE_FLAG_DELETE_ON_CLOSE，当对象析构句柄关闭时，系统自动删除临时文件
 */
class DecryptedFileHandle {
public:
    DecryptedFileHandle(HANDLE hFile, const std::wstring& path)
        : m_hFile(hFile), m_path(path) {}
    ~DecryptedFileHandle() {
        if (m_hFile != INVALID_HANDLE_VALUE) CloseHandle(m_hFile);
    }
    std::wstring path() const { return m_path; }
    bool isValid() const { return m_hFile != INVALID_HANDLE_VALUE; }

private:
    HANDLE m_hFile;
    std::wstring m_path;
};

/**
 * @brief 加密管理器
 */
class EncryptionManager {
public:
    static EncryptionManager& instance();

    /**
     * @brief 分块加密文件 (支持大文件)
     */
    bool encryptFile(const std::wstring& srcPath, const std::wstring& destPath, const std::string& password);

    /**
     * @brief 解密文件并保存至指定物理路径
     */
    bool decryptFile(const std::wstring& amencPath, const std::wstring& destPath, const std::string& password);

    /**
     * @brief 异步批量加密文件
     */
    void encryptBatchAsync(const QStringList& targets, const std::string& password, std::function<void(bool success)> onFinished);

    /**
     * @brief 异步批量解密文件
     */
    void decryptBatchAsync(const QStringList& targets, const std::string& password, std::function<void(bool anySuccess)> onFinished);

    /**
     * @brief 异步批量修改密码
     */
    void changePasswordBatchAsync(const QStringList& targets, const std::string& oldPassword, const std::string& newPassword, std::function<void(bool anySuccess)> onFinished);

    /**
     * @brief 解密文件并持有句柄 (RAII)
     */
    std::shared_ptr<DecryptedFileHandle> decryptToTemp(const std::wstring& amencPath, const std::string& password);

private:
    EncryptionManager();
    ~EncryptionManager();

    bool deriveKey(const std::string& password, const std::vector<BYTE>& salt, std::vector<BYTE>& key);
    std::vector<BYTE> generateRandom(size_t size);

    BCRYPT_ALG_HANDLE m_aesAlg = NULL;
};

} // namespace QuarkMeta
