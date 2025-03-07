#ifndef MESSAGE_H
#define MESSAGE_H

#include <QVariantMap>
#include <QString>

struct Message {
    QString ChatText;
    QString Origin;
    int SequenceNumber;

    QVariantMap toVariantMap() const;
    static Message fromVariantMap(const QVariantMap &map);
};

#endif // MESSAGE_H