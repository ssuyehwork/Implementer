#pragma once
#include <QTextEdit>
#include <QKeyEvent>
#include <QResizeEvent>

namespace QuarkMeta {

class ElasticEdit : public QTextEdit {
    Q_OBJECT
public:
    explicit ElasticEdit(QWidget* parent = nullptr);

    bool hasHeightForWidth() const override;
    int heightForWidth(int w) const override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void returnPressed();

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void changeEvent(QEvent* e) override;
};

} // namespace QuarkMeta
