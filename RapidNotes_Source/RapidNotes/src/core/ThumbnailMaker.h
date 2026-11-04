#pragma once

#include <QImage>
#include <QIcon>
#include <QSize>

namespace RapidNotes {

/**
 * @brief 图片缩略图生成算法唯一真理源 (SSOT)
 * 对应 docs/SSOT接口索引.md 第 6 章。
 */
class ThumbnailMaker {
public:
    static QImage makeThumbnail(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64));
    static QIcon makeThumbnailIcon(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64));

private:
    ThumbnailMaker() = default;
};

} // namespace RapidNotes
