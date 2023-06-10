#include "messageprocessor.h"
#include <QDataStream>
#include <QByteArray>
#include <QIODevice>
#include <QDebug>
#include <QFile>

namespace {
    const uint countFrames{3};
    const uint countRows{11};
    const uint countColumns{61};
    const int headerSize{12};
    enum class MsgTypes:ushort{
        Classification = 1,
        ReplaceModel = 2
    };
}

MessageProcessor::MessageProcessor()
{
}

MessageProcessor::~MessageProcessor()
{
}

bool MessageProcessor::checkTcpMessageLen(const QByteArray &ba) const
{
    if (ba.size() > headerSize) {
        QByteArray baLen = ba.mid(headerSize-sizeof(uint), sizeof(uint));
        uint len;
        QDataStream stream(baLen);
        stream.setByteOrder(QDataStream::LittleEndian); // or BigEndian
        stream >> len;
        if(ba.size() - headerSize == len*2) {
            return true;
        }
    }
    return false;
}

void MessageProcessor::rawMessageParser(QByteArray &ba)
{
    QDataStream ds(&ba, QIODevice::ReadOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> bufferForHeader;
    type = bufferForHeader.mid(sizeof(uint), sizeof(uint)).toUInt();
    size  = bufferForHeader.mid(sizeof(uint)*2, sizeof(uint)).toUInt();
    qDebug() << "type" << type;
    messagePayload = bufferForHeader.mid(headerSize, bufferForHeader.size()-headerSize);

    processHeader();
}

void MessageProcessor::processHeader()
{
    switch (type){
    case static_cast<uint>(MsgTypes::Classification):
        classificationParser();
        break;
    case static_cast<uint>(MsgTypes::ReplaceModel):
        modelParser();
        break;
    }
}

void MessageProcessor::classificationParser()
{
    QDataStream ds(&messagePayload, QIODevice::ReadOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    uint size = countFrames * countRows * countColumns;
    objectImage.resize(size);
    for(uint i=0; i<size; ++i){
       ds >> objectImage[i];
    }
    qDebug() << objectImage;
    emit stageEnded();
}

void MessageProcessor::modelParser()
{
    QFile file("../MyReceivedOnnxModel.onnx");

    if(!file.open(QIODevice::WriteOnly)){
        qDebug() << file.errorString();
        return;
    }
    file.write(messagePayload);
    file.close();
    emit modelParsingComplete();
}

void MessageProcessor::replayClassMessage()
{

}

void MessageProcessor::replayFileMessage()
{

}
