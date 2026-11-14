#include "QuickLookWindow.h"
#include "UiHelper.h"
#include "ColorPaletteEngine.h"
#include <QTextBlock>
#include "../meta/HistoryService.h"
#include "ShellIconManager.h"
#include "FramelessWindowHelper.h"
#include "ImageDecoderFacade.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QMimeData>
#include <QDir>
#include <QDesktopServices>
#include <QContextMenuEvent>
#include "../core/AppConfig.h"
#include "controllers/ContextMenuFactory.h"
#include "dialogs/TextExtensionDialog.h"
#include "ColorPicker.h"
#include "FramelessWindowHelper.h"
#include "../meta/MetadataManager.h"
#include "../meta/LibraryService.h"
#include "FormatDecoders.h"
#include <QWidgetAction>
#include <QCryptographicHash>
#include <QStandardPaths>
#include <QFileInfo>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QFile>
#include <QStringDecoder>
#include <QScrollBar>
#include <algorithm>
#include <QSvgRenderer>
#include <QtConcurrent>
#include <QPointer>
#include <QTimer>
#include <QWheelEvent>
#include <QSet>
#include <QUrl>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace RapidNotes {


// ==========================================
// QuickLookWindow 实现
// ==========================================

QuickLookWindow& QuickLookWindow::instance() {
    static QuickLookWindow inst;
    return inst;
}

QuickLookWindow::QuickLookWindow() : QWidget(nullptr) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool);
    m_previewThreadPool.setMaxThreadCount(2);
    
    setupUi();
    installEventFilter(this);

    // [归一化] 8 向拖拽缩放统一接入 FramelessWindowHelper（SSOT）
    m_framelessHelper = FramelessWindowHelper::apply(this, nullptr);
}

QuickLookWindow::~QuickLookWindow() {}

void QuickLookWindow::setupUi() {
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    m_container = new QWidget();
    m_container->setObjectName("QLContainer");

    auto* layout = new QVBoxLayout(m_container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_titleLabel = new QLabel(m_container);
    m_titleLabel->setObjectName("QLTitle");
    m_titleLabel->hide();

    // 图片渲染控件
    m_graphicsView = new QuickLookGraphicsView();
    m_graphicsView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_graphicsView->installEventFilter(this);
    layout->addWidget(m_graphicsView);

    // 文本渲染控件
    m_textEdit = new QPlainTextEdit();
    m_textEdit->setReadOnly(true);
    m_textEdit->setObjectName("QLPlainTextEdit");
    m_textEdit->hide();
    m_textEdit->installEventFilter(this);
    m_textEdit->viewport()->installEventFilter(this);
    layout->addWidget(m_textEdit);

    // 空文本提示标签
    m_lblEmptyPrompt = new QLabel(m_container);
    m_lblEmptyPrompt->setAlignment(Qt::AlignCenter);
    m_lblEmptyPrompt->setObjectName("QLEmptyPromptLabel");
    m_lblEmptyPrompt->hide();
    m_lblEmptyPrompt->installEventFilter(this);
    m_container->installEventFilter(this);
    layout->addWidget(m_lblEmptyPrompt);

    // 状态与信息标签
    m_infoLabel = new QLabel(m_container);
    m_infoLabel->setObjectName("QLInfoLabel");
    m_infoLabel->hide();

    // 🚀【QuickLook 文本搜索悬浮浮层】：两层紧凑型结构
    m_searchBar = new QWidget(this);
    m_searchBar->setObjectName("QLSearchBar");
    m_searchBar->hide();

    auto* searchBarLayout = new QVBoxLayout(m_searchBar);
    searchBarLayout->setContentsMargins(8, 6, 8, 6);
    searchBarLayout->setSpacing(4);

    // 上下两层布局：使状态指示行与搜索输入框左右精准对齐
    auto* mainRow = new QHBoxLayout();
    mainRow->setContentsMargins(0, 0, 0, 0);
    mainRow->setSpacing(6);

    auto* leftCol = new QVBoxLayout();
    leftCol->setContentsMargins(0, 0, 0, 0);
    leftCol->setSpacing(4);

    m_searchEdit = new QLineEdit(m_searchBar);
    m_searchEdit->setObjectName("QLSearchEdit");
    m_searchEdit->setPlaceholderText("在文本中查找...");
    leftCol->addWidget(m_searchEdit);

    m_lblStatus = new QLabel("无 | 0", m_searchBar);
    m_lblStatus->setObjectName("QLSearchStatus");
    leftCol->addWidget(m_lblStatus);

    mainRow->addLayout(leftCol, 1);

    auto* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(6);

    m_btnFindNext = new QPushButton("查找下一个", m_searchBar);
    m_btnFindNext->setObjectName("QLSearchBtn");
    btnLayout->addWidget(m_btnFindNext);

    m_btnCount = new QPushButton("计数", m_searchBar);
    m_btnCount->setObjectName("QLSearchBtn");
    btnLayout->addWidget(m_btnCount);

    auto* rightCol = new QVBoxLayout();
    rightCol->setContentsMargins(0, 0, 0, 0);
    rightCol->setSpacing(4);
    rightCol->addLayout(btnLayout);
    rightCol->addStretch();

    mainRow->addLayout(rightCol);
    searchBarLayout->addLayout(mainRow);

    connect(m_btnFindNext, &QPushButton::clicked, this, &QuickLookWindow::findNext);
    connect(m_btnCount, &QPushButton::clicked, this, &QuickLookWindow::updateMatchCount);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &QuickLookWindow::findNext);

    m_searchBar->installEventFilter(this);
    m_searchEdit->installEventFilter(this);

    rootLayout->addWidget(m_container);
}

void QuickLookWindow::preview(const QString& filePath) {
    m_currentPath = filePath;
    QFileInfo fi(filePath);
    m_titleLabel->setText(fi.fileName());
    m_infoLabel->setObjectName("QLInfoLabel");
    
    QString ext = fi.suffix().toLower();
    
    if (ColorPaletteEngine::isGraphicsFile(ext)) {
        renderImage(filePath);
    } else {
        renderText(filePath);
    }

    showFullScreen();
    raise();
    activateWindow();

    m_ignoreDeactivate = true;
    QTimer::singleShot(150, this, [this]() {
        m_ignoreDeactivate = false;
    });
    FramelessWindowHelper::setAlwaysOnTop(this, true);
}

void QuickLookWindow::closePreview() {
    hideSearchBar();
    m_previewGeneration.fetch_add(1, std::memory_order_relaxed);
    if (m_previewToken) m_previewToken->cancel();
    if (m_graphicsView) {
        m_graphicsView->clear();
    }
    hide();
}

void QuickLookWindow::renderImage(const QString& path) {
    hideSearchBar();
    m_textEdit->hide();
    if (m_lblEmptyPrompt) m_lblEmptyPrompt->hide();
    m_graphicsView->show();
    m_graphicsView->clear();
    m_infoLabel->setText("正在加载预览...");

    QFileInfo fi(path);
    QString ext = fi.suffix().toLower();

    static const QSet<QString> QT_NATIVE_FORMATS = {"png", "jpg", "jpeg", "bmp", "gif"};

    // 新一次预览到来时，先取消上一次还在跑的 Ghostscript / 解码 
    if (m_previewToken) m_previewToken->cancel(); 
    m_previewToken = std::make_shared<CancellationToken>(); 
    std::shared_ptr<CancellationToken> token = m_previewToken; 
 
    QPointer<QuickLookWindow> weakThis(this); 
    (void)QtConcurrent::run(&m_previewThreadPool, [weakThis, path, ext, token]() { 
        if (!weakThis) return;
        
        QImage img = loadOrExtractQuickLookImage(path, 2048, token);

        if (!weakThis) return;
        QMetaObject::invokeMethod(weakThis.data(), [weakThis, img, path]() {
            if (!weakThis || weakThis->m_currentPath != path) return;
            if (!img.isNull()) {
                qint64 totalPixels = static_cast<qint64>(img.width()) * img.height();
                bool isHuge = totalPixels > 50000000LL; // 超过 5000 万像素安全降采样
                
                QPixmap pix;
                if (isHuge) {
                    pix = QPixmap::fromImage(img.scaled(4096, 4096, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    weakThis->m_infoLabel->setText(QString("超大图像（已应用安全限制）: %1x%2 | %3")
                        .arg(img.width()).arg(img.height()).arg(path));
                } else {
                    pix = QPixmap::fromImage(img);
                    weakThis->m_infoLabel->setText(QString("%1x%2 | %3")
                        .arg(img.width()).arg(img.height()).arg(path));
                }
                pix.setDevicePixelRatio(weakThis->devicePixelRatioF());
                weakThis->m_graphicsView->setPixmap(pix);
            } else {
                weakThis->renderText(path); // 图片加载失败尝试文本模式
            }
        });
    });
}

void QuickLookWindow::renderText(const QString& path) {
    hideSearchBar();
    m_graphicsView->hide();
    m_lblEmptyPrompt->hide();
    m_textEdit->show();
    m_textEdit->setPlainText("正在读取文件...");

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        m_textEdit->setPlainText("无法打开文件进行预览。");
        return;
    }

    QByteArray fileData = file.read(128 * 1024);
    file.close();

    if (file.size() == 0 || fileData.trimmed().isEmpty()) {
        m_textEdit->hide();
        m_lblEmptyPrompt->setText("该项目内容为空");
        m_lblEmptyPrompt->show();
        m_infoLabel->setText(QString("大小: 0 KB | %1").arg(path));
        return;
    }

    bool potentialUtf16 = fileData.startsWith("\xFF\xFE") || fileData.startsWith("\xFE\xFF");
    if (!potentialUtf16 && isBinary(fileData)) {
        m_textEdit->hide();
        m_graphicsView->show();
        m_graphicsView->clear();
        
        QIcon fileIcon = ShellIconManager::getFileIcon(path, 256);
        QPixmap pix = fileIcon.pixmap(256, 256);
        m_graphicsView->setPixmap(pix);
        
        m_infoLabel->setText("二进制文件，无法直接预览文本");
        m_infoLabel->setObjectName("QLInfoLabelWarn");
        return;
    }

    QString encodingName = detectEncoding(fileData);
    QString text;

    if (encodingName == "UTF-8") {
        text = QString::fromUtf8(fileData);
    } else if (encodingName == "UTF-16LE") {
        auto decoder = QStringDecoder(QStringDecoder::Utf16LE);
        text = decoder(fileData);
    } else if (encodingName == "UTF-16BE") {
        auto decoder = QStringDecoder(QStringDecoder::Utf16BE);
        text = decoder(fileData);
    } else {
        text = QString::fromLocal8Bit(fileData);
    }

    m_textEdit->setPlainText(text);
    m_textEdit->verticalScrollBar()->setValue(0);
    m_infoLabel->setText(QString("编码: %1 | 大小: %2 KB | %3").arg(encodingName).arg(QFileInfo(path).size() / 1024.0, 0, 'f', 1).arg(path));
}

QImage QuickLookWindow::loadOrExtractQuickLookEps(const QString& filePath, int targetSize, std::shared_ptr<CancellationToken> token) {
    return loadOrExtractQuickLookImage(filePath, targetSize, token);
}

QImage QuickLookWindow::loadOrExtractQuickLookImage(const QString& filePath, int targetSize, std::shared_ptr<CancellationToken> token) {
    // 🚀【全图形图像 QuickLook 统一缓存架构】：使用 SHA256 与两级分桶
    QByteArray normalized = QDir::toNativeSeparators(filePath).toLower().toUtf8();
    QString hashStr = QString::fromUtf8(QCryptographicHash::hash(normalized, QCryptographicHash::Sha256).toHex());
    QString dirL1 = hashStr.left(2);
    QString dirL2 = hashStr.mid(2, 2);

    QString baseCacheDir = AppConfig::instance().getGlobalCacheDir() + "/quicklook_previews";
    QString cacheDir = QDir(baseCacheDir).filePath(QString("%1/%2").arg(dirL1, dirL2));
    QDir().mkpath(cacheDir);

    QFileInfo srcInfo(filePath);
    QString cachePath = QDir(cacheDir).filePath(QString("%1.png").arg(hashStr));
    QFileInfo cacheInfo(cachePath);

    // 1. 若独立缓存命中且未过期，直接 0ms 秒级载入高清预览图
    if (cacheInfo.exists() && cacheInfo.size() > 0 && cacheInfo.lastModified() >= srcInfo.lastModified()) {
        QImage cachedImg(cachePath);
        if (!cachedImg.isNull()) {
            return cachedImg;
        }
    }

    // 2. 缓存未命中：按图形格式提取高清 2048px 预览图
    QString ext = srcInfo.suffix().toLower();
    QImage highQImg;
    bool shouldSaveCache = true;

    if (ext == "eps") {
        bool fromGhostscript = false;
        highQImg = FormatDecoders::extractEpsQuickLook(filePath, targetSize, ImageDecoderFacade::kLongTimeoutMs, token, &fromGhostscript);
        if (!fromGhostscript) shouldSaveCache = false; // 仅缓存高质量 Ghostscript 渲染
    } else if (ext == "svg") {
        QSvgRenderer renderer(filePath);
        if (renderer.isValid()) {
            highQImg = QImage(targetSize, targetSize, QImage::Format_ARGB32);
            highQImg.fill(Qt::transparent);
            QPainter painter(&highQImg);
            renderer.render(&painter);
        }
    } else {
        static const QSet<QString> nativeFormats = {"png", "jpg", "jpeg", "bmp", "gif"};
        if (nativeFormats.contains(ext)) {
            highQImg.load(filePath);
        } else {
            highQImg = ImageDecoderFacade::decodeSinglePass(filePath, targetSize, ImageDecoderFacade::ExtractMode::Preview, token).thumbnail512;
        }
    }

    if (highQImg.isNull()) {
        return QImage();
    }

    // 3. 将高质量预览图缓存至 quicklook_previews
    if (shouldSaveCache) {
        highQImg.save(cachePath, "PNG");
    }
    return highQImg;
}

bool QuickLookWindow::isBinary(const QByteArray& fileData) {
    if (fileData.isEmpty()) return false;
    int checkLen = std::min<int>(fileData.size(), 1024);
    int continuousNull = 0;
    for (int i = 0; i < checkLen; ++i) {
        if (fileData[i] == '\0') {
            continuousNull++;
            if (continuousNull > 2) return true;
        } else {
            continuousNull = 0;
        }
    }
    return false;
}

QString QuickLookWindow::detectEncoding(const QByteArray& fileData) {
    if (fileData.startsWith("\xEF\xBB\xBF")) return "UTF-8";
    if (fileData.startsWith("\xFF\xFE")) return "UTF-16LE";
    if (fileData.startsWith("\xFE\xFF")) return "UTF-16BE";

    int utf8Count = 0;
    for (int i = 0; i < fileData.size() - 2; ++i) {
        unsigned char c = (unsigned char)fileData[i];
        if (c >= 0xC0 && c <= 0xDF) {
            if ((unsigned char)fileData[i+1] >= 0x80 && (unsigned char)fileData[i+1] <= 0xBF) { utf8Count++; i++; }
        } else if (c >= 0xE0 && c <= 0xEF) {
            if ((unsigned char)fileData[i+1] >= 0x80 && (unsigned char)fileData[i+1] <= 0xBF &&
                (unsigned char)fileData[i+2] >= 0x80 && (unsigned char)fileData[i+2] <= 0xBF) { utf8Count += 2; i += 2; }
        }
    }

    return (utf8Count > 0) ? "UTF-8" : "GBK";
}

void QuickLookWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W && (event->modifiers() & Qt::ControlModifier)) {
        closePreview();
        event->accept();
        return;
    }

    // 🚀【Ctrl+F 场景化门禁】：仅在非图形图像预览（纯文本 m_textEdit 可视）时呼出搜索栏
    if (event->key() == Qt::Key_F && (event->modifiers() & Qt::ControlModifier)) {
        if (m_textEdit && m_textEdit->isVisible()) {
            showSearchBar();
            event->accept();
            return;
        }
    }

    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Escape) {
        closePreview();
        return;
    }
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Left) {
        emit prevRequested();
        return;
    }
    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_Right) {
        emit nextRequested();
        return;
    }

    if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_5 && !(event->modifiers() & Qt::AltModifier)) {
        int rating = event->key() - Qt::Key_0;
        emit ratingRequested(rating);
        return;
    }

    if (event->modifiers() & Qt::AltModifier && event->key() >= Qt::Key_1 && event->key() <= Qt::Key_9) {
        QString color;
        switch (event->key()) {
            case Qt::Key_1: color = "red"; break;
            case Qt::Key_2: color = "orange"; break;
            case Qt::Key_3: color = "yellow"; break;
            case Qt::Key_4: color = "green"; break;
            case Qt::Key_5: color = "cyan"; break;
            case Qt::Key_6: color = "blue"; break;
            case Qt::Key_7: color = "purple"; break;
            case Qt::Key_8: color = "gray"; break;
            case Qt::Key_9: color = ""; break;
        }
        emit colorRequested(color);
        return;
    }

    QWidget::keyPressEvent(event);
}

void QuickLookWindow::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    updateSearchBarGeometry();
}

void QuickLookWindow::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateSearchBarGeometry();
}

void QuickLookWindow::handleSearchShortcut() {
    // 🚀【场景化门禁】：当且仅当预览的是非图形图像（文本模式且 m_textEdit 可视）时才弹出搜索框
    if (m_textEdit && m_textEdit->isVisible()) {
        showSearchBar();
    }
}

void QuickLookWindow::updateSearchBarGeometry() {
    if (!m_searchBar) return;
    int barW = 360;
    int barH = m_searchBar->sizeHint().height();
    if (barH < 62) barH = 62;
    int x = width() - barW - 24;
    int y = 24;
    m_searchBar->setGeometry(x, y, barW, barH);
    m_searchBar->raise();
}

void QuickLookWindow::showSearchBar() {
    if (!m_searchBar) return;
    updateSearchBarGeometry();
    m_searchBar->show();
    m_searchBar->raise();
    m_searchEdit->setFocus();
    m_searchEdit->selectAll();
}

void QuickLookWindow::hideSearchBar() {
    if (m_searchBar && m_searchBar->isVisible()) {
        m_searchBar->hide();
        clearSearchHighlights();
    }
}

void QuickLookWindow::clearSearchHighlights() {
    m_lastSearchQuery.clear();
    m_cachedTotalMatches = 0;
    if (m_textEdit) {
        m_textEdit->setExtraSelections({});
    }
    if (m_lblStatus) {
        m_lblStatus->setText("无 | 0");
    }
}

void QuickLookWindow::updateMatchCount() {
    if (!m_textEdit) return;
    QString query = m_searchEdit->text();
    if (query.isEmpty()) {
        clearSearchHighlights();
        return;
    }

    QTextDocument* doc = m_textEdit->document();
    int count = 0;
    int currentLine = -1;
    QTextCursor currentCursor = m_textEdit->textCursor();

    QTextCursor cursor(doc);
    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = doc->find(query, cursor);
        if (!cursor.isNull()) {
            count++;
            if (cursor.selectionStart() <= currentCursor.selectionStart() &&
                cursor.selectionEnd() >= currentCursor.selectionEnd()) {
                currentLine = cursor.blockNumber() + 1;
            }
        }
    }

    m_lastSearchQuery = query;
    m_cachedTotalMatches = count;

    if (count == 0) {
        m_lblStatus->setText("无 | 0");
    } else {
        QString lineStr = (currentLine > 0) ? QString::number(currentLine) : "无";
        m_lblStatus->setText(QString("%1 | %2").arg(lineStr).arg(count));
    }
}

void QuickLookWindow::findNext() {
    if (!m_textEdit) return;
    QString query = m_searchEdit->text();
    if (query.isEmpty()) {
        clearSearchHighlights();
        return;
    }

    QTextDocument* doc = m_textEdit->document();
    QTextCursor startCursor = m_textEdit->textCursor();
    QTextCursor match = doc->find(query, startCursor);

    // 如果到底，从文档开头循环查找
    if (match.isNull()) {
        QTextCursor docStart(doc);
        match = doc->find(query, docStart);
    }

    if (!match.isNull()) {
        m_textEdit->setTextCursor(match);

        // 设置 #ff551C 高亮
        QList<QTextEdit::ExtraSelection> extraSelections;
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(QColor("#ff551C"));
        selection.format.setForeground(Qt::white);
        selection.cursor = match;
        extraSelections.append(selection);
        m_textEdit->setExtraSelections(extraSelections);

        // 定位当前行号与总数
        int currentLine = match.blockNumber() + 1;
        if (m_lastSearchQuery != query) {
            int totalMatches = 0;
            QTextCursor c(doc);
            while (!c.isNull() && !c.atEnd()) {
                c = doc->find(query, c);
                if (!c.isNull()) totalMatches++;
            }
            m_lastSearchQuery = query;
            m_cachedTotalMatches = totalMatches;
        }

        m_lblStatus->setText(QString("%1 | %2").arg(currentLine).arg(m_cachedTotalMatches));
        m_textEdit->centerCursor();
    } else {
        clearSearchHighlights();
    }
}

bool QuickLookWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
    // [归一化] 8 向拖拽缩放的原生命中测试统一走 FramelessWindowHelper（SSOT）
    if (m_framelessHelper && m_framelessHelper->handleNativeEvent(message, result)) {
        return true;
    }
    return QWidget::nativeEvent(eventType, message, result);
}

bool QuickLookWindow::eventFilter(QObject* watched, QEvent* event) {
    bool hasTextEditViewport = m_textEdit && m_textEdit->viewport();

    if ((watched == m_textEdit || (hasTextEditViewport && watched == m_textEdit->viewport()) ||
         watched == m_graphicsView || watched == m_lblEmptyPrompt ||
         watched == m_container || watched == this) && event->type() == QEvent::MouseButtonDblClick) {
        closePreview();
        return true;
    }

    if ((watched == m_textEdit || (hasTextEditViewport && watched == m_textEdit->viewport()) || watched == m_graphicsView) && event->type() == QEvent::ContextMenu) {
        showContextMenu(static_cast<QContextMenuEvent*>(event)->globalPos());
        return true;
    }

    if ((watched == m_textEdit || (hasTextEditViewport && watched == m_textEdit->viewport()) || watched == m_graphicsView) && event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        bool intercept = false;
        int key = keyEvent->key();
        Qt::KeyboardModifiers mods = keyEvent->modifiers();
        
        if (key == Qt::Key_Space || key == Qt::Key_Escape) {
            if (m_searchBar && m_searchBar->isVisible() && key == Qt::Key_Escape) {
                hideSearchBar();
                return true;
            }
            // 如果用户正在搜索输入框中输入空格，不拦截关闭，原生放行给输入框打字
            if (m_searchEdit && m_searchEdit->hasFocus() && key == Qt::Key_Space) {
                return false;
            }
            intercept = true;
        } else if (key == Qt::Key_W && (mods & Qt::ControlModifier)) {
            intercept = true;
        } else if (key == Qt::Key_F && (mods & Qt::ControlModifier)) {
            handleSearchShortcut();
            return true;
        } else if (key == Qt::Key_Up || key == Qt::Key_Left || key == Qt::Key_Down || key == Qt::Key_Right) {
            intercept = true;
        } else if (key >= Qt::Key_1 && key <= Qt::Key_5 && !(mods & Qt::AltModifier)) {
            intercept = true;
        } else if ((mods & Qt::AltModifier) && key >= Qt::Key_1 && key <= Qt::Key_9) {
            intercept = true;
        }
        
        if (intercept) {
            keyPressEvent(keyEvent);
            return true;
        }
    }

    if (event->type() == QEvent::WindowDeactivate) {
        if (m_ignoreDeactivate) {
            return true;
        }

        QWidget* activeWin = QApplication::activeWindow();
        QWidget* focusW = QApplication::focusWidget();
        if (activeWin == this || (focusW && this->isAncestorOf(focusW)) ||
            (m_searchBar && (m_searchBar == activeWin || m_searchBar->isAncestorOf(focusW)))) {
            return true;
        }

        closePreview();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void QuickLookWindow::contextMenuEvent(QContextMenuEvent* event) {
    showContextMenu(event->globalPos());
}

void QuickLookWindow::showContextMenu(const QPoint& globalPos) {
    if (m_currentPath.isEmpty()) return;

    QMenu menu(this);
    UiHelper::applyMenuStyle(&menu);

    QAction* actPrev = menu.addAction(UiHelper::getIcon("scroll-007", QColor("#FFFFFF"), 18), "上一个");
    QAction* actNext = menu.addAction(UiHelper::getIcon("scroll-006", QColor("#FFFFFF"), 18), "下一个");
    menu.addSeparator();

    QAction* actRotate = menu.addAction(UiHelper::getIcon("refresh", QColor("#FFFFFF"), 18), "旋转");
    QAction* actFlip = menu.addAction(UiHelper::getIcon("split_v", QColor("#FFFFFF"), 18), "水平翻转");
    QAction* actOrig = menu.addAction(UiHelper::getIcon("image_picture", QColor("#FFFFFF"), 18), "原始");
    QAction* actFit = menu.addAction(UiHelper::getIcon("resize2", QColor("#FFFFFF"), 18), "自适应");
    actOrig->setCheckable(true);
    actFit->setCheckable(true);
    bool isFit = m_graphicsView->isFitMode();
    actFit->setChecked(isFit);
    actOrig->setChecked(!isFit);
    menu.addSeparator();

    ContextMenuFactory::buildOpenWithMenu(&menu, m_currentPath, this);
    ContextMenuFactory::buildShowInExplorerAction(&menu, m_currentPath, this);
    menu.addSeparator();

    // 1. 颜色标记选择栏
    QString currentColorStr;
    auto meta = MetadataManager::instance().getMeta(m_currentPath.toStdWString());
    currentColorStr = QString::fromStdWString(meta.manualColor);

    QWidgetAction* pickerAction = new QWidgetAction(&menu);
    ColorStripPicker* pickerWidget = new ColorStripPicker(currentColorStr, &menu);
    pickerAction->setDefaultWidget(pickerWidget);
    menu.addAction(pickerAction);

    connect(pickerWidget, &ColorStripPicker::colorSelected, this, [this, &menu](const QString& hexColor) {
        emit colorRequested(hexColor);
        menu.close();
    });

    // 2. 置顶功能项
    ContextMenuFactory::buildPinToggleAction(&menu, FramelessWindowHelper::isAlwaysOnTop(this), [this](bool pin) {
        FramelessWindowHelper::setAlwaysOnTop(this, pin);
    }, this);

    // 3. 添加至“库”功能项
    LibraryService::instance().buildLibraryAction(&menu, QStringList{m_currentPath}, this);
    menu.addSeparator();

    QAction* actCopy = menu.addAction(UiHelper::getIcon("copy", QColor("#EEEEEE"), 18), "复制");
    QAction* actCut = menu.addAction(UiHelper::getIcon("cut", QColor("#EEEEEE"), 18), "剪切");
    QAction* actDel = menu.addAction(UiHelper::getIcon("trash", QColor("#EEEEEE"), 18), "删除");
    menu.addSeparator();

    ContextMenuFactory::buildCopyNameAction(&menu, QStringList{m_currentPath}, this);
    ContextMenuFactory::buildCopyPathAction(&menu, QStringList{m_currentPath}, this);
    ContextMenuFactory::buildExtractContentAction(&menu, m_currentPath, this);
    menu.addSeparator();

    QAction* actTextExtSettings = menu.addAction(UiHelper::getIcon("Ext.svg", QColor("#EEEEEE"), 18), "文本扩展名设置...");

    bool isImage = m_graphicsView->isVisible();
    actRotate->setEnabled(isImage);
    actFlip->setEnabled(isImage);
    actOrig->setEnabled(isImage);
    actFit->setEnabled(isImage);

    QAction* selected = menu.exec(globalPos);
    if (!selected) return;

    if (selected == actPrev) {
        emit prevRequested();
    } else if (selected == actNext) {
        emit nextRequested();
    } else if (selected == actRotate) {
        m_graphicsView->rotateClockwise();
    } else if (selected == actFlip) {
        m_graphicsView->flipHorizontal();
    } else if (selected == actOrig) {
        m_graphicsView->setZoomOriginal();
    } else if (selected == actFit) {
        m_graphicsView->fitImage();
    } else if (selected == actCopy) {
        QList<QUrl> urls;
        urls << QUrl::fromLocalFile(m_currentPath);
        QMimeData* mime = new QMimeData();
        mime->setUrls(urls);
        QApplication::clipboard()->setMimeData(mime);
    } else if (selected == actCut) {
        QList<QUrl> urls;
        urls << QUrl::fromLocalFile(m_currentPath);
        QMimeData* mime = new QMimeData();
        mime->setUrls(urls);
        QByteArray effectData;
        effectData.append((char)2);
        mime->setData("Preferred DropEffect", effectData);
        QApplication::clipboard()->setMimeData(mime);
    } else if (selected == actDel) {
        emit deleteRequested(m_currentPath);
    } else if (selected == actTextExtSettings) {
        TextExtensionDialog dlg(this);
        dlg.exec();
    }
}

} // namespace RapidNotes
