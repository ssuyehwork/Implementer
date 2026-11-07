#include "SvgIconRenderer.h"
#include "SvgIcons.h"
#include "../util/DiskMediaExtractor.h"
#include <QSvgRenderer>
#include <QPainter>
#include <QBuffer>
#include <QDir>
#include <QMutexLocker>

namespace QuarkMeta {

QMap<QString, QPixmap>& SvgIconRenderer::iconPixmapCache() {
    static QMap<QString, QPixmap> cache;
    return cache;
}

QMutex& SvgIconRenderer::iconMutex() {
    static QMutex mutex;
    return mutex;
}

QPixmap SvgIconRenderer::renderIcon(const QString& key, const QSize& size, const QColor& color) {
    if (!SvgIcons::icons.contains(key)) return QPixmap();
    QString svgData = SvgIcons::icons[key];
    svgData.replace("currentColor", color.name());

    std::lock_guard<std::mutex> guard(DiskMediaExtractor::s_qtGuiMutex);
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QSvgRenderer renderer(svgData.toUtf8());

    QSize defaultSz = renderer.defaultSize();
    QRectF targetRect(0, 0, size.width(), size.height());
    if (defaultSz.isValid() && !defaultSz.isEmpty()) {
        QSizeF scaled = defaultSz.scaled(size, Qt::KeepAspectRatio);
        qreal x = (size.width() - scaled.width()) / 2.0;
        qreal y = (size.height() - scaled.height()) / 2.0;
        targetRect = QRectF(x, y, scaled.width(), scaled.height());
    }

    renderer.render(&painter, targetRect);
    return pixmap;
}

QString SvgIconRenderer::getSvgDataUrl(const QString& key, const QColor& color) {
    QPixmap pix = renderIcon(key, QSize(20, 20), color);
    if (pix.isNull()) return QString();

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    pix.save(&buffer, "PNG");
    return QString("data:image/png;base64,%1").arg(QString(ba.toBase64()));
}

QIcon SvgIconRenderer::getIcon(const QString& key, const QColor& color, int size) {
    QIcon icon;
    QPixmap pix = getPixmap(key, QSize(size, size), color);
    if (!pix.isNull()) icon.addPixmap(pix);
    return icon;
}

QPixmap SvgIconRenderer::getPixmap(const QString& key, const QSize& size, const QColor& color) {
    QString cKey = QString("%1_%2_%3_%4").arg(key).arg(size.width()).arg(size.height()).arg(color.rgba());

    {
        QMutexLocker locker(&iconMutex());
        if (iconPixmapCache().contains(cKey)) return iconPixmapCache()[cKey];
    }

    QPixmap rendered = renderIcon(key, size, color);
    if (rendered.isNull()) return rendered;

    QMutexLocker locker(&iconMutex());
    iconPixmapCache().insert(cKey, rendered);
    return rendered;
}

QString SvgIconRenderer::getSvgTempFilePath(const QString& key, const QColor& color) {
    QPixmap pix = renderIcon(key, QSize(20, 20), color);
    if (pix.isNull()) return QString();

    QString tmpPath = QDir::temp().filePath(
        QString("QuarkMeta_%1_%2_v3.png").arg(key).arg(color.name().mid(1))
    );
    pix.save(tmpPath, "PNG");
    return QDir::fromNativeSeparators(tmpPath);
}


} // namespace QuarkMeta
