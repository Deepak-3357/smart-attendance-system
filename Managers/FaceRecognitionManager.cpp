#include "FaceRecognitionManager.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <QDir>
#include <QFileInfo>
#include <filesystem>
FaceRecognitionManager::FaceRecognitionManager(QObject* p):QObject(p){}
bool FaceRecognitionManager::loadModel(const QString& path){
 if(!QFileInfo::exists(path)) return false;
 try{model_=cv::face::LBPHFaceRecognizer::create();model_->read(path.toStdString());return true;}
 catch(const cv::Exception&){model_.release();return false;}
}
QPair<int,double> FaceRecognitionManager::predict(const cv::Mat& face) const{
 if(model_.empty()||face.empty()) return {-1,100.0}; cv::Mat resized;cv::resize(face,resized,{200,200});int label=-1;double confidence=100.;model_->predict(resized,label,confidence);return {label,confidence};
}
void FaceRecognitionManager::train(const QString& facesDir,const QString& modelPath){
 emit logMessage("Scanning face dataset..."); QDir dir(facesDir);const auto files=dir.entryInfoList({"*.jpg","*.jpeg","*.png"},QDir::Files);std::vector<cv::Mat> images;std::vector<int> labels;int done=0;
 for(const QFileInfo& f:files){bool ok=false;int label=f.completeBaseName().section('.',0,0).toInt(&ok);cv::Mat image=cv::imread(f.absoluteFilePath().toStdString(),cv::IMREAD_GRAYSCALE);if(ok&&!image.empty()){cv::resize(image,image,{200,200});images.push_back(image);labels.push_back(label);}emit trainingProgress(files.isEmpty()?0:(++done*75/files.size()));}
 if(images.empty()){emit trainingFinished(false,"No valid dataset images were found.",0);return;}
 try{emit logMessage(QString("Training LBPH with %1 images...").arg(images.size()));model_=cv::face::LBPHFaceRecognizer::create();model_->train(images,labels);std::filesystem::create_directories(QFileInfo(modelPath).absolutePath().toStdString());emit trainingProgress(90);model_->save(modelPath.toStdString());emit trainingProgress(100);emit logMessage("Model saved successfully.");emit trainingFinished(true,"Training completed successfully.",static_cast<int>(images.size()));}
 catch(const cv::Exception& e){emit trainingFinished(false,QString::fromStdString(e.what()),static_cast<int>(images.size()));}
}
