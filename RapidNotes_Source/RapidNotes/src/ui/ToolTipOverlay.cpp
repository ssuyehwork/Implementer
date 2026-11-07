#include "ToolTipOverlay.h"
#include "UiHelper.h"

#include <QTimer>

namespace QuarkMeta {

ToolTipOverlay::ToolTipOverlay() : QWidget(nullptr) {
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint |
                  Qt::WindowTransparentForInput | Qt::NoDropShadowWindowHint |
                  Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnTopHint);
    setObjectName("ToolTipOverlay");

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    m_doc.setUndoRedoEnabled(false);
    // [ULTIMATE FIX] 强制锁定调色板颜色
    QPalette pal = palette();
    pal.setColor(QPalette::WindowText, QColor("#EEEEEE"));
    pal.setColor(QPalette::Text, QColor("#EEEEEE"));
    pal.setColor(QPalette::ButtonText, QColor("#EEEEEE"));
    setPalette(pal);

    m_doc.setDefaultStyleSheet("body, div, p, span, b, i { color: #EEEEEE !important; font-family: 'Microsoft YaHei', 'Segoe UI'; }");

    QFont f = font();
    f.setPointSize(9);
    m_doc.setDefaultFont(f);

    m_hideTimer.setSingleShot(true);
    connect(&m_hideTimer, &QTimer::timeout, this, &ToolTipOverlay::hideOverlay);

    m_showDelayTimer.setSingleShot(true);
    connect(&m_showDelayTimer, &QTimer::timeout, this, &ToolTipOverlay::triggerPendingShow);

    hide();
}

void ToolTipOverlay::silentWarmup() {
    if (m_warmedUp) return;
    m_warmedUp = true;

    // 1. 强制向操作系统申请底层真正的 Win32 窗口句柄 (HWND)
    (void)this->winId();

    // 2. 绝对不可见双重保护：移至屏幕外万象之外 + 全透明
    setWindowOpacity(0.0);
    move(-9999, -9999);
    resize(40, 24);

    // 3. 预先激活 QTextDocument 字体解析引擎与字形缓存
    m_doc.setHtml(
        "<html><body style='margin:0; padding:0; color:#EEEEEE; font-family:\"Microsoft YaHei\",\"Segoe UI\",sans-serif;'>"
        "<div style='color:#EEEEEE;'>Warmup</div></body></html>"
    );
    m_doc.setTextWidth(-1);
    (void)m_doc.idealWidth();
    (void)m_doc.size();

    // 4. 驱动 DWM 在显存中分配透明混合图层并完成首次 GPU 渲染指令预编译
    show();
    hide();

    // 5. 静默复位，随时就绪
    setWindowOpacity(1.0);
    m_doc.clear();
}

void ToolTipOverlay::hideOverlay() {
    m_showDelayTimer.stop();
    m_hideTimer.stop();
    hide();
}

void ToolTipOverlay::showColorToast(const QString& colorHex, QWidget* targetWidget) {
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this, colorHex, targetWidget]() {
            showColorToast(colorHex, targetWidget);
        });
        return;
    }

    QPixmap colorPix(16, 16);
    colorPix.fill(Qt::transparent);

    if (colorHex.isEmpty() || colorHex == "transparent" || colorHex == "none") {
        colorPix = UiHelper::getPixmap("no_color", QSize(16, 16), QColor("#AAAAAA"));
    } else {
        QPainter painter(&colorPix);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        QColor drawColor(colorHex);
        if (!drawColor.isValid()) {
            drawColor = QColor("#2ecc71");
        }
        painter.setBrush(drawColor);
        painter.drawEllipse(0, 0, 16, 16);
    }

    m_doc.addResource(QTextDocument::ImageResource, QUrl("color_dot.png"), colorPix);

    QString html = "<div style='line-height:16px; text-align:center;'>"
                   "<img src='color_dot.png' style='vertical-align:middle;' />"
                   "</div>";

    showToast(html, 1500, QColor("#444444"), targetWidget);
}

void ToolTipOverlay::showRatingToast(int rating, QWidget* targetWidget) {
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this, rating, targetWidget]() {
            showRatingToast(rating, targetWidget);
        });
        return;
    }

    QPixmap starOn = UiHelper::getPixmap("star_filled", QSize(14, 14), QColor("#AAAAAA"));
    QPixmap starOff = UiHelper::getPixmap("star_filled", QSize(14, 14), QColor("#444444"));

    m_doc.addResource(QTextDocument::ImageResource, QUrl("star_on.png"), starOn);
    m_doc.addResource(QTextDocument::ImageResource, QUrl("star_off.png"), starOff);

    QString html = "<div style='line-height:14px;'>";
    if (rating <= 0) {
        html += "<span style='color:#AAAAAA; font-size:12px;'>已清除星级</span>";
    } else {
        for (int i = 1; i <= 5; ++i) {
            if (i <= rating) {
                html += "<img src='star_on.png' style='vertical-align:middle; margin-right:2px;' />";
            } else {
                html += "<img src='star_off.png' style='vertical-align:middle; margin-right:2px;' />";
            }
        }
    }
    html += "</div>";

    showToast(html, 1500, QColor("#444444"), targetWidget);
}

void ToolTipOverlay::showToast(const QString& text, int timeout, const QColor& borderColor, QWidget* targetWidget) {
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this, text, timeout, borderColor, targetWidget]() {
            showToast(text, timeout, borderColor, targetWidget);
        });
        return;
    }

    QWidget* anchor = targetWidget;
    if (!anchor) {
        QWidget* activeWin = QApplication::activeWindow();
        if (activeWin) {
            QList<QWidget*> allWidgets = activeWin->findChildren<QWidget*>();
            QWidget* fallbackPanel = nullptr;
            for (QWidget* w : allWidgets) {
                if (w->inherits("QuarkMeta::ContentPanel") || w->inherits("ContentPanel") || w->objectName() == "EditorContainer") {
                    if (w->property("activePane").toString() == "true") {
                        anchor = w;
                        break;
                    }
                    if (!fallbackPanel) {
                        fallbackPanel = w;
                    }
                }
            }
            if (!anchor) {
                anchor = fallbackPanel;
            }
            if (!anchor) {
                anchor = activeWin->findChild<QWidget*>("ContentPanel");
            }
        }
    }

    QPoint targetPos;
    if (anchor && anchor->isVisible()) {
        QPoint globalTopLeft = anchor->mapToGlobal(QPoint(0, 0));
        int panelWidth = anchor->width();

        m_pendingText = text;
        triggerPendingShow();
        int tipWidth = width();

        int centerX = globalTopLeft.x() + (panelWidth - tipWidth) / 2;
        int topY = globalTopLeft.y() + 10;

        QWidget* header = anchor->findChild<QWidget*>("ContentHeaderWidget");
        if (header && header->isVisible()) {
            topY = header->mapToGlobal(QPoint(0, header->height())).y() + 10;
        }

        targetPos = QPoint(centerX, topY);
    } else {
        // 彻底禁止跟随鼠标 QCursor::pos()，自动回退到当前活动窗口或主屏幕顶部水平居中固定位置
        m_pendingText = text;
        triggerPendingShow();
        int tipWidth = width();

        QWidget* activeWin = QApplication::activeWindow();
        if (activeWin && activeWin->isVisible()) {
            QPoint winTopLeft = activeWin->mapToGlobal(QPoint(0, 0));
            targetPos = QPoint(winTopLeft.x() + (activeWin->width() - tipWidth) / 2, winTopLeft.y() + 10);
        } else {
            QScreen* screen = QGuiApplication::primaryScreen();
            QRect geom = screen ? screen->availableGeometry() : QRect(0, 0, 1920, 1080);
            targetPos = QPoint(geom.x() + (geom.width() - tipWidth) / 2, geom.y() + 50);
        }
    }

    showText(targetPos, text, timeout, borderColor, true);
}

void ToolTipOverlay::showText(const QPoint& globalPos, const QString& text, int timeout, const QColor& borderColor, bool exactPosition, const QColor& backgroundColor) {
    if (thread() != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this, globalPos, text, timeout, borderColor, exactPosition, backgroundColor]() {
            showText(globalPos, text, timeout, borderColor, exactPosition, backgroundColor);
        });
        return;
    }

    if (text.isEmpty() && !exactPosition) {
        hideOverlay();
        return;
    }

    // 若已经处于显示状态，且文本/边框无变化，仅当位置发生显著变化（>5px）时更新，避免无谓的重绘与抖动
    if (isVisible() && m_pendingText == text && m_currentBorderColor == borderColor && !exactPosition) {
        if ((globalPos - m_lastPos).manhattanLength() < 5) {
            return;
        }
        m_lastPos = globalPos;
        m_showDelayTimer.stop();
        QPoint pos = globalPos + QPoint(15, 15);
        QScreen* screen = QGuiApplication::screenAt(globalPos);
        if (!screen) screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenGeom = screen->geometry();
            if (pos.x() + width() > screenGeom.right()) {
                pos.setX(globalPos.x() - width() - 15);
            }
            if (pos.y() + height() > screenGeom.bottom()) {
                pos.setY(globalPos.y() - height() - 15);
            }
        }
        move(pos);
        return;
    }

    m_lastPos = globalPos;
    m_pendingPos = globalPos;
    m_pendingText = text;
    m_pendingTimeout = timeout;
    m_pendingBorderColor = borderColor;
    m_pendingBackgroundColor = backgroundColor;
    m_pendingExactPosition = exactPosition;

    m_showDelayTimer.start(150);
}

void ToolTipOverlay::triggerPendingShow() {
    m_currentBorderColor = m_pendingBorderColor;
    m_currentBackgroundColor = m_pendingBackgroundColor;

    int timeout = m_pendingTimeout;
    if (timeout > 0) {
        timeout = qBound(500, timeout, 60000);
    }

    int w = 40;
    int h = 24;

    if (m_pendingText.isEmpty()) {
        m_text = "";
        m_doc.clear();
        w = 60;
        h = 24;
    } else {
        QString htmlBody;
        if (m_pendingText.contains("<") && m_pendingText.contains(">")) {
            htmlBody = m_pendingText;
        } else {
            htmlBody = m_pendingText.toHtmlEscaped().replace("\n", "<br>");
        }

        m_text = QString(
            "<html><head><style>div, p, span, body { color: #EEEEEE !important; }</style></head>"
            "<body style='margin:0; padding:0; color:#EEEEEE; font-family:\"Microsoft YaHei\",\"Segoe UI\",sans-serif;'>"
            "<div style='color:#EEEEEE !important;'>%1</div>"
            "</body></html>"
        ).arg(htmlBody);

        m_doc.setHtml(m_text);
        m_doc.setDocumentMargin(0);

        m_doc.setTextWidth(-1);
        qreal idealW = m_doc.idealWidth();

        if (idealW > 450) {
            m_doc.setTextWidth(450);
        } else {
            m_doc.setTextWidth(idealW);
        }

        QSize textSize = m_doc.size().toSize();

        int padX = 12;
        int padY = 8;

        w = textSize.width() + padX * 2;
        h = textSize.height() + padY * 2;

        w = qMax(w, 40);
        h = qMax(h, 24);
    }

    resize(w, h);

    QPoint pos;
    if (m_pendingExactPosition) {
        pos = m_pendingPos;
    } else {
        pos = m_pendingPos + QPoint(15, 15);
        QScreen* screen = QGuiApplication::screenAt(m_pendingPos);
        if (!screen) screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect screenGeom = screen->geometry();
            if (pos.x() + width() > screenGeom.right()) {
                pos.setX(m_pendingPos.x() - width() - 15);
            }
            if (pos.y() + height() > screenGeom.bottom()) {
                pos.setY(m_pendingPos.y() - height() - 15);
            }
        }
    }

    move(pos);
    setWindowOpacity(1.0);
    show();
    update();
    raise();

    if (timeout > 0) {
        m_hideTimer.start(timeout);
    } else {
        m_hideTimer.stop();
    }
}

void ToolTipOverlay::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF rectF(0.5, 0.5, width() - 1, height() - 1);

    p.setPen(QPen(m_currentBorderColor, 1));
    p.setBrush(m_currentBackgroundColor);
    // ToolTip 圆角锁定为 2px
    p.drawRoundedRect(rectF, 2, 2);

    if (!m_text.isEmpty()) {
        p.save();
        p.translate(12, 8);
        m_doc.drawContents(&p);
        p.restore();
    }
}

} // namespace QuarkMeta