#include "NotificationWidget.h"
#include <QTimer>
NotificationWidget::NotificationWidget(QWidget* p):QLabel(p){hide();setAlignment(Qt::AlignCenter);}
void NotificationWidget::showMessage(const QString& text,bool error){setText(text);setStyleSheet(QString("padding:10px;border-radius:7px;color:white;background:%1;").arg(error?"#dc2626":"#16a34a"));show();raise();QTimer::singleShot(3500,this,&QWidget::hide);}
