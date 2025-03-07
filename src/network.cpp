#include "network.h"
#include <QDataStream>
#include <QNetworkInterface>
#include <QDebug>

Network::Network(QObject *parent) : QObject(parent), udpSocket(nullptr), localPort(0)
{
    udpSocket = new QUdpSocket(this);
}

Network::~Network()
{
    if (udpSocket) {
        udpSocket->close();
    }
}

bool Network::initialize(quint16 port)
{
    if (udpSocket->bind(QHostAddress::Any, port)) {
        localPort = udpSocket->localPort();
        
        connect(udpSocket, &QUdpSocket::readyRead, this, [this]() {
            while (udpSocket->hasPendingDatagrams()) {
                QByteArray datagram;
                datagram.resize(udpSocket->pendingDatagramSize());
                QHostAddress sender;
                quint16 senderPort;
                
                udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
                
                QDataStream stream(&datagram, QIODevice::ReadOnly);
                QVariantMap message;
                stream >> message;
                
                emit messageReceived(message, sender.toString(), senderPort);
            }
        });
        
        return true;
    }
    return false;
}

void Network::sendMessage(const QString& ipAddress, quint16 port, const QVariantMap& message)
{
    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << message;
    
    udpSocket->writeDatagram(datagram, QHostAddress(ipAddress), port);
}

QHostAddress Network::getLocalAddress() const
{
    for (const QHostAddress &address : QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && 
            address != QHostAddress::LocalHost) {
            return address;
        }
    }
    return QHostAddress::LocalHost;
}

quint16 Network::getPort() const
{
    return localPort;
}

void Network::connectToHost(const QString& host, quint16 port)
{
    // Send a connection request message
    QVariantMap request;
    request["Type"] = "ConnectionRequest";
    
    sendMessage(host, port, request);
    
    // Emit the connectionRequested signal
    emit connectionRequested(host, port);
}

QUdpSocket* Network::getSocket()
{
    return udpSocket;
}