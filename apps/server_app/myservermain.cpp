#include "myservermain.h"
#include "ui_myservermain.h"
#include "onnxruntime_cxx_api.h"
#include "messageheaders.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QFileDialog>
#include <QDebug>
#include <cmath>

namespace{
const std::vector<std::string> classes {"Car", "Drone", "Human"};
const int framesCount_{3};
const int width_{61};
const int height_{11};
const int size_{framesCount_ * width_ * height_};
    bool readTensorFromFile(QString path, std::vector<float> &input_image_)
    {
        QDir dir(path);
        QStringList files = dir.entryList();
        files.removeFirst();
        files.removeFirst();
        for(uint i=0; i<3; ++i){
            QFile file(path + QDir::separator() + files[i]);
            if (!file.open(QIODevice::ReadOnly)) {
                qDebug() << file.errorString();
                return false;
            }
            QStringList wordList;
            while (!file.atEnd()) {
                QByteArray line = file.readLine();
                wordList = (QString(line).trimmed().split(','));
                for(auto &word: wordList){
                    input_image_.push_back(word.toFloat());
                }
            }
        }
        return true;
    }
}

MyServerMain::MyServerMain(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyServerMain)
    , serverTcp{nullptr}
    , clientConnection{nullptr}
    , classificator {new MyClassifier(height_, width_, framesCount_)}
    , message{new MessageProcessor}
{
    ui->setupUi(this);
    classificator->setNames("modelInput", "modelOutput");
    classificator->loadModel(ui->leReadModel->text().toStdString());
    connect(ui->tbReadModel, &QToolButton::clicked, this, &MyServerMain::choseModelFile);
    connect(ui->pbReadModel, &QPushButton::clicked, this, [this](){emit testSend(object);});
    connect(this, &MyServerMain::testSend, this, &MyServerMain::readModelFile);
    connect(ui->pbStartServer, &QPushButton::clicked, this, &MyServerMain::startServer);
    connect(ui->pbStopServer, &QPushButton::clicked, this, &MyServerMain::stopServer);
    connect(message, &MessageProcessor::stageEnded, this, &MyServerMain::readModelFile);
}

MyServerMain::~MyServerMain()
{
    serverTcp->close();
    delete serverTcp;
    delete message;
    delete ui;
}

void MyServerMain::startServer()
{
    serverTcp = new QTcpServer;
    connect(serverTcp, &QTcpServer::newConnection, this, [this]()
    {
        if (clientConnection != nullptr) {
            return;
//            clientConnection->close();
//            delete clientConnection;
        }
        clientConnection = serverTcp->nextPendingConnection();
        ui->lwServer->addItem("New TCP connection from: "
                                + clientConnection->peerAddress().toString()
                                + " port: "+ QString::number(clientConnection->peerPort()));
        connect(clientConnection,
                &QTcpSocket::readyRead,
                this,
                &MyServerMain::reciveTcpMsg);
        connect(clientConnection, &QTcpSocket::disconnected, this, [this]()
        {
            qDebug() << "here";
            clientConnection->flush();
            clientConnection->close();
        });

    });


    int port = ui->sbPort->value();
    if(!serverTcp->listen(QHostAddress::LocalHost, port )) {
        qDebug() << "Server could not start";
    } else {
        ui->lwServer->addItem("Server started! adress: "
                              + QHostAddress(QHostAddress::LocalHost).toString()
                              + " Port: "
                              + QString::number(port));
    }
}

void MyServerMain::stopServer()
{
//    QString adr = clientConnection->peerAddress().toString();
//    QString port = QString::number(clientConnection->peerPort());
//    clientConnection->close();
//    clientConnection = nullptr;
//    ui->lwServer->addItem("Connection from: "
//                          + adr
//                          + " port: "
//                          + port);
    if(!serverTcp)
        return;
    serverTcp->close();
    delete serverTcp;
    serverTcp = nullptr;
    ui->lwServer->addItem("Server is stoped");
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

    object = message->objectImage;
    if(object.size() == 0) {
        readTensorFromFile("../../data/People/12-21f/", object);
    }
    short result = classificator->predict(object);
    QString msg =  QString::fromStdString(classes[result]);
    ui->lwServer->addItem("The class is: " + msg);
//    emit makeResponseMessage(result);
}

void MyServerMain::reciveTcpMsg()
{
    rawMessageBuffer += clientConnection->readAll();
    bool msgIsReady = message->checkTcpMessageLen(rawMessageBuffer);
    qDebug() << msgIsReady;
    if(msgIsReady){
        message->rawMessageParser(rawMessageBuffer);
        rawMessageBuffer.clear();
    }

}

void MyServerMain::startClassification(const std::vector<float> &object)
{
    short result  = classificator->predict(object);
    QString msg =  QString::fromStdString(classes[result]);
    ui->lwServer->addItem("The class is: " + msg);
}
