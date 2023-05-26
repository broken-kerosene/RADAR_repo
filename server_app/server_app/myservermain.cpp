#include "myservermain.h"
#include "ui_myservermain.h"
#include "onnxruntime_cxx_api.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QFileDialog>
#include <QDebug>
#include <cmath>

namespace{
const int width_{61};
const int height_{11};
const int size_{3 * width_ * height_};
    bool readTensorFromFile(QString path, std::array<float, size_> &input_image_)
    {
        QDir dir(path);
        QStringList files = dir.entryList();
        for(uint i=2; i<5; ++i){
            QFile file(path + QDir::separator() + files[i]);
            if (!file.open(QIODevice::ReadOnly)) {
                qDebug() << file.errorString();
                return false;
            }
            QStringList wordList;
            int j=0;
            while (!file.atEnd()) {
                QByteArray line = file.readLine();
                wordList = (QString(line).trimmed().split(','));
                for(auto &word: wordList){
                input_image_[i] = word.toFloat();
                ++j;
                }
            }
        }
        return true;
    }

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
MyServerMain::MyServerMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyServerMain)
    , serverTcp{new QTcpServer(this)}
    , clientConnection{nullptr}
{
    ui->setupUi(this);
    connect(ui->tbReadModel, &QToolButton::clicked, this, &MyServerMain::choseModelFile);
    connect(ui->pbReadModel, &QPushButton::clicked, this, &MyServerMain::readModelFile);

}

MyServerMain::~MyServerMain()
{
    delete ui;
}

void MyServerMain::startServer()
{
    connect(serverTcp, &QTcpServer::newConnection, this, [this]()
    {
        if (clientConnection != nullptr) {
            clientConnection->close();
            delete clientConnection;
        }
        clientConnection = serverTcp->nextPendingConnection();
        ui->lwServer->addItem("New TCP connection from: "
                                + clientConnection->peerAddress().toString()
                                + " port: "+ QString::number(clientConnection->peerPort()));
        connect(clientConnection,
                &QTcpSocket::readyRead,
                this,
                &MyServerMain::reciveTcpMsg);
    });
}

void MyServerMain::stopServer()
{

}

void MyServerMain::choseModelFile()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("model (*.onnx)"));
    if (dialog.exec()) {
        ui->leReadModel->setText(dialog.selectedFiles().first());
    }
}

void MyServerMain::readModelFile()
{
    /*
     * https://github.com/microsoft/onnxruntime-inference-examples/blob/main/c_cxx/MNIST/MNIST.cpp
     */
    std::array<float, size_> input_image_{};
    std::array<float, 3> results_{};
    int64_t result_{0};
    readTensorFromFile("/home/kerosene/study/RADAR_repo/data/Drones/12-34/", input_image_);

    Ort::Value input_tensor_{nullptr};
    std::array<int64_t, 4> input_shape_{1, 3, height_, width_};

    Ort::Value output_tensor_{nullptr};
    std::array<int64_t, 2> output_shape_{1, 3};

    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
    input_tensor_ = Ort::Value::CreateTensor<float>(memory_info, input_image_.data(), input_image_.size(),
                                                    input_shape_.data(), input_shape_.size());
    output_tensor_ = Ort::Value::CreateTensor<float>(memory_info, results_.data(), results_.size(),
                                                     output_shape_.data(), output_shape_.size());
    Ort::Env env;
    std::string model_path = ui->leReadModel->text().toStdString();
    Ort::Session mysession(env, model_path.c_str(), Ort::SessionOptions{ nullptr });

    const char* input_names[] = {"modelInput"};
    const char* output_names[] = {"modelOutput"};

    Ort::RunOptions run_options;
    mysession.Run(run_options, input_names, &input_tensor_, 1, output_names, &output_tensor_, 1);
//    softmax(results_);
//    result_ = std::distance(results_.begin(), std::max_element(results_.begin(), results_.end()));

    qDebug() << result_;
}

void MyServerMain::reciveTcpMsg()
{
    rawMessageBuffer += clientConnection->readAll();
    bool msgIsReady = message->checkTcpMessageLen(rawMessageBuffer);

}
