#include <QtTest>
#include "../src/gossip.h"
#include "../src/message.h"
#include "../src/network.h"

class TestGossip : public QObject {
    Q_OBJECT

private slots:
    void testStatusMessage();
};

void TestGossip::testStatusMessage() {
    Network network;
    Gossip gossip(&network, "test-node");
    
    QVariantMap status = gossip.createStatusMessage();
    
    QVERIFY(status.contains("Type"));
    QCOMPARE(status["Type"].toString(), QString("Status"));
    QVERIFY(status.contains("Want"));
}

QTEST_MAIN(TestGossip)
#include "test_gossip.moc"