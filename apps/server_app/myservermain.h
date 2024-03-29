#ifndef MYSERVERMAIN_H
#define MYSERVERMAIN_H

//#undef slots
////#include <torch/torch.h>
//#include <torch/script.h>
//#define slots Q_SLOTS
#include <QMainWindow>
#include "messageprocessor.h"
#include "myclassifier.h"

class QTcpServer;
class QTcpSocket;

QT_BEGIN_NAMESPACE
namespace Ui { class MyServerMain; }
QT_END_NAMESPACE

class MyServerMain : public QMainWindow
{
    Q_OBJECT

public:
    MyServerMain(QWidget *parent = nullptr);
    ~MyServerMain();

public slots:
    void startServer();
    void stopServer();
    void choseModelFile();
    void readModelFile();
    void reciveTcpMsg();
//    void sendResponseMsg();

signals:
    void makeResponseMessage(short result);
    void testSend(std::vector<float> &object);

private:
    Ui::MyServerMain *ui;
    QTcpServer *serverTcp;
    QTcpSocket *clientConnection;
    MessageProcessor *message;
    QByteArray rawMessageBuffer;
    MyClassifier *classificator;
    std::vector<float> object;
};
#endif // MYSERVERMAIN_H
