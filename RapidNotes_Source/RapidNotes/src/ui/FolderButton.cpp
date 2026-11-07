#include "FolderButton.h"
#include <QPainter>
#include <QFileInfo>
#include <QDir>
#include "UiHelper.h"
#include "ToolTipOverlay.h"
#include "../core/AppConfig.h"
#include "StyleLibrary.h"

namespace QuarkMeta {

FolderButton::FolderButton(const QString& folderPath, QWidget* parent)
    : QPushButton(parent), m_folderPath(folderPath) {
    setFixedSize(28, 28);
    setCursor(Qt::PointingHandCursor);
    m_folderName = QFileInfo(folderPath).fileName();
    if (m_folderName.isEmpty()) {
        m_folderName = folderPath;
    }
}

void FolderButton::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect r = rect().adjusted(1, 1, -1, -1);
    QColor bgColor;
    QColor borderColor;

    // Hover 样式对齐 5.4 标题栏按钮规范，且拒绝使用 rgba 蒙版
    if (underMouse()) {
        if (isDown()) {
            bgColor = QColor("#4E4E52"); // Style::PressedBackground
        } else {
            bgColor = QColor("#3E3E42"); // Style::HoverBackground
        }
        borderColor = QColor("#555555");
    } else {
        bgColor = QColor("#333333");
        borderColor = QColor("#444444");
    }

    painter.setPen(QPen(borderColor, 1));
    painter.setBrush(bgColor);
    painter.drawRoundedRect(r, 4, 4);

    // 从 AppConfig 读取自定义图标和颜色，完美支持个性化渲染
    QString colorKey = QString("DriveBar/FolderColor_%1").arg(m_folderPath);
    QString iconKeyPath = QString("DriveBar/FolderIcon_%1").arg(m_folderPath);

    QString colorStr = AppConfig::instance().getValue(colorKey, "#FFFFFF").toString();
    QString iconKey = AppConfig::instance().getValue(iconKeyPath, "folder_filled").toString();

    QColor folderColor = QColor(colorStr);
    if (!folderColor.isValid()) {
        folderColor = Style::TextMain;
    }

    // 绘制自定义矢量图标 (SvgIcons)
    QPixmap pix = UiHelper::getPixmap(iconKey, QSize(16, 16), folderColor);
    QRect iconRect(r.left() + (r.width() - 16) / 2, r.top() + (r.height() - 16) / 2, 16, 16);
    painter.drawPixmap(iconRect, pix);
}

void FolderButton::enterEvent(QEnterEvent* event) {
    Q_UNUSED(event);
    // 悬停时通过全局 ToolTipOverlay 渲染文件夹名称与绝对路径，避免使用原生 ToolTip
    QString tipText = QString("<b>%1</b><br><span style='color:#888888;'>%2</span>")
        .arg(m_folderName)
        .arg(QDir::toNativeSeparators(m_folderPath));
    ToolTipOverlay::instance()->showText(mapToGlobal(QPoint(0, height() + 4)), tipText, 0);
}

void FolderButton::leaveEvent(QEvent* event) {
    Q_UNUSED(event);
    ToolTipOverlay::hideTip();
}

} // namespace QuarkMeta
