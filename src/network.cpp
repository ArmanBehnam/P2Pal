#include "network.h"
#include <QNetworkInterface>

Network::Network() : udpSocket(new QUdpSocket(this)) {
    localPort = 0;
}

Network::~Network() {
    delete udpSocket;
}

void Network::sendMessage(const QVariantMap &message, const QHostAddress &address, quint16 port) {
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << message;
    udpSocket->writeDatagram(data, address, port);
}

QList<QPair<QHostAddress, quint16>> Network::discoverLocalPorts() {
    QList<QPair<QHostAddress, quint16>> ports;
    for (quint16 port = 5000; port < 5010; ++port) { // Example range
        if (udpSocket->bind(QHostAddress::LocalHost, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
            ports.append(qMakePair(QHostAddress::LocalHost, port));
            udpSocket->close();
        }
    }
    return ports;
}

void Network::bindToPort(quint16 port) {
    localPort = port;
    udpSocket->bind(QHostAddress::LocalHost, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
}

QVariantMap Network::receiveMessage() {
    QByteArray data;
    data.resize(udpSocket->pendingDatagramSize());
    QHostAddress senderAddress;
    quint16 senderPort;
    udpSocket->readDatagram(data.data(), data.size(), &senderAddress, &senderPort);
    QDataStream stream(&data, QIODevice::ReadOnly);
    QVariantMap message;
    stream >> message;
    return message;
}

QUdpSocket* Network::getSocket(){
    return this->udpSocket;
}