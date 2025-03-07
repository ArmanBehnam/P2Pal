#include <QtTest>
#include "message.h"

class TestMessage : public QObject {
    Q_OBJECT

private slots:
    void testSerialization();
};

void TestMessage::testSerialization() {
    Message msg;
    msg.ChatText = "Hello, world!";
    msg.Origin = "testOrigin";
    msg.SequenceNumber = 1;

    QVariantMap map = msg.toVariantMap();
    Message deserializedMsg = Message::fromVariantMap(map);

    QCOMPARE(msg.ChatText, deserializedMsg.ChatText);
    QCOMPARE(msg.Origin, deserializedMsg.Origin);
    QCOMPARE(msg.SequenceNumber, deserializedMsg.SequenceNumber);
}

QTEST_MAIN(TestMessage)
#include "test_message.moc"