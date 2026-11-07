#pragma once

#include <QImage>
#include <QSize>
#include <QString>
#include <memory>
#include "../core/CoreEngine.h"

namespace QuarkMeta {

struct DecodedMediaResult {
    QSize originalSize;       // 原始图像物理尺寸 (如 6000x4000)
    QImage thumbnail512;      // 历史字段名：解码出的缩略图（不要改名）
    bool isValid = false;     // 解码是否成功
};

class ImageDecoderFacade {
public:
    // 三档提取模式
    enum class ExtractMode {
        Auto,        // ① 自动提取：只用于文件夹里显示，求快不求质量，默认短超时
        Regenerate,  // ② 重新生成缩略图：同样只用于文件夹里显示，专救自动提取失败项，超时放长
        Preview      // ③ 双击预览：求高清，结果绝不能写入缩略图缓存
    };

    // Regenerate / Preview 使用的长超时（毫秒）
    static constexpr int kLongTimeoutMs = 45000;

    // 【唯一指定提图接口】单次读盘同时获取原始尺寸与缩略图
    static DecodedMediaResult decodeSinglePass(const QString& filePath, int targetSize, ExtractMode mode, std::shared_ptr<CancellationToken> token = nullptr);

    // 保留辅助接口
    static QImage loadScaledImage(const QString& filePath, int targetSize = 512, int maxAllocationMB = 128);
    static QSize readImageDimensions(const QString& filePath);
};

} // namespace QuarkMeta
