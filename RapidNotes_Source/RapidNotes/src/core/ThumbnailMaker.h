#pragma once

#include <QImage>
#include <QIcon>
#include <QSize>

namespace RapidNotes {

class ThumbnailMaker {
public:
    static QImage makeThumbnail(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64));
    static QIcon makeThumbnailIcon(const QImage& sourceImage, const QSize& targetSize = QSize(64, 64));

private:
    ThumbnailMaker() = default;
};

} // namespace RapidNotes
