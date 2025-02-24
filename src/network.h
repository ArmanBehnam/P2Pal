#ifndef NETWORK_H
#define NETWORK_H

#include <QUdpSocket>
#include <QVariantMap>
#include <QHostAddress>
#include <QList>

class Network {
public:
    Network();
    ~Network();

    void sendMessage(const QVariantMap &message, const QHostAddress &address, quint16 port);
    QList<QPair<QHostAddress, quint16>> discoverLocalPorts();
    void bindToPort(quint16 port);
    QVariantMap receiveMessage();
    QUdpSocket* getSocket();

private:
    QUdpSocket *udpSocket;
    quint16 localPort;
};

#endif // NETWORK_H