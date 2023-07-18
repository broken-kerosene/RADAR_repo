#ifndef MESSAGEPROCESSOR_H
#define MESSAGEPROCESSOR_H

#include <QObject>
#include "messageheaders.h"


class MessageProcessor : public QObject
{
    Q_OBJECT
public:
    MessageProcessor();
    ~MessageProcessor();

    bool checkTcpMessageLen(const QByteArray &ba) const;

private:
    uint type;
    QByteArray rawMessageBuffer;
    MessageHeader bufferForHeader;
    QByteArray messagePayload;
    void processHeader();

signals:
    void stageEnded();
    void classificationDataReady(std::vector<float> &objct);
    void classificationComplete();
    void modelParsingComplete();

public slots:
    void rawMessageParser(QByteArray &ba);
    void classificationParser();
    void modelParser();
    void replayClassMessage();
    void replayFileMessage();

public:
    std::vector<float> objectImage;
};

#endif // MESSAGEPROCESSOR_H
