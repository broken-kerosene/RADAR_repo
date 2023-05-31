#ifndef MYCLASSIFIER_H
#define MYCLASSIFIER_H

#include "onnxruntime_cxx_api.h"
#include <vector>
#include <string>

class MyClassifier
{
public:
    explicit MyClassifier(uint height, uint width, uint count);
    ~MyClassifier();
    void setNames(const std::string input, const std::string output);
    bool loadModel(const std::string modelFilepath);
    short predict(const std::vector<float> &input_image);

private:
    uint count_;
    uint height_;
    uint width_;
    uint size_;
    std::string input_names_;
    std::string output_names_;
    Ort::Value input_tensor_;
    Ort::Value output_tensor_;
    std::array<float, 3> results_;
    Ort::Session *mySession;
};

#endif // MYCLASSIFIER_H
