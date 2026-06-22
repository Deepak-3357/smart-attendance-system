#include "CameraManager.h"
#include "FaceRecognitionManager.h"
#include "../Student.h"
#include "../Attendance.h"
#include "../Recognition/RecognitionUtils.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <filesystem>
#include <QSettings>
CameraManager::CameraManager(StudentManager* s,AttendanceManager* a,FaceRecognitionManager* r,QObject* p):QObject(p),students_(s),attendance_(a),recognition_(r),liveness_("haarcascade_eye.xml"){
 connect(&timer_,&QTimer::timeout,this,&CameraManager::processFrame);timer_.setInterval(30);
}
CameraManager::~CameraManager(){stop();}
bool CameraManager::start(int index,Mode mode,const QString& reg){
 stop();if(!faceDetector_.load("haarcascade_frontalface_default.xml")){emit errorOccurred("Could not load Haar face cascade.");return false;}if(!camera_.open(index)){emit errorOccurred(QString("Could not open camera %1.").arg(index));return false;}mode_=mode;regNo_=reg;captured_=0;frameGap_=0;stableVotes_.clear();marked_.clear();timer_.start();return true;
}
void CameraManager::stop(){timer_.stop();if(camera_.isOpened())camera_.release();}
QImage CameraManager::toImage(const cv::Mat& bgr)const{cv::Mat rgb;cv::cvtColor(bgr,rgb,cv::COLOR_BGR2RGB);return QImage(rgb.data,rgb.cols,rgb.rows,static_cast<int>(rgb.step),QImage::Format_RGB888).copy();}
void CameraManager::processFrame(){
 cv::Mat frame;if(!camera_.read(frame)||frame.empty())return;cv::Mat gray;cv::cvtColor(frame,gray,cv::COLOR_BGR2GRAY);cv::equalizeHist(gray,gray);std::vector<cv::Rect> faces;faceDetector_.detectMultiScale(gray,faces,1.1,5,0,{80,80});std::set<int> seen;
 for(const auto& face:faces){cv::Mat crop=gray(face);cv::Mat normalized;cv::resize(crop,normalized,{200,200});cv::Scalar color(37,99,235);QString label="Face detected";
  if(mode_==Mode::Dataset){label="Hold still";if(++frameGap_>=3&&captured_<100){auto quality=RecognitionUtils::checkFaceQuality(normalized);if(quality.accepted){std::filesystem::create_directories("faces");++captured_;cv::imwrite("faces/"+regNo_.toStdString()+"."+std::to_string(captured_)+".jpg",normalized);emit captureProgress(captured_,100);}frameGap_=0;}if(captured_>=100){emit captureFinished(true,"100 face samples captured.");stop();}}
  else if(mode_==Mode::Attendance&&recognition_&&recognition_->isLoaded()){
   auto result=recognition_->predict(normalized);Student student;const bool known=result.first>=0&&result.second>0&&result.second<RecognitionUtils::getConfidenceThreshold()&&students_->getStudentByRegNo(std::to_string(result.first),student);
   if(known){seen.insert(result.first);int& votes=stableVotes_[result.first];++votes;const int accuracy=RecognitionUtils::confidenceToAccuracy(result.second);QSettings().setValue("recognition/lastAccuracy",accuracy);QString live="Stabilizing";if(votes>=10){const bool verified=liveness_.update(result.first,normalized);live=QString::fromStdString(liveness_.statusText(result.first));color=verified?cv::Scalar(34,197,94):cv::Scalar(234,179,8);if(verified&&!marked_.count(student.regNo)){if(attendance_->markPresent(student)){marked_.insert(student.regNo);emit attendanceMarked(QString::fromStdString(student.name),QString::fromStdString(student.regNo),QString::fromStdString(AttendanceManager::currentTime()));}}}label=QString::fromStdString(student.name);emit recognitionUpdated(label,QString::fromStdString(student.regNo),accuracy,live);
   }else{color=cv::Scalar(220,38,38);label="Unknown person";if(unknownCooldown_<=0){RecognitionUtils::saveUnknownFace(normalized);unknownCooldown_=60;}}
  }
  cv::rectangle(frame,face,color,2);cv::putText(frame,label.toStdString(),{face.x,std::max(25,face.y-8)},cv::FONT_HERSHEY_SIMPLEX,.65,color,2);
 }
 for(auto& vote:stableVotes_)if(!seen.count(vote.first))vote.second=0;if(unknownCooldown_>0)--unknownCooldown_;emit frameReady(toImage(frame));
}
