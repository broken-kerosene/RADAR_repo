#include "messageprocessor.h"
#include <QDataStream>
#include <QByteArray>
#include <QIODevice>

namespace {
    const uint countRows{11};
    const uint countColumns{61};
    const uint headerSize{16};
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
        uint len = ba.mid(headerSize - sizeof(uint), headerSize).toUInt();
        if(ba.size() - headerSize == len) {
            return true;
        }
    }
    return false;
}

void MessageProcessor::rawMessageParser()
{
    QDataStream ds(&rawMessageBuffer, QIODevice::ReadOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    ds >> bufferForHeader;
    messagePayload = rawMessageBuffer.mid(headerSize, rawMessageBuffer.size());
    emit stageEnded();
}

void MessageProcessor::processHeader()
{
    switch (type){
    case static_cast<ushort>(MsgTypes::Classification):
        classificationParser();
        break;
    case static_cast<ushort>(MsgTypes::ReplaceModel):
        modelParser();
        break;
    }
}

void MessageProcessor::classificationParser()
{
    QDataStream ds(&messagePayload, QIODevice::ReadOnly);
    ds.setByteOrder(QDataStream::LittleEndian);
    std::vector<int> objectImage;
    uint size = countRows * countColumns;
    objectImage.resize(size);
    for(uint i=0; i<size; ++i){
       ds >> objectImage[i];
    }
    emit classificationComplete();
}

void MessageProcessor::modelParser()
{


    emit modelParsingComplete();
}
