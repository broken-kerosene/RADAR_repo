#ifndef MYCLASSIFIER_H
#define MYCLASSIFIER_H

#include "myclassifier.h"
#include <vector>
#include <algorithm>
#include <cmath>

#include "onnxruntime_cxx_api.h"
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

namespace{
template <typename T>
    static void softmax(T& input) {
        float rowmax = *std::max_element(input.begin(), input.end());
        std::vector<float> y(input.size());
        float sum = 0.0f;
        for (size_t i = 0; i != input.size(); ++i) {
            sum += y[i] = std::exp(input[i] - rowmax);
        }
        for (size_t i = 0; i != input.size(); ++i) {
            input[i] = y[i] / sum;
        }
    }
}

MyClassifier::MyClassifier(uint height, uint width, uint count):
      count_ {count}
    , height_{height}
    , width_{width}
    , input_tensor_{nullptr}
    , output_tensor_{nullptr}
{
    size_ = count_ * height_ * width_;
}

MyClassifier::~MyClassifier()
{
    delete mySession;
}

void MyClassifier::setNames(const std::string input, const std::string output)
{
    input_names_ = input;
    output_names_ = output;
}

bool MyClassifier::loadModel(const std::string modelFilepath)
{
    Ort::Env env;
    Ort::SessionOptions opt{nullptr};
    mySession = new Ort::Session(env, modelFilepath.c_str(), opt );
}

short MyClassifier::predict(const std::vector<float> &input_image)
{
    const uint size = 2013;
    std::array<float, size> input_image_;
    char *input_names[] = input_names_.c_str();
//    char *output_names[] = {output_names_.c_str()};
    std::array<int64_t, 4> input_shape_{1, 3, height_, width_};
    std::array<int64_t, 2> output_shape_{1, 3};

    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    input_tensor_ = Ort::Value::CreateTensor<float>(memory_info, input_image_.data(), size_,
                                                    input_shape_.data(), input_shape_.size());
    output_tensor_ = Ort::Value::CreateTensor<float>(memory_info, results_.data(), results_.size(),
                                                     output_shape_.data(), output_shape_.size());
    short result_{0};
    Ort::RunOptions run_options;
    mySession->Run(run_options, input_names, &input_tensor_, 1, output_names_.c_str(), &output_tensor_, 1);
    softmax(results_);
    result_ = std::distance(results_.begin(), std::max_element(results_.begin(), results_.end()));
}
#endif // MYCLASSIFIER_H
