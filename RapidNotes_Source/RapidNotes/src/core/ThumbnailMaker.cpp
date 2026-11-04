#include "ThumbnailMaker.h"
#include <QPixmap>

namespace RapidNotes {

QImage ThumbnailMaker::makeThumbnail(const QImage& sourceImage, const QSize& targetSize) {
    if (sourceImage.isNull()) return QImage();
    return sourceImage.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QIcon ThumbnailMaker::makeThumbnailIcon(const QImage& sourceImage, const QSize& targetSize) {
    QImage thumb = makeThumbnail(sourceImage, targetSize);
    if (thumb.isNull()) return QIcon();
    return QIcon(QPixmap::fromImage(thumb));
}

} // namespace RapidNotes
