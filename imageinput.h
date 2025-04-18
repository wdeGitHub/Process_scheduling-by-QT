#ifndef IMAGEINPUT_H
#define IMAGEINPUT_H

#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>
#include <string>

class ImageInput
{
public:
    ImageInput();
    
    // 加载并预处理图像
    cv::Mat preprocessImage(const std::string& imagePath);
    
    // 识别图像中的数字
    int recognizeDigit(const cv::Mat& image);
    
    // 训练模型
    void trainModel(const std::string& trainingDataPath);
    
private:
    cv::Ptr<cv::ml::KNearest> knn;  // KNN分类器
    
    // 图像预处理辅助函数
    cv::Mat resizeImage(const cv::Mat& image);
    cv::Mat thresholdImage(const cv::Mat& image);
};

#endif // IMAGEINPUT_H
