#pragma once
#include <QLabel>
class NotificationWidget : public QLabel {
 Q_OBJECT
public: explicit NotificationWidget(QWidget* parent=nullptr); void showMessage(const QString& text,bool error=false); };
