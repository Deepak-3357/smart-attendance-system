#pragma once
#include <QWidget>
class CameraManager;class FaceRecognitionManager;namespace Ui{class AttendancePage;}
class AttendancePage:public QWidget{
 Q_OBJECT
public:AttendancePage(CameraManager* camera,FaceRecognitionManager* recognition,QWidget* parent=nullptr);~AttendancePage()override;
signals:void attendanceChanged();
private slots:void start();void stop();void showFrame(const QImage&);void updateResult(const QString&,const QString&,int,const QString&);void marked(const QString&,const QString&,const QString&);
private:Ui::AttendancePage* ui;CameraManager* camera_;FaceRecognitionManager* recognition_;};
