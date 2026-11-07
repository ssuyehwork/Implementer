#pragma once

#include <QApplication>

namespace QuarkMeta {

class QuarkApplication : public QApplication {
    Q_OBJECT
public:
    QuarkApplication(int& argc, char** argv);
    ~QuarkApplication() override = default;

    bool notify(QObject* receiver, QEvent* event) override;

private:
    void performGlobalSelfHealing(QWidget* widget);
};

} // namespace QuarkMeta
