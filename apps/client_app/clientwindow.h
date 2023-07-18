#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class clientWindow; }
QT_END_NAMESPACE
class QTcpSocket;
class clientWindow : public QMainWindow
{
    Q_OBJECT

public:
    clientWindow(QWidget *parent = nullptr);
    ~clientWindow();

private:
    Ui::clientWindow *ui;
    QTcpSocket *socket;
    std::vector<float> inputImage;
    uint tag;

public slots:
    void choseFilePath();
    void choseModelFilePath();
    void readClassFile();
    void makeConnection();
    void makeDisconnection();
    void classificate();
    void sendModel();
};
#endif // CLIENTWINDOW_H
