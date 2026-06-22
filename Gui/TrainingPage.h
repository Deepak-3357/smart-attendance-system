#pragma once
#include <QWidget>
class FaceRecognitionManager;namespace Ui{class TrainingPage;}
class TrainingPage:public QWidget{
 Q_OBJECT
public:explicit TrainingPage(FaceRecognitionManager* manager,QWidget* parent=nullptr);~TrainingPage()override;
signals:void modelUpdated();
private slots:void startTraining();void finished(bool,const QString&,int);void inspectModel();
private:Ui::TrainingPage* ui;FaceRecognitionManager* manager_;};
