#pragma once
#include <QWidget>
class ThemeManager;namespace Ui{class SettingsPage;}
class SettingsPage:public QWidget{
 Q_OBJECT
public:explicit SettingsPage(ThemeManager*,QWidget* parent=nullptr);~SettingsPage()override;
private slots:void save();void browse();
private:Ui::SettingsPage* ui;ThemeManager* themes_;};
