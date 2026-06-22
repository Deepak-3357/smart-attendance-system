#pragma once
#include <QObject>
#include <QImage>
#include <QTimer>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <map>
#include <set>
#include "../Recognition/LivenessDetector.h"
class StudentManager; class AttendanceManager; class FaceRecognitionManager;
class CameraManager : public QObject {
 Q_OBJECT
public:
 enum class Mode{Preview,Dataset,Attendance}; Q_ENUM(Mode)
 explicit CameraManager(StudentManager* students,AttendanceManager* attendance,FaceRecognitionManager* recognition,QObject* parent=nullptr);
 ~CameraManager() override;
 bool start(int cameraIndex,Mode mode,const QString& registration=QString());
 void stop(); bool isRunning() const{return camera_.isOpened();}
signals:
 void frameReady(const QImage& frame); void errorOccurred(const QString& message); void captureProgress(int count,int total);
 void captureFinished(bool ok,const QString& message); void recognitionUpdated(const QString& name,const QString& regNo,int accuracy,const QString& liveness);
 void attendanceMarked(const QString& name,const QString& regNo,const QString& time);
private slots: void processFrame();
private:
 QImage toImage(const cv::Mat& bgr) const; cv::VideoCapture camera_;cv::CascadeClassifier faceDetector_;QTimer timer_;Mode mode_=Mode::Preview;QString regNo_;int captured_=0;int frameGap_=0;std::map<int,int> stableVotes_;std::set<std::string> marked_;int unknownCooldown_=0;
 StudentManager* students_;AttendanceManager* attendance_;FaceRecognitionManager* recognition_;LivenessDetector liveness_;
};
