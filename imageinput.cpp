#include "imageinput.h"

ImageInput::ImageInput() {
    knn = cv::ml::KNearest::create();
}

cv::Mat ImageInput::preprocessImage(const std::string& imagePath) {
    // 读取图像
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    if(image.empty()) {
        throw std::runtime_error("无法加载图像");
    }
    
    // 预处理
    cv::Mat processed = resizeImage(image);
    processed = thresholdImage(processed);
    return processed;
}

cv::Mat ImageInput::resizeImage(const cv::Mat& image) {
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(28, 28)); // MNIST标准大小
    return resized;
}

cv::Mat ImageInput::thresholdImage(const cv::Mat& image) {
    cv::Mat thresh;
    cv::threshold(image, thresh, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    return thresh;
}

int ImageInput::recognizeDigit(const cv::Mat& image) {
    // 确保图像已经过预处理
    cv::Mat sample = image.reshape(1, 1);
    sample.convertTo(sample, CV_32F);
    
    // 使用KNN进行预测
    cv::Mat results;
    float response = knn->predict(sample, results);
    return static_cast<int>(response);
}

void ImageInput::trainModel(const std::string& trainingDataPath) {
    // 加载MNIST训练数据
    cv::Mat trainingImages = cv::imread(trainingDataPath, cv::IMREAD_GRAYSCALE);
    if(trainingImages.empty()) {
        throw std::runtime_error("无法加载训练数据");
    }
    
    // 准备训练数据
    cv::Mat trainingData;
    trainingImages.reshape(1, trainingImages.rows).convertTo(trainingData, CV_32F);
    
    // 准备标签（这里假设标签是已知的，实际使用时需要提供正确的标签）
    cv::Mat labels(trainingImages.rows, 1, CV_32S);
    
    // 训练KNN模型
    knn->train(trainingData, cv::ml::ROW_SAMPLE, labels);
}
