#pragma once
#include <QObject>
class QApplication;
class ThemeManager : public QObject {
 Q_OBJECT
public:
 enum class Theme { Light, Dark };
 explicit ThemeManager(QApplication* app);
 void apply(Theme theme);
 Theme current() const { return theme_; }
signals: void themeChanged(Theme theme);
private: QApplication* app_; Theme theme_ = Theme::Dark;
};
