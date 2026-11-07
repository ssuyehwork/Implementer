#pragma once
#include <QColor>
#include <QString>

namespace QuarkMeta {
namespace Style {

/**
 * @brief 物理 UI 常量引擎 (StyleLibrary)
 * 解决硬编码问题，实现全应用视觉契约。
 */

// 核心品牌色
const QColor PrimaryBlue   = QColor("#3498db");
const QColor SuccessGreen   = QColor("#2ecc71");
const QColor WarningOrange  = QColor("#f39c12");
const QColor ErrorRed       = QColor("#e81123");
const QColor BrandOrange    = QColor("#cb7208"); // 核心品牌色 (QuarkMeta)
const QColor ActiveOrange   = QColor("#ff551c"); // 2026-06-23 物理级拨乱反正：全局置顶/激活态唯一合法色值
const QColor AccentCyan     = QColor("#41F2F2");

// 面板背景色
const QColor BackgroundDeep   = QColor("#1E1E1E");
const QColor BackgroundHeader = QColor("#252526");
const QColor BackgroundHover  = QColor("#2A2A2A");
const QColor BackgroundSelected = QColor("#282828");
const QColor BorderColor      = QColor("#333333");
const QColor BorderDark       = QColor("#444444");

// 文字颜色
const QColor TextMain       = QColor("#EEEEEE");
const QColor TextDim        = QColor("#B0B0B0");
const QColor TextDark       = QColor("#AAAAAA");
const QColor TextMuted      = QColor("#888888");

// 布局常量
const int StandardIconSize  = 18;
const int ToolBtnIconSize   = 18;
const int RowHeight         = 28;
const int TreeIndentation   = 20;
const int TitleBarHeight    = 32;
const int StatusBarHeight   = 28;

// 交互反馈
const QColor HoverBackground = QColor("#3E3E42");
const QColor PressedBackground = QColor("#4E4E52");

struct ColorTagItem {
    QString hex;
    QColor color;
    QString name;
};

inline const QList<ColorTagItem>& getColorPalette() {
    static const QList<ColorTagItem> s_palette = {
        {"",        QColor("#888780"), "无色标"},
        {"#E24B4A", QColor("#E24B4A"), "红色"},
        {"#EF9F27", QColor("#EF9F27"), "橙色"},
        {"#FECF0E", QColor("#FECF0E"), "黄色"},
        {"#639922", QColor("#639922"), "绿色"},
        {"#1D9E75", QColor("#1D9E75"), "青色"},
        {"#378ADD", QColor("#378ADD"), "蓝色"},
        {"#7F77DD", QColor("#7F77DD"), "紫色"},
        {"#5F5E5A", QColor("#5F5E5A"), "灰色"}
    };
    return s_palette;
}

inline QString getColorHexByName(const QString& name) {
    for (const auto& item : getColorPalette()) {
        if (item.name == name) return item.hex;
    }
    return name; // Fallback if already hex
}

// QSS Helper
inline QString qssColor(const QColor& color) { return color.name(); }

} // namespace Style
} // namespace QuarkMeta
