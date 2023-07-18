#ifndef MESSAGEHEADERS_H
#define MESSAGEHEADERS_H
#include <QDataStream>
#include <QDebug>

struct MessageHeader
{
    uint tag;
    uint type;
    uint len;
};

//MessageHeaders overloads
inline QDataStream &operator <<(QDataStream &out, const MessageHeader &any)
{
    out << any.tag;
    out << any.type;
    out << any.len;
    return out;
}

inline QDataStream &operator >>(QDataStream &out, MessageHeader &any)
{
    out >> any.tag;
    out >> any.type;
    out >> any.len;
    return out;
}

inline QDebug operator <<(QDebug dbg, const MessageHeader &any)
{
    dbg << any.tag << any.type << any.len;
    return dbg;
}
#endif // MESSAGEHEADERS_H
