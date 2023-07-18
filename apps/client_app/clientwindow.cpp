#include "clientwindow.h"
#include "ui_clientwindow.h"
#include "messageheaders.h"

#include <QFileDialog>
#include <QTcpSocket>
#include <QHostAddress>

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
        for(uint i=0; i<3; ++i){ QFile file(path + QDir::separator() + files[i]);
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

clientWindow::clientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::clientWindow)
    , socket{nullptr}
    , tag{0}
{
    ui->setupUi(this);
    connect(ui->tbReadFile, &QToolButton::clicked, this, &clientWindow::choseFilePath);
    connect(ui->tbReadModelFile, &QToolButton::clicked, this, &clientWindow::choseModelFilePath);
    connect(ui->pbConnect, &QPushButton::clicked, this, &clientWindow::makeConnection);
    connect(ui->pbDisconnect, &QPushButton::clicked, this, &clientWindow::makeDisconnection);
    connect(ui->pbClassification, &QPushButton::clicked, this, &clientWindow::classificate);
    connect(ui->pbSendModel, &QPushButton::clicked, this, &clientWindow::sendModel);
}

clientWindow::~clientWindow()
{
    makeDisconnection();
    delete ui;
}

void clientWindow::choseFilePath()
{
    QString outputFolder = QFileDialog::getExistingDirectory(0,
                                                             ("Select Output Folder"),
                                                             QDir::currentPath());
    ui->leFilename->setText(outputFolder);
}

void clientWindow::choseModelFilePath()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("model (*.onnx)"));
    if (dialog.exec()) {
        ui->leModelFile->setText(dialog.selectedFiles().first());
    }

}

void clientWindow::readClassFile()
{
    inputImage.clear();
    QString filePath = ui->leFilename->text();
    readTensorFromFile(filePath, inputImage);
    ui->lwMessages->addItem("Files have been read");
}

void clientWindow::makeConnection()
{
    if(socket){
        socket->flush();
        socket->close();
        delete socket;
    }
    socket = new QTcpSocket;
    QHostAddress adress(ui->leAdress->text());
    int port{ui->sbPort->value()};

    bool ok = socket->bind(QHostAddress::LocalHost, 6117);

    if(ok){
        socket->connectToHost(adress, port);
        ui->lwMessages->addItem("Connected to: " + adress.toString() + " Port: " + QString::number(port));
        connect(socket, &QTcpSocket::disconnected, this, [this](){qDebug() << "socket is closed";});
    } else {
        ui->lwMessages->addItem("Can't connect to host");
    }
}

void clientWindow::makeDisconnection()
{
    if(socket == nullptr)
        return;
    ui->lwMessages->addItem("Disconnected from: "
                            + socket->peerAddress().toString()
                            + " Port: "
                            + QString::number(socket->peerPort()));
    socket->disconnectFromHost();
    socket->flush();
    socket->close();
    delete socket;
    socket = nullptr;
}

void clientWindow::classificate()
{
    if(!socket){
        return;
    }
    readClassFile();
    QByteArray data;
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    MessageHeader messageHead;
    messageHead.tag = tag++;
    messageHead.type = 1;
    messageHead.len = inputImage.size() * sizeof(float)*2;
    qDebug() << "len" << messageHead.len;
    ds << messageHead;
    for(int i=0; i<inputImage.size(); ++i){
        ds << inputImage[i];
    }
    int msgSize = socket->write(data);
    qDebug() << "msg size" << msgSize;
    socket->waitForBytesWritten();
}

void clientWindow::sendModel()
{
    QFile file(ui->leModelFile->text());
    if(!file.open(QIODevice::ReadOnly)){
        return;
    }

    MessageHeader messageHead;
    messageHead.tag = tag++;
    messageHead.type = 2;
    messageHead.len = file.size()+sizeof(int);
    qDebug() << "len" << messageHead.len << "filesize" << file.size();
    QByteArray data;
    QByteArray fileData{file.readAll()};
    QDataStream ds(&data, QIODevice::WriteOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds << messageHead << fileData;

//    for(auto &elem: fileData){
//        ds << elem;
//    }
    int msgSize = socket->write(data);
    socket->waitForBytesWritten();
    qDebug() << "msg size" << msgSize;


}

