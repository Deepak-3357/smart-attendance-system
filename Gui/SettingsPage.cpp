#include "SettingsPage.h"
#include "ui_SettingsPage.h"
#include "ThemeManager.h"
#include "../Recognition/RecognitionUtils.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
SettingsPage::SettingsPage(ThemeManager*t,QWidget*p):QWidget(p),ui(new Ui::SettingsPage),themes_(t){ui->setupUi(this);for(int i=0;i<3;++i)ui->cameraCombo->addItem(QString("Camera %1").arg(i),i);QSettings s;ui->cameraCombo->setCurrentIndex(s.value("camera/index",0).toInt());ui->themeCombo->setCurrentText(s.value("appearance/theme","Dark").toString());ui->thresholdSpin->setValue(s.value("recognition/threshold",45.0).toDouble());ui->backupEdit->setText(s.value("backup/folder").toString());ui->autoBackupCheck->setChecked(s.value("backup/automatic",false).toBool());connect(ui->browseButton,&QPushButton::clicked,this,&SettingsPage::browse);connect(ui->saveButton,&QPushButton::clicked,this,&SettingsPage::save);}
SettingsPage::~SettingsPage(){delete ui;}void SettingsPage::browse(){const QString d=QFileDialog::getExistingDirectory(this,"Select backup folder",ui->backupEdit->text());if(!d.isEmpty())ui->backupEdit->setText(d);}void SettingsPage::save(){QSettings s;s.setValue("camera/index",ui->cameraCombo->currentData());s.setValue("recognition/threshold",ui->thresholdSpin->value());s.setValue("backup/folder",ui->backupEdit->text());s.setValue("backup/automatic",ui->autoBackupCheck->isChecked());RecognitionUtils::setConfidenceThreshold(ui->thresholdSpin->value());themes_->apply(ui->themeCombo->currentText()=="Light"?ThemeManager::Theme::Light:ThemeManager::Theme::Dark);QMessageBox::information(this,"Settings","Preferences saved.");}
