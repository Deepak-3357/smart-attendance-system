#include "TrainingPage.h"
#include "ui_TrainingPage.h"
#include "../Managers/FaceRecognitionManager.h"
#include <QtConcurrent>
#include <QFileInfo>
#include <QDateTime>
#include <QMessageBox>
TrainingPage::TrainingPage(FaceRecognitionManager*m,QWidget*p):QWidget(p),ui(new Ui::TrainingPage),manager_(m){ui->setupUi(this);connect(ui->trainButton,&QPushButton::clicked,this,&TrainingPage::startTraining);connect(manager_,&FaceRecognitionManager::trainingProgress,ui->trainingProgress,&QProgressBar::setValue);connect(manager_,&FaceRecognitionManager::logMessage,ui->logEdit,&QPlainTextEdit::appendPlainText);connect(manager_,&FaceRecognitionManager::trainingFinished,this,&TrainingPage::finished);inspectModel();}
TrainingPage::~TrainingPage(){delete ui;}void TrainingPage::startTraining(){ui->trainButton->setEnabled(false);ui->trainingProgress->setValue(0);ui->logEdit->appendPlainText("Starting training job...");QtConcurrent::run([this]{manager_->train();});}
void TrainingPage::finished(bool ok,const QString&m,int count){ui->trainButton->setEnabled(true);ui->logEdit->appendPlainText(m);inspectModel();if(ok){manager_->loadModel();emit modelUpdated();QMessageBox::information(this,"Training complete",QString("%1\n%2 images used.").arg(m).arg(count));}else QMessageBox::critical(this,"Training failed",m);}
void TrainingPage::inspectModel(){QFileInfo f("trainer/trainer.yml");ui->modelInfo->setText(f.exists()?QString("%1 KB • updated %2").arg(f.size()/1024.0,0,'f',1).arg(f.lastModified().toString("dd MMM yyyy, hh:mm")):"No trained model found");}
