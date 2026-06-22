#pragma once
#include <QWidget>
class StudentManager;class CameraManager;namespace Ui{class DatasetCollectionPage;}
class DatasetCollectionPage:public QWidget{
 Q_OBJECT
public:DatasetCollectionPage(StudentManager* students,CameraManager* camera,QWidget* parent=nullptr);~DatasetCollectionPage()override;
public slots:void refreshStudents();
private slots:void start();void stop();void showFrame(const QImage&);void progress(int,int);void finished(bool,const QString&);
private:Ui::DatasetCollectionPage* ui;StudentManager* students_;CameraManager* camera_;};
