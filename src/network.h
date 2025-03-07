#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QUdpSocket>
#include <QVariantMap>
#include <QHostAddress>
#include <QList>

class Network : public QObject {
    Q_OBJECT
public:
    Network(QObject* parent = nullptr);
    ~Network();

    bool initialize(quint16 port);
    void sendMessage(const QString& ipAddress, quint16 port, const QVariantMap& message);
    QHostAddress getLocalAddress() const;
    quint16 getPort() const;
    void connectToHost(const QString& host, quint16 port);
    QUdpSocket* getSocket();

signals:
    void messageReceived(const QVariantMap& message, const QString& senderIP, quint16 senderPort);
    void connectionAccepted(const QString& ipAddress, quint16 port);
    void connectionRequested(const QString& ipAddress, quint16 port);

private:
    QUdpSocket *udpSocket;
    quint16 localPort;
};

#endif // NETWORK_H