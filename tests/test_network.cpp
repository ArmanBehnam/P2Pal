#include <QtTest>
#include "network.h"
#include <QHostAddress>

class TestNetwork : public QObject {
    Q_OBJECT

private slots:
    void testSendMessage();
};

void TestNetwork::testSendMessage() {
    Network sender, receiver;
    QList<QPair<QHostAddress, quint16>> ports = receiver.discoverLocalPorts();
    if(ports.isEmpty()){
        QFAIL("No ports found");
    }
    quint16 port = ports.first().second;
    receiver.bindToPort(port);
    sender.sendMessage(QVariantMap(), QHostAddress::LocalHost, port);
    QVariantMap received = receiver.receiveMessage();
    QVERIFY(!received.isEmpty());
}

QTEST_MAIN(TestNetwork)
#include "test_network.moc"