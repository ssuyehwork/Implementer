#include "ColorPaletteEngine.h"
#include "../meta/ExtensionColorDao.h"
#include <QImageReader>
#include <QFileInfo>
#include <QSet>
#include <QMap>
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace QuarkMeta {

bool ColorPaletteEngine::isGraphicsFile(const QString& ext) {
    static const QSet<QString> graphicsExts = {
        "jpg", "jpeg", "png", "bmp", "webp", "gif", "ico", "cur", "ani",
        "psd", "psb", "ai", "eps", "svg", "tif", "tiff", "hdr", "tga",
        "dng", "cr2", "cr3", "nef", "arw", "rw2", "orf", "heic", "avif"
    };
    return graphicsExts.contains(ext.toLower().trimmed());
}

bool ColorPaletteEngine::isStandardImage(const QString& ext) {
    static const QSet<QString> stdExts = {
        "jpg", "jpeg", "png", "bmp", "webp", "gif", "ico", "tif", "tiff"
    };
    return stdExts.contains(ext.toLower().trimmed());
}

bool ColorPaletteEngine::isVectorGraphics(const QString& ext) {
    QString e = ext.toLower().trimmed();
    return (e == "svg" || e == "ai" || e == "eps");
}

bool ColorPaletteEngine::isRawPhoto(const QString& ext) {
    static const QSet<QString> rawExts = {
        "dng", "cr2", "cr3", "nef", "arw", "rw2", "orf"
    };
    return rawExts.contains(ext.toLower().trimmed());
}

QColor ColorPaletteEngine::parseColorName(const QString& colorName) {
    if (colorName.isEmpty()) return QColor();
    
    QColor c(colorName);
    if (c.isValid()) return c;

    QString name = colorName.trimmed().toLower();
    if (name == "red"    || name == "红" || name == "红色") return QColor("#E24B4A");
    if (name == "orange" || name == "橙" || name == "橙色") return QColor("#EF9F27");
    if (name == "yellow" || name == "黄" || name == "黄色") return QColor("#FECF0E");
    if (name == "green"  || name == "绿" || name == "绿色") return QColor("#639922");
    if (name == "cyan"   || name == "青" || name == "青色") return QColor("#1D9E75");
    if (name == "blue"   || name == "蓝" || name == "蓝色") return QColor("#378ADD");
    if (name == "purple" || name == "紫" || name == "紫色") return QColor("#7F77DD");
    if (name == "gray"   || name == "灰" || name == "灰色") return QColor("#5F5E5A");
    if (name == "black"  || name == "黑" || name == "黑色") return QColor("#000000");
    if (name == "white"  || name == "白" || name == "白色") return QColor("#FFFFFF");
    
    return QColor();
}

QString ColorPaletteEngine::normalizeColorHex(const QString& colorStr) {
    if (colorStr.trimmed().isEmpty()) return "";
    QColor c = parseColorName(colorStr);
    if (!c.isValid()) return "";
    return c.name().toUpper();
}

QColor ColorPaletteEngine::getExtensionColor(const QString& ext) {
    return getExtensionBadgeColors(ext).first;
}

QPair<QColor, QColor> ColorPaletteEngine::getExtensionBadgeColors(const QString& ext) {
    QString e = ext.toLower().trimmed();

    // 1. 内存缓存与数据库初始化
    static QMap<QString, QPair<QColor, QColor>> s_colorCache;
    static bool s_tableInited = false;

    if (!s_tableInited) {
        ExtensionColorDao::initTable();
        s_colorCache = ExtensionColorDao::loadAllColors();
        s_tableInited = true;
    }

    // 2. 独占硬编码保护项校验与旧持久化缓存擦除同步
    auto resolveReserved = [&](const QPair<QColor, QColor>& reservedColors) -> QPair<QColor, QColor> {
        if (!s_colorCache.contains(e) || s_colorCache.value(e) != reservedColors) {
            ExtensionColorDao::saveExtensionColor(e, reservedColors.first, reservedColors.second, false);
            s_colorCache.insert(e, reservedColors);
        }
        return reservedColors;
    };

    if (e == "dir" || e == "folder") return resolveReserved({ QColor("#1C2D37"), QColor("#38BDF8") });
    if (e == "psd" || e == "psb")    return resolveReserved({ QColor("#001D26"), QColor("#02B1DD") });
    if (e == "eps")                  return resolveReserved({ QColor("#35483D"), QColor("#F88025") });
    if (e == "ai")                   return resolveReserved({ QColor("#F88025"), QColor("#35483D") });

    // 3. 普通扩展名内存缓存查找
    if (s_colorCache.contains(e)) {
        return s_colorCache.value(e);
    }

    // 3. 动态生成新配色并物理落盘写入 QuarkMeta.store
    uint hashVal = static_cast<uint>(qHash(e));
    int hue = static_cast<int>(hashVal % 360);
    int saturation = 130 + static_cast<int>((hashVal >> 8) % 80);
    int lightness = 80 + static_cast<int>((hashVal >> 16) % 60);

    QColor bgColor = QColor::fromHsl(hue, saturation, lightness);
    double luminance = (0.299 * bgColor.red() + 0.587 * bgColor.green() + 0.114 * bgColor.blue()) / 255.0;
    QColor textColor = (luminance < 0.55) ? QColor("#FFFFFF") : QColor("#1A1A1A");

    QPair<QColor, QColor> colorPair = { bgColor, textColor };
    
    // 刷盘固化并填充内存 Cache
    ExtensionColorDao::saveExtensionColor(e, bgColor, textColor, false);
    s_colorCache.insert(e, colorPair);

    return colorPair;
}

QColor ColorPaletteEngine::extractDominantColor(const QString& filePath) {
    auto palette = extractPalette(filePath, 1);
    if (!palette.isEmpty()) {
        return palette.first().first;
    }
    return QColor();
}

QVector<QPair<QColor, float>> ColorPaletteEngine::extractPalette(const QString& filePath, int maxColors) {
    if (!isGraphicsFile(QFileInfo(filePath).suffix())) return {};

    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    reader.setScaledSize(QSize(128, 128));
    QImage img = reader.read();
    if (img.isNull()) return {};

    return extractPaletteFromImage(img, maxColors);
}

QColor ColorPaletteEngine::extractDominantColorFromImage(const QImage& preScaledImage) {
    auto pal = extractPaletteFromImage(preScaledImage, 1);
    if (!pal.isEmpty()) return pal.first().first;
    return QColor();
}

double ColorPaletteEngine::deltaE76(const QColor& c1, const QColor& c2) {
    LabColor l1 = rgbToLab(c1);
    LabColor l2 = rgbToLab(c2);
    return std::sqrt(std::pow(l1.L - l2.L, 2) + std::pow(l1.a - l2.a, 2) + std::pow(l1.b - l2.b, 2));
}

QVector<QPair<QColor, float>> ColorPaletteEngine::extractWeightedPalette(const QImage& preScaledImage) {
    if (preScaledImage.isNull()) return {};

    QImage sampled = preScaledImage.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    struct BucketInfo {
        long long rSum = 0, gSum = 0, bSum = 0;
        double rankWeight = 0.0;
        int count = 0;
    };
    QMap<QRgb, BucketInfo> bucketStats;
    int totalPixels = 0;

    for (int row = 0; row < sampled.height(); ++row) {
        for (int col = 0; col < sampled.width(); ++col) {
            QRgb rgb = sampled.pixel(col, row);
            if (qAlpha(rgb) < 128) continue;

            int r = qRed(rgb), g = qGreen(rgb), b = qBlue(rgb);
            QColor color(r, g, b);
            int h, s, l; color.getHsl(&h, &s, &l);
            double sat = s / 255.0, lig = l / 255.0;

            double centerX = sampled.width() / 2.0;
            double centerY = sampled.height() / 2.0;
            double maxDist = std::sqrt(centerX * centerX + centerY * centerY);
            double dist = std::sqrt(std::pow(col - centerX, 2) + std::pow(row - centerY, 2));
            double spatialWeight = 1.0 + (1.0 - dist / maxDist) * 0.5;

            double vibrancy = sat * (1.0 - std::abs(lig - 0.5) * 2.0);
            double weight = (0.5 + 4.0 * std::pow(vibrancy, 1.5)) * spatialWeight;

            if (lig > 0.95 && sat < 0.05) {
                weight = 0.001;
            } else if (lig < 0.15) {
                weight = 2.0 * spatialWeight;
            }

            QRgb rgbKey = qRgb(r & 0xF8, g & 0xF8, b & 0xF8);
            auto& stat = bucketStats[rgbKey];
            stat.rSum += r; stat.gSum += g; stat.bSum += b;
            stat.rankWeight += weight;
            stat.count++;
            totalPixels++;
        }
    }
    if (totalPixels == 0) return {};

    struct FinalBucket { QColor avgColor; double rankWeight; int count; };
    QList<FinalBucket> buckets;
    for (auto it = bucketStats.begin(); it != bucketStats.end(); ++it) {
        const auto& s = it.value();
        buckets.append({ QColor((int)(s.rSum / s.count), (int)(s.gSum / s.count), (int)(s.bSum / s.count)), s.rankWeight, s.count });
    }

    QList<FinalBucket> merged;
    for (const auto& b : buckets) {
        bool found = false;
        for (auto& m : merged) {
            double de = deltaE76(b.avgColor, m.avgColor);
            if (de < 10.0) {
                int total = m.count + b.count;
                m.avgColor = QColor(
                    (int)(m.avgColor.red() * m.count + b.avgColor.red() * b.count) / total,
                    (int)(m.avgColor.green() * m.count + b.avgColor.green() * b.count) / total,
                    (int)(m.avgColor.blue() * m.count + b.avgColor.blue() * b.count) / total
                );
                m.rankWeight += b.rankWeight; m.count = total;
                found = true; break;
            }
        }
        if (!found) merged.append(b);
    }

    QVector<QPair<QColor, float>> result;
    struct Candidate { QColor color; double score; int count; };
    QList<Candidate> candidates;
    for (const auto& m : merged) {
        candidates.append({ m.avgColor, m.rankWeight, m.count });
    }

    while (result.size() < 10 && !candidates.isEmpty()) {
        int bestIdx = -1; double maxScore = -1e9;
        for (int i = 0; i < candidates.size(); ++i) {
            const auto& c = candidates[i];
            double score = c.score;

            for (const auto& r : result) {
                double de = deltaE76(c.color, r.first);
                if (de < 20.0) {
                    score *= 0.01;
                } else if (de < 45.0) {
                    score *= (de / 45.0) * 0.5;
                }
            }

            if (score > maxScore) { maxScore = score; bestIdx = i; }
        }
        if (bestIdx != -1 && maxScore > 0) {
            result.append({ candidates[bestIdx].color, (float)candidates[bestIdx].count / totalPixels });
            candidates.removeAt(bestIdx);
        } else break;
    }
    return result;
}

QVector<QPair<QColor, float>> ColorPaletteEngine::extractPaletteFromImage(const QImage& preScaledImage, int maxColors) {
    QVector<QPair<QColor, float>> result;
    if (preScaledImage.isNull()) return result;

    QImage img = preScaledImage;
    if (img.width() > 128 || img.height() > 128) {
        img = img.scaled(128, 128, Qt::KeepAspectRatio, Qt::FastTransformation);
    }
    img = img.convertToFormat(QImage::Format_RGB888);
    
    const int totalPixels = img.width() * img.height();
    if (totalPixels <= 0) return result;

    QMap<quint16, int> histogram;
    const uchar* bits = img.constBits();
    const int stride = img.bytesPerLine();

    for (int y = 0; y < img.height(); ++y) {
        const uchar* line = bits + y * stride;
        for (int x = 0; x < img.width(); ++x) {
            int r = line[x * 3 + 0] >> 3;
            int g = line[x * 3 + 1] >> 3;
            int b = line[x * 3 + 2] >> 3;
            quint16 key = (r << 10) | (g << 5) | b;
            histogram[key]++;
        }
    }

    QVector<QPair<int, quint16>> sortedBuckets;
    for (auto it = histogram.begin(); it != histogram.end(); ++it) {
        sortedBuckets.append({it.value(), it.key()});
    }
    std::sort(sortedBuckets.begin(), sortedBuckets.end(), std::greater<QPair<int, quint16>>());

    int count = std::min(maxColors, static_cast<int>(sortedBuckets.size()));
    for (int i = 0; i < count; ++i) {
        quint16 k = sortedBuckets[i].second;
        int r = ((k >> 10) & 0x1F) << 3;
        int g = ((k >> 5) & 0x1F) << 3;
        int b = (k & 0x1F) << 3;
        float ratio = static_cast<float>(sortedBuckets[i].first) / totalPixels;
        result.append({QColor(r, g, b), ratio});
    }

    return result;
}

QColor ColorPaletteEngine::quantizeToStandardColor(const QColor& color) {
    if (!color.isValid()) return QColor();

    static const QVector<QColor> standardPalette = {
        QColor("#E24B4A"), // 红色
        QColor("#EF9F27"), // 橙色
        QColor("#FECF0E"), // 黄色
        QColor("#639922"), // 绿色
        QColor("#1D9E75"), // 青色
        QColor("#378ADD"), // 蓝色
        QColor("#7F77DD"), // 紫色
        QColor("#5F5E5A")  // 灰色
    };

    double minDelta = 1e9;
    QColor closest = standardPalette.first();

    for (const auto& stdCol : standardPalette) {
        double d = calculateDeltaE(color, stdCol);
        if (d < minDelta) {
            minDelta = d;
            closest = stdCol;
        }
    }
    return closest;
}

ColorPaletteEngine::LabColor ColorPaletteEngine::rgbToLab(const QColor& rgb) {
    double r = rgb.redF();
    double g = rgb.greenF();
    double b = rgb.blueF();

    auto pivot = [](double n) {
        return (n > 0.04045) ? std::pow((n + 0.055) / 1.055, 2.4) : (n / 12.92);
    };
    r = pivot(r) * 100.0;
    g = pivot(g) * 100.0;
    b = pivot(b) * 100.0;

    double x = r * 0.4124 + g * 0.3576 + b * 0.1805;
    double y = r * 0.2126 + g * 0.7152 + b * 0.0722;
    double z = r * 0.0193 + g * 0.1192 + b * 0.9505;

    x /= 95.047;
    y /= 100.000;
    z /= 108.883;

    auto labPivot = [](double n) {
        return (n > 0.008856) ? std::cbrt(n) : (7.787 * n + 16.0 / 116.0);
    };
    double fx = labPivot(x);
    double fy = labPivot(y);
    double fz = labPivot(z);

    LabColor lab;
    lab.L = (116.0 * fy) - 16.0;
    lab.a = 500.0 * (fx - fy);
    lab.b = 200.0 * (fy - fz);
    return lab;
}

double ColorPaletteEngine::degToRad(double deg) { return deg * (M_PI / 180.0); }
double ColorPaletteEngine::radToDeg(double rad) { return rad * (180.0 / M_PI); }

double ColorPaletteEngine::calculateDeltaE(const QColor& c1, const QColor& c2) {
    LabColor l1 = rgbToLab(c1);
    LabColor l2 = rgbToLab(c2);

    double c_star1 = std::sqrt(l1.a * l1.a + l1.b * l1.b);
    double c_star2 = std::sqrt(l2.a * l2.a + l2.b * l2.b);
    double c_bar = (c_star1 + c_star2) / 2.0;

    double g = 0.5 * (1.0 - std::sqrt(std::pow(c_bar, 7) / (std::pow(c_bar, 7) + std::pow(25.0, 7))));
    double a1_prime = (1.0 + g) * l1.a;
    double a2_prime = (1.0 + g) * l2.a;

    double c1_prime = std::sqrt(a1_prime * a1_prime + l1.b * l1.b);
    double c2_prime = std::sqrt(a2_prime * a2_prime + l2.b * l2.b);

    auto computeHPrime = [](double a_p, double b) {
        if (a_p == 0.0 && b == 0.0) return 0.0;
        double deg = radToDeg(std::atan2(b, a_p));
        return (deg >= 0.0) ? deg : (deg + 360.0);
    };

    double h1_prime = computeHPrime(a1_prime, l1.b);
    double h2_prime = computeHPrime(a2_prime, l2.b);

    double delta_L_prime = l2.L - l1.L;
    double delta_C_prime = c2_prime - c1_prime;

    double delta_h_prime = 0.0;
    if (c1_prime * c2_prime != 0.0) {
        double diff = h2_prime - h1_prime;
        if (std::abs(diff) <= 180.0) delta_h_prime = diff;
        else if (diff > 180.0) delta_h_prime = diff - 360.0;
        else delta_h_prime = diff + 360.0;
    }
    double delta_H_prime = 2.0 * std::sqrt(c1_prime * c2_prime) * std::sin(degToRad(delta_h_prime / 2.0));

    double L_bar_prime = (l1.L + l2.L) / 2.0;
    double C_bar_prime = (c1_prime + c2_prime) / 2.0;

    double H_bar_prime = 0.0;
    if (c1_prime * c2_prime != 0.0) {
        double sum = h1_prime + h2_prime;
        double diff = std::abs(h1_prime - h2_prime);
        if (diff <= 180.0) H_bar_prime = sum / 2.0;
        else if (sum < 360.0) H_bar_prime = (sum + 360.0) / 2.0;
        else H_bar_prime = (sum - 360.0) / 2.0;
    }

    double T = 1.0 - 0.17 * std::cos(degToRad(H_bar_prime - 30.0))
                   + 0.24 * std::cos(degToRad(2.0 * H_bar_prime))
                   + 0.32 * std::cos(degToRad(3.0 * H_bar_prime + 6.0))
                   - 0.20 * std::cos(degToRad(4.0 * H_bar_prime - 63.0));

    double delta_theta = 30.0 * std::exp(-std::pow((H_bar_prime - 275.0) / 25.0, 2));
    double R_C = 2.0 * std::sqrt(std::pow(C_bar_prime, 7) / (std::pow(C_bar_prime, 7) + std::pow(25.0, 7)));
    double S_L = 1.0 + ((0.015 * std::pow(L_bar_prime - 50.0, 2)) / std::sqrt(20.0 + std::pow(L_bar_prime - 50.0, 2)));
    double S_C = 1.0 + 0.045 * C_bar_prime;
    double S_H = 1.0 + 0.015 * C_bar_prime * T;
    double R_T = -std::sin(degToRad(2.0 * delta_theta)) * R_C;

    double dE = std::sqrt(std::pow(delta_L_prime / S_L, 2) +
                          std::pow(delta_C_prime / S_C, 2) +
                          std::pow(delta_H_prime / S_H, 2) +
                          R_T * (delta_C_prime / S_C) * (delta_H_prime / S_H));
    return dE;
}

bool ColorPaletteEngine::matchesAutoColor(const QColor& target, const QColor& candidate, int precision) {
    if (!target.isValid() || !candidate.isValid()) return false;

    int targetSat = target.hsvSaturation();
    int pcSat = candidate.hsvSaturation();

    if (targetSat >= 38) {
        // 有彩色：要求 candidate 非灰黑白(饱和度 >= 38)，色相偏离 <= kHueTolerance，纯度符合精准度要求
        if (pcSat < 38) return false;

        int targetHue = target.hsvHue();
        int pcHue = candidate.hsvHue();
        if (targetHue < 0 || pcHue < 0) return false;

        int d = std::abs(pcHue - targetHue);
        if (d > 180) d = 360 - d;

        double precisionFactor = static_cast<double>(precision) / 100.0;

        // 1. 动态收紧色相差容差：精准度 0~100 对应色相差 15° ~ 6° (50时为 10°)
        int maxHueDiff = qRound(15.0 - 9.0 * precisionFactor);
        if (d > maxHueDiff) return false;

        // 2. 纯度要求：必须符合精准度最低饱和度阈值
        double minSat = 255.0 * (1.0 - precisionFactor);
        if (static_cast<double>(pcSat) < minSat) return false;

        // 3. 严格明度约束：精准度 0~100 对应最大明度差 40 ~ 15 (50时仅为 27)
        int maxValDiff = qRound(40.0 - 25.0 * precisionFactor);
        return std::abs(candidate.value() - target.value()) <= maxValDiff;
    } else {
        // 灰、黑、白：要求 candidate 亦属于灰黑白(饱和度 < 38)，且 lightnessF 差值 <= 0.2
        if (pcSat >= 38) return false;
        return std::abs(candidate.lightnessF() - target.lightnessF()) <= 0.2;
    }
}

float ColorPaletteEngine::matchedAreaRatio(const QColor& target, const QVector<QPair<QColor, float>>& palettes, const QString& autoColorHex, int precision) {
    if (!palettes.empty()) {
        float totalArea = 0.0f;
        for (const auto& pe : palettes) {
            if (matchesAutoColor(target, pe.first, precision)) {
                totalArea += pe.second;
            }
        }
        return totalArea;
    } else if (!autoColorHex.isEmpty()) {
        QColor recordCol = parseColorName(autoColorHex);
        if (matchesAutoColor(target, recordCol, precision)) {
            return 1.0f;
        }
    }
    return 0.0f;
}

} // namespace QuarkMeta
