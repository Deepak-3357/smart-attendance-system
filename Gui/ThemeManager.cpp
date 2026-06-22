#include "ThemeManager.h"
#include <QApplication>
#include <QPalette>
#include <QSettings>
#include <QStyle>
ThemeManager::ThemeManager(QApplication* app) : QObject(app), app_(app) {
 apply(QSettings().value("appearance/theme", "Dark").toString() == "Light" ? Theme::Light : Theme::Dark);
}
void ThemeManager::apply(Theme theme) {
 theme_ = theme; QPalette p;
 if (theme == Theme::Dark) {
  p.setColor(QPalette::Window,QColor("#111827")); p.setColor(QPalette::WindowText,Qt::white);
  p.setColor(QPalette::Base,QColor("#0f172a")); p.setColor(QPalette::AlternateBase,QColor("#1f2937"));
  p.setColor(QPalette::Text,QColor("#e5e7eb")); p.setColor(QPalette::Button,QColor("#1f2937"));
  p.setColor(QPalette::ButtonText,Qt::white); p.setColor(QPalette::Highlight,QColor("#2563eb"));
 } else { p=app_->style()->standardPalette(); p.setColor(QPalette::Highlight,QColor("#2563eb")); }
 app_->setPalette(p);
 app_->setStyleSheet("QWidget{font-family:'Segoe UI';font-size:10pt;}QFrame#card{border:1px solid palette(mid);border-radius:12px;background:palette(base);}QPushButton{padding:8px 14px;border-radius:7px;background:#2563eb;color:white;border:0;}QPushButton:hover{background:#1d4ed8;}QPushButton:disabled{background:#64748b;}QLineEdit,QComboBox,QDateEdit,QDoubleSpinBox{padding:7px;border:1px solid palette(mid);border-radius:6px;}QTableView{border:0;gridline-color:palette(mid);selection-background-color:#2563eb;}QHeaderView::section{padding:8px;border:0;border-bottom:1px solid palette(mid);font-weight:600;}QProgressBar{border:0;border-radius:5px;text-align:center;background:palette(midlight);}QProgressBar::chunk{border-radius:5px;background:#22c55e;}");
 QSettings().setValue("appearance/theme",theme==Theme::Dark?"Dark":"Light"); emit themeChanged(theme);
}
