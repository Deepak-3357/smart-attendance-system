#pragma once
#include <QObject>
#include <opencv2/face.hpp>
#include <opencv2/core.hpp>
class FaceRecognitionManager : public QObject {
 Q_OBJECT
public:
 explicit FaceRecognitionManager(QObject* parent=nullptr);
 bool loadModel(const QString& path="trainer/trainer.yml");
 bool isLoaded() const { return !model_.empty(); }
 QPair<int,double> predict(const cv::Mat& grayFace) const;
public slots: void train(const QString& facesDir="faces",const QString& modelPath="trainer/trainer.yml");
signals: void trainingProgress(int value); void logMessage(const QString& text); void trainingFinished(bool ok,const QString& message,int imageCount);
private: cv::Ptr<cv::face::LBPHFaceRecognizer> model_;
};
