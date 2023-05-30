//#include "myclassifier.h"
//#include <vector>
//#include <algorithm>
//#include <cmath>

//namespace{
//template <typename T>
//    static void softmax(T& input) {
//        float rowmax = *std::max_element(input.begin(), input.end());
//        std::vector<float> y(input.size());
//        float sum = 0.0f;
//        for (size_t i = 0; i != input.size(); ++i) {
//            sum += y[i] = std::exp(input[i] - rowmax);
//        }
//        for (size_t i = 0; i != input.size(); ++i) {
//            input[i] = y[i] / sum;
//        }
//    }
//}

//template<std::size_t N>
//MyClassifier<N>::MyClassifier(uint height, uint width):
//     height_{height}
//    ,width_{width}
//{
//    input_tensor_ = nullptr;
//    output_tensor_ = nullptr;

//}

//template<std::size_t N>
//MyClassifier<N>::~MyClassifier()
//{
//    delete mySession;
//}

//template<std::size_t N>
//void MyClassifier<N>::setNames(const char *input[], const char *output[])
//{
//    input_names_ = std::string(input);
//    output_names_ = std::string(output);
//}

//template<std::size_t N>
//bool MyClassifier<N>::loadModel(const std::string modelFilepath)
//{
//    Ort::Env env;
//    Ort::SessionOptions opt{nullptr};
//    mySession = new Ort::Session(env, modelFilepath.c_str(), opt );
//}

//template<std::size_t N>
//short MyClassifier<N>::predict(std::array<float, N> input_image_)
//{
//    char *input_names[] = {input_names_.c_str()};
//    char *output_names[] = {output_names_.c_str()};
//    std::array<int64_t, 4> input_shape_{1, 3, height_, width_};
//    std::array<int64_t, 2> output_shape_{1, 3};

//    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
//    input_tensor_ = Ort::Value::CreateTensor<float>(memory_info, input_image_.data(), input_image_.size(),
//                                                    input_shape_.data(), input_shape_.size());
//    output_tensor_ = Ort::Value::CreateTensor<float>(memory_info, results_.data(), results_.size(),
//                                                     output_shape_.data(), output_shape_.size());
//    short result_{0};
//    Ort::RunOptions run_options;
//    mySession->Run(run_options, input_names, &input_tensor_, 1, output_names, &output_tensor_, 1);
//    softmax(results_);
//    result_ = std::distance(results_.begin(), std::max_element(results_.begin(), results_.end()));
//}
