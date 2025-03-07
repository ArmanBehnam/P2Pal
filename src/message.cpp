#include "message.h"

QVariantMap Message::toVariantMap() const {
    QVariantMap map;
    map["ChatText"] = ChatText;
    map["Origin"] = Origin;
    map["SequenceNumber"] = SequenceNumber;
    return map;
}

Message Message::fromVariantMap(const QVariantMap &map) {
    Message msg;
    msg.ChatText = map["ChatText"].toString();
    msg.Origin = map["Origin"].toString();
    msg.SequenceNumber = map["SequenceNumber"].toInt();
    return msg;
}