#ifndef MESSAGEPROCESSOR_H
#define MESSAGEPROCESSOR_H

#include <QObject>

class MessageProcessor : public QObject
{
    Q_OBJECT
public:
    MessageProcessor();
    ~MessageProcessor();

    bool checkTcpMessageLen(const QByteArray &ba) const;

private:
    ushort type;
    QByteArray rawMessageBuffer;
    QByteArray bufferForHeader;
    QByteArray messagePayload;
    void processHeader();

signals:
    void stageEnded();
    void classificationComplete();
    void modelParsingComplete();

public slots:
    void rawMessageParser();
    void classificationParser();
    void modelParser();

};

#endif // MESSAGEPROCESSOR_H
